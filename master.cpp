/*
 * master.cpp
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 */
#include <string.h>

#include "master.h"
#include "parallel.h"

parallel_master::parallel_master()
{

}

parallel_master::~parallel_master()
{

}

/**
 * This function is used to caculate which worked to
 * handle this expaditon.
 */
int parallel_master::get_worker_rank(int x,int y)
{
	int rank_cnt = get_number_of_worker();
	int rang_per_worker = MAX_X_RANGE/rank_cnt;
	int rank = 0;

	/**
	 * rank id start from 1.
	 */
	rank = 1+x/rang_per_worker;

	if(rank > rank_cnt){
		rank = rank_cnt;
	}

	return rank;
}

/**
 * dispach job to worker
 */
bool parallel_master::dispatch_walk_job(expadition &expa,int rank)
{
	request_pdu_t request;
	request.direction	= expa.m_direction;
	request.left_step	= expa.m_left_step;
	request.id			= expa.m_id;
	request.x			= expa.m_x;
	request.y			= expa.m_y;
	request.op			= REQUEST_DO_WALK;
	request.status		= expa.m_status;

	if(!this->send_request_pdu(&request,rank)){
		parallel_error("send request error[rank:%d]",rank);
		return false;
	}
	return true;
}

void
parallel_master::redispatch_job(list<expadition> &expaditions,response_pdu_t &response_pdu)
{
	expadition *p_expa = NULL;
	int rank = 0;
	p_expa = stor_expadition(expaditions,response_pdu);
	if(p_expa == NULL ){
		parallel_error("can't find expadition[id:%d]",response_pdu.id);
		return;
	}

	rank = get_worker_rank(p_expa->m_x,p_expa->m_y);
	if(!dispatch_walk_job(*p_expa,rank)){
		parallel_error("Dispatch walk job error");
	}
}


int parallel_master::dispatch_all_walk_job(list<expadition> &expaditions)
{
	int rank = 0;
	int scheduled_cnt = 0;

	for(auto it = expaditions.begin();it != expaditions.end();it++){
		expadition expa = *it;

		/*If the expaditon is dead,we need not schedule*/
		if(!(expa.m_status & EXP_STATUS_LIVE) ){
			continue;
		}

		rank = get_worker_rank(expa.m_x,expa.m_y);
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
	for(auto it = expaditions.begin();it != expaditions.end();it++){
		expadition &exp = *it;
		if(id == exp.get_id()){
			p_item = &exp;
		}
	}

	return p_item;
}

expadition*
parallel_master::stor_expadition(list<expadition> &expaditions,response_pdu_t &response_pdu)
{
	expadition *p_expa = NULL;

	//find the expadition
	p_expa = find_expadition_byid(expaditions,response_pdu.id);
	if(!p_expa){
		parallel_error("could find expadition %d",response_pdu.id);
		goto out;
	}

	p_expa->m_left_step	= response_pdu.left_step;
	p_expa->m_x			= response_pdu.x;
	p_expa->m_y			= response_pdu.y;
	p_expa->m_direction = response_pdu.direction;
	p_expa->m_status	|= response_pdu.status;

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


/***
 * make_walk - dispatch all walking job to the work and wait for the walk finished.
 */
bool parallel_master::make_walk(list<expadition> &expaditions)
{
	bool ret = true;
	int unfinished_cnt = 0;
	response_pdu_t response;

	//schedule job to walk
	unfinished_cnt = dispatch_all_walk_job(expaditions);
	if ( 0 == unfinished_cnt ) {
		parallel_error("failed to dispatch all job");
		ret = false;
		goto out;
	}

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
				break;
			//If the x-coordinate out of the range that charged by the worker currently,
			//The worker will ask to re-dispatch.
			//That is implement by return a response with it's @ret_code equal to
			//@RESPONSE_RE_DSIPATCH
			case RESPONSE_RE_DISPATCH:
				redispatch_job(expaditions,response);
				break;
			default:
				parallel_error("response error[ret_code:%d]",
					        response.ret_code);
				ret = false;
				goto out;
				break;
		}
	}

out:
	return ret;
}

bool
parallel_master::update_expadition(list<expadition> &expaditions,int expadition_cnt)
{
	int unfinished_cnt = expadition_cnt;

	bool ret = true;
	request_pdu_t pdu;
	response_pdu_t response_pdu;

	for(int i=1;i<=get_number_of_worker();i++)
	{
		memset(&pdu,0,sizeof(pdu));
		pdu.op = REQUEST_GET_STAT;
		this->send_request_pdu(&pdu,i);
	}

	while(unfinished_cnt){

		memset(&response_pdu,0,sizeof(response_pdu));
		if(!this->receive_response_pdu(response_pdu)){
			parallel_error("receive response error");
			ret = false;
			break;
		}

		switch(response_pdu.ret_code){
			case RESPONSE_GET_STAT_FIN:
				unfinished_cnt--;
				break;
			case RESPONSE_STAT_INFO:
				stor_expadition(expaditions,response_pdu);
				break;
			default:
				parallel_error("response error[ret_code:%d]",response_pdu.ret_code);
				ret = false;
				goto out;
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
		if(!pre_start(worker_count)){
			parallel_error("prestart error");
			err_code = -1;
			goto out;
		}

		if(!make_walk(expaditions)){
			parallel_error("make walk error");
			err_code = -1;
			goto out;
		}

		if(!update_expadition(expaditions,expadition_cnt)){
			err_code = -1;
			parallel_error("Get expadition count error");
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
