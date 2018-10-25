/*
 * master.cpp
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 */
#include "master.h"
#include "parallel.h"

parallel_master::parallel_master()
{

}

parallel_master::~parallel_master()
{

}

int parallel_master::get_number_of_worker()
{
#ifdef PTHREAD_TEST
	return PTHREAD_WORKER_CNT;
#endif

	return 0;
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

	if(rank >¡¡rank_cnt){
		rank = rank_cnt;
	}

	return rank;
}

/**
 * dispach job to worker
 */
int parallel_master::dispatch_walk_job(expadition &expa,int rank)
{

}

bool parallel_master::receive_response(response_pdu_t &response_pdu)
{

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
		if(!dispatch_walk_job(expa,rank){
			parallel_error("dispatch job error",expa.m_x,expa.m_y);
			ret = false;
			goto out;
		}
	}

out:
	return ret;
}


bool parallel_master::get_rest_expadition()
{
	bool ret = true;
	request_pdu_t pdu;

	for(int i=1;i<=get_number_of_worker();i++)
	{
		memset(&pdu,0,sizeof(pdu))
		pdu.op = REQUEST_GET_STAT;
		this->send_request_pdu(&pdu,i);
	}
}

void parallel_master::stor_rest_expadition(response_pdu_t &response_pdu)
{

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
	int rank = 0;
	int unfinished_cnt = expadition_cnt;
	response_pdu_t response_pdu;

	while(t){
		t--;

		//schedule job to walk
		if(dispatch_all_walk_job(expaditions)){
			parallel_error("failed to dispatch all job");
			err_code = -1;
			goto out;
		}

		//wait for walking finished
		while(unfinished_cnt)
		{
			receive_response(response_pdu);
			switch(response_pdu.ret_code){
			case RESPONSE_WALK_FIN:
				unfinished_cnt--;
				break;
			case RESPONSE_RE_DISPATCH:
				redispatch_job(response_pdu);
				break;
			default:
				parallel_error("response error[ret_code:%d]",response_pdu.ret_code);
				err_code = -1;
				goto out;
				break;
			}
		}

		//Get the stat
		if(!get_rest_expadition())
		{
			parallel_error("failed to send get rest stat");
			err_code = -1;
			goto out;
		}

		//wait and get all stat
		unfinished_cnt = get_number_of_worker();
		while(unfinished_cnt){
			receive_response(response_pdu);
			switch(response_pdu.ret_code){
			case RESPONSE_CONFLICT_DETECT_FIN:
				unfinished_cnt--;
				break;
			case RESPONSE_STAT_INFO:
				stor_rest_expadition(response_pdu)
				break;
			default:
				parallel_error("response error[ret_code:%d]",response_pdu.ret_code);
				err_code = -1;
				goto out;
				break;
			}
		}

		//out put infomation
		dump_rest_expaditon();

	}

out:
	//TODO: if err_code != 0,we must quit all workers
	return err_code;
}
