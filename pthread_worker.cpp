/*
 * pthread_worker.cpp
 *
 *  Created on: 2018Äê10ÔÂ20ÈÕ
 */
#include "parallel.h"
#include "pthread_worker.h"
#include <errno.h>
#include <unistd.h>
#ifdef PTHREAD_TEST
pthread_worker::pthread_worker()
{
	m_pthread_id = 0;
}

pthread_worker::~pthread_worker()
{

}

void *pthread_worker::thread_routine(void *arg)
{
	pthread_worker *self = (pthread_worker *)arg;

	//run the run method belong to supperclass of it self
	parallel_debug("work thread has exit[tid:%lu]",self->get_pid());

	return NULL;
}

bool pthread_worker::init()
{
	int err = 0;
	int ret = true;

	//create thread and wait for job
	err = pthread_create(&m_pthread_id,NULL,thread_routine,this);
	if(err){
		parallel_error("create thread failed[err:%d]",err);
		ret = false;
	}else{
		parallel_debug("The thread has been created[tid:%lu]",m_pthread_id);
	}

	return ret;
}

void pthread_worker::exit()
{
	pthread_join(m_pthread_id,NULL);
	parallel_debug("The thread has exit[tid:%lu]",m_pthread_id);
	m_pthread_id = 0;
}

#endif
