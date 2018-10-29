/*
 * master.cpp
 *
 */
#include <string.h>

#include "master.h"
#include "parallel.h"

parallel_master::parallel_master()
{
	m_curr_rank = 1;
	m_state.clear();
}

parallel_master::~parallel_master()
{

}

/**
 * This function is used to caculate which worked to
 * handle this expaditon.
 *
 * We use round-robin mechanism to schedule all job
 */
int parallel_master::get_worker_rank()
{
	int rank_cnt = get_number_of_worker();
	int rank = m_curr_rank;
	m_curr_rank++;
	if( m_curr_rank > rank_cnt ){
		m_curr_rank = 1;
	}

	return rank;
}

/**
 * dispach job to worker
 */
bool parallel_master::dispatch_walk_job(expadition &expa,int rank)
{
	request_pdu_t request;
	expadition_attribute_t *p_obj = &request.obj;

	memset(&request,0,sizeof(request));

	request.obj_cnt		= 1;
	p_obj->direction	= expa.m_direction;
	p_obj->speed		= expa.m_speed;
	p_obj->id			= expa.m_id;
	p_obj->x			= expa.m_x;
	p_obj->y			= expa.m_y;
	p_obj->status		= expa.m_status;
	p_obj->indx			= 0;

	request.op			= REQUEST_DO_WALK;

	if(!this->send_request_pdu(&request,rank)){
		parallel_error("send request error[rank:%d]",rank);
		return false;
	}
	return true;
}

int parallel_master::dispatch_all_walk_job(list<expadition> &expaditions)
{
	int rank = 0;
	int scheduled_cnt = 0;

	m_curr_rank = 1;
	for(auto it = expaditions.begin();it != expaditions.end();it++){
		expadition expa = *it;

		/*If the expaditon is dead,we need not schedule*/
		if(!(expa.m_status & EXP_STATUS_LIVE) ){
			continue;
		}

		rank = get_worker_rank();
		if(!dispatch_walk_job(expa,rank)){
			parallel_error("dispatch job error[x:%d,y:%d]",expa.m_x,expa.m_y);
			goto out;
		}

		scheduled_cnt++;
	}

out:
	return scheduled_cnt;
}

expadition*
parallel_master::find_expadition_byid(list<expadition> &expaditions,int id)
{
	expadition *p_item = NULL;
	//for(auto it = expaditions.begin();it != expaditions.end();it++){
	for(auto& exp:expaditions){
//		expadition &exp = *it;
		if(id == exp.get_id()){
			p_item = &exp;
			break;
		}
	}

	return p_item;
}

expadition*
parallel_master::stor_expadition(list<expadition> &expaditions,response_pdu_t &response_pdu)
{
	expadition *p_expa = NULL;
	expadition_attribute_t *p_obj;

	//In this function assume that only one obj in a response.
	//find the expadition
	p_obj = &response_pdu.obj;
	p_expa = find_expadition_byid(expaditions,p_obj->id);
	if(!p_expa){
		parallel_error("could find expadition %d",p_obj->id);
		goto out;
	}

	p_expa->m_speed		= p_obj->speed;
	p_expa->m_x			= p_obj->x;
	p_expa->m_y			= p_obj->y;
	p_expa->m_direction = p_obj->direction;
	p_expa->m_status	|= p_obj->status;

out:
	return p_expa;
}

bool parallel_master::pre_start(int worker_cnt)
{
	request_pdu_t request;
	response_pdu_t response;
	int unfinished = worker_cnt;

	memset(&request,0,sizeof(request));
	request.op = REQUEST_PRE_START;

	for(int i=1;i<=worker_cnt;i++){
		if(!this->send_request_pdu(&request,i)){
			parallel_error("send pdu error");
			return false;
		}
	}

	while(unfinished)
	{
		memset(&response,0,sizeof(response));

		if(!this->receive_response_pdu(response)){
			parallel_error("failed to receive response");
			return false;
		}

		if(response.ret_code == RESPONSE_READY){
			unfinished--;
		}else{
			parallel_error("get unexpected response[ret_code:%d]",response.ret_code);
			return false;
		}
	}

	return true;
}

/**
 * save_stat - save the stat to @m_stat member.
 *
 * This is called when the worker complete a expaditon walk in a second
 */
void parallel_master::save_state(expadition *p_exp)
{
	int idx = p_exp->m_y*MAX_X_RANGE + p_exp->m_x;

	if(m_state.find(idx) == m_state.end()){
		vector<expadition *> tmpvec{p_exp};
		m_state.insert(make_pair(idx,tmpvec));
	}else{
		m_state[idx].push_back(p_exp);
	}

}

/***
 * make_walk - dispatch all walking job to the work and wait for the walk finished.
 */
bool parallel_master::make_walk(list<expadition> &expaditions)
{
	bool ret = true;
	int unfinished_cnt = 0;
	response_pdu_t response;
	expadition *p_exp = NULL;

	//schedule job to walk
	unfinished_cnt = dispatch_all_walk_job(expaditions);
	if ( 0 == unfinished_cnt ) {
		parallel_error("failed to dispatch all job");
		ret = false;
		goto out;
	}

	parallel_debug("send all job need to be process[%d]",unfinished_cnt);

	// wait for walking finished
	//
	// TODO: If there is an error occur on a worker,the expadintion belong to
	// it will be lost and the process will block there forever.we must disposal
	// this function by timeout mechanism.
	while (unfinished_cnt) {
		memset(&response,0,sizeof(response));

		if(!this->receive_response_pdu(response)){
			parallel_error("receive pdu error");
			ret = -1;
			break;
		}

		switch (response.ret_code) {
			case RESPONSE_WALK_FIN:
				unfinished_cnt--;

				p_exp = stor_expadition(expaditions,response);
				if(!p_exp){
					parallel_error("failed to stor expadition");
					break;
				}

				parallel_debug("receive response:%d,unfinished:%d",response.obj.id,unfinished_cnt);
				save_state(p_exp);
				break;
			default:
				parallel_error("response error[ret_code:%d]",
					        response.ret_code);
				ret = false;
				break;
		}
	}

out:
	return ret;
}

void parallel_master::dump_all_rest_expaditon(list<expadition> &expaditions)
{
	printf("id\tx\ty\tspeed\tdirection\n");
	for(auto it = expaditions.begin();it != expaditions.end();it++){
		expadition &exp = *it;
		if(!(exp.m_status & EXP_STATUS_LIVE)){
			continue;
		}
		printf("%d\t%d\t%d\t%d\t%s\n",exp.m_id,exp.m_x,exp.m_y,exp.m_speed,
				exp.direction_tostring().c_str());
	}
}

bool parallel_master::conflict_detect()
{
	bool ret = true;
	request_pdu_t *p_req = NULL;
	request_pdu_t req_next_size;
	int attr_size = 0;
	expadition_attribute_t *p_attr=NULL;
	int rank = 0;
	int unfinished = 0;
	response_pdu_t response;

	//reset rank number
	m_curr_rank = 1;
	for(auto item:m_state){
		memset(&req_next_size,0,sizeof(req_next_size));
		rank = get_worker_rank();
		req_next_size.op = REQUEST_NEXT_PDU_SIZE;
		req_next_size.obj_cnt = item.second.size();

		if(!this->send_request_pdu(&req_next_size,rank)){
			parallel_error("send request error[rank:%d]",rank);
			ret = false;
			goto out;
		}

		attr_size = (item.second.size()-1)*sizeof(expadition_attribute_t);
		p_req = (request_pdu_t *)malloc(sizeof(request_pdu_t)+attr_size);
		memset(p_req,0,sizeof(request_pdu_t)+attr_size);
		p_req->op = REQUEST_CONFLICT_DETECT;
		p_req->obj_cnt = item.second.size();
		p_req->index = item.first;;

		p_attr = p_req->objs;
		p_attr->indx = item.first;
		for(auto& exp:item.second){
			exp->copy_to_attribute(p_attr);
			p_attr++;
		}

		if(!this->send_request_pdu(p_req,rank)){
			parallel_error("send request error[rank:%d]",rank);
			ret = false;
			goto out;
		}

		free(p_req);
		p_req = NULL;
		unfinished++;
	}

	//Get the result
	while(unfinished){

		if(!this->receive_response_pdu(response)){
			parallel_error("failed to receive response");
			goto out;
		}
		if(response.ret_code != RESPONSE_CONFLICT_DETECT_FIN){
			parallel_error("unexpected ret_code:%d",response.ret_code);
			continue;
		}

		//how to updatestate of expaditions
		iterator_t it = m_state.find(response.indx);
		if(it == m_state.end()){
			parallel_error("bad index[index:%d]",response.indx);
			ret = false;
			goto out;
		}

		//update the staus of the group expaditions
		for(auto& item:(*it).second){
			expadition &exp = *item;
			if(exp.m_id != response.obj.id){
				exp.m_status &= ~EXP_STATUS_LIVE;
			}else{
				exp.m_status |= EXP_STATUS_LIVE;
			}
		}

		unfinished--;
	}

out:
	return ret;
}

/**
 * run the core job.
 */
int
parallel_master::run( int time,list<expadition> &expaditions,int expadition_cnt)
{
	int err_code = 0;
	int t = time;
	int worker_count = this->get_number_of_worker();

	while(t){
		t--;
		m_state.clear();
		if(!pre_start(worker_count)){
			parallel_error("prestart error");
			err_code = -1;
			goto out;
		}

		//walk
		if(!make_walk(expaditions)){
			parallel_error("make walk error");
			err_code = -1;
			goto out;
		}

		//conflict detect
		if(!conflict_detect()){
			parallel_error("conflict detect error");
			err_code = -1;
			goto out;
		}
	}

	//out put infomation
	dump_all_rest_expaditon(expaditions);

out:
	//TODO: if err_code != 0,we must quit all workers
	return err_code;
}

void parallel_master::stop_workers(int worker_cnt)
{
	request_pdu_t request;
	response_pdu_t response;
	int unfinished = worker_cnt;

	memset(&request,0,sizeof(request));
	request.op = REQUEST_STOP;

	parallel_debug("there are %d threads need to stop",worker_cnt);
	for(int i=1;i<=worker_cnt;i++){
		if(!this->send_request_pdu(&request,i)){
			parallel_error("send pdu error");
			return;
		}
	}

	while(unfinished)
	{
		memset(&response,0,sizeof(response));

		if(!this->receive_response_pdu(response)){
			parallel_error("failed to receive response");
			return;
		}

		if(response.ret_code == RESPONSE_STOPPED){
			unfinished--;
		}else{
			//Get error response just ignore it
			parallel_error("get unexpected response[ret_code:%d]",response.ret_code);
		}
	}

	return;
}

void parallel_master::stop()
{
	stop_workers(get_number_of_worker());
}
