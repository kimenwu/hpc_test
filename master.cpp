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
	return true;
}

bool parallel_master::receive_response(response_pdu_t &response_pdu)
{
	return true;
}

void parallel_master::redispatch_job(response_pdu_t &response_pdu)
{

}


bool parallel_master::dispatch_all_walk_job(list<expadition> &expaditions)
{
	bool ret = true;
	int rank = 0;

	for(auto it = expaditions.begin();it != expaditions.end();it++){
		expadition expa = *it;
		rank = get_worker_rank(expa.m_x,expa.m_y);
		if(!dispatch_walk_job(expa,rank)){
			parallel_error("dispatch job error[x:%d,y:%d]",expa.m_x,expa.m_y);
			ret = false;
			goto out;
		}
	}

out:
	return ret;
}

void parallel_master::stor_expadition(response_pdu_t &response_pdu)
{

}

bool parallel_master::pre_start(int worker_cnt)
{
	request_pdu_t request;
	response_pdu_t response;
	request.op = REQUEST_PRE_START;
	int unfinished = worker_cnt;

	for(int i=1;i<=worker_cnt;i++){
		if(!this->send_request_pdu(&request,i)){
			parallel_error("send pdu error");
			return false;
		}
	}

	while(unfinished)
	{
		memset(&response,0,sizeof(response));
		receive_response(response);
		if(response.ret_code == RESPONSE_READY){
			unfinished--;
		}else{
			parallel_error("get unexpected response[ret_code:%d]",response.ret_code);
			return false;
		}
	}

	return true;

}


bool parallel_master::make_walk(list<expadition> &expaditions,int expadition_cnt)
{
	bool ret = true;
	int unfinished_cnt = expadition_cnt;
	response_pdu_t response;

	//schedule job to walk
	if (dispatch_all_walk_job(expaditions)) {
		parallel_error("failed to dispatch all job");
		ret = false;
		goto out;
	}

	//wait for walking finished
	while (unfinished_cnt) {
		memset(&response,0,sizeof(response));
		receive_response(response);
		switch (response.ret_code) {
			case RESPONSE_WALK_FIN:
				unfinished_cnt--;
				break;
			case RESPONSE_RE_DISPATCH:
				redispatch_job(response);
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
		receive_response(response_pdu);
		switch(response_pdu.ret_code){
			case RESPONSE_GET_STAT_FIN:
				unfinished_cnt--;
				break;
			case RESPONSE_STAT_INFO:
				stor_expadition(response_pdu);
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

void parallel_master::dump_all_rest_expaditon()
{

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
			goto out;
		}

		if(!make_walk(expaditions,expadition_cnt)){
			parallel_error("make walk error");
		}
	}

	//out put infomation
	dump_all_rest_expaditon();

out:
	//TODO: if err_code != 0,we must quit all workers
	return err_code;
}
