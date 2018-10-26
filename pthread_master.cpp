/*
 * pthread_master.cpp
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 *      Author: kimen
 */
#include "parallel.h"
#include "pthread_master.h"

using namespace std;
#ifdef PTHREAD_TEST

pthread_master::pthread_master()
{

}

pthread_master::~pthread_master()
{
	parallel_debug("this object has been deleted");
}

void pthread_master::exit()
{
	exit_workers();
}

bool pthread_master::init()
{
	bool ret = true;
	ret = create_workers();
	if(!ret){
		parallel_error("create works error");
		ret = false;
	}

	return ret;
}

int pthread_master::get_number_of_worker()
{
	return PTHREAD_WORKER_CNT;
}

void pthread_master::exit_workers()
{
	parallel_worker *p_worker = NULL;

	for(auto it = workers.begin();it != workers.end();)
	{
		p_worker = *it;
		p_worker->exit();
		delete p_worker;
		p_worker = NULL;
		workers.erase(it);
	}
}

bool pthread_master::create_workers()
{
	bool ret = true;
	pthread_worker *p_worker;

	for(int idx = 0;idx <= PTHREAD_WORKER_CNT;idx++ ){
		p_worker = new pthread_worker();
		ret = p_worker->init();
		if(!ret){
			parallel_error("create worker error");
			ret = false;
			goto err;
		}
		workers.push_back(p_worker);
	}

	return ret;
err:
	//clear all worker has been created
	exit_workers();
	return ret;
}

bool pthread_master::send_request_pdu(request_pdu_t*, int)
{
	return true;
}

bool pthread_master::receive_response_pdu(response_pdu_t &response_pdu)
{
	return true;
}

#endif
