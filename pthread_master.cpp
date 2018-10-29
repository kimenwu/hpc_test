/*
 * pthread_master.cpp
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 *      Author: kimen
 */
#include "parallel.h"
#include "pthread_master.h"
#include "pthread_worker.h"

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
		workers.erase(it);
		delete p_worker;
		p_worker = NULL;
	}
}

bool pthread_master::create_workers()
{
	bool ret = true;
	pthread_worker *p_worker;

	for(int idx = 0;idx < PTHREAD_WORKER_CNT;idx++ ){
		p_worker = new pthread_worker();
		p_worker->set_master(this);

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

pthread_worker *pthread_master::get_worker_by_id(unsigned int id)
{
	if(id > workers.size()){
		parallel_error("bad rank[bank:%d]",id);
		return NULL;
	}

	//the work index start from 1
	return workers[id-1];
}

bool pthread_master::send_request_pdu(request_pdu_t *p_req, unsigned int rank)
{
	int size = 0;
	pthread_worker *p_worker = NULL;

	p_worker = get_worker_by_id(rank);

	if(p_req->obj_cnt > 1){
		size = sizeof(request_pdu_t)+(p_req->obj_cnt-1)*sizeof(expadition_attribute_t);
	}else{
		size = sizeof(request_pdu_t);
	}
	return p_worker->send_to_me((char *)p_req,size);
}

bool pthread_master::receive_response_pdu(response_pdu_t &response_pdu)
{
	//We assume that all the response pdus have the same size
	return connector.receive((char *)&response_pdu,(int)sizeof(response_pdu_t));
}

bool pthread_master::send_to_me(response_pdu_t *p_response)
{
	return connector.send_to_me((char *)p_response,sizeof(response_pdu_t));
}

#endif
