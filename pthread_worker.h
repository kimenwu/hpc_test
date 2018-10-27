/*
 * pthread_worker.h
 *
 *  Created on: 2018Äê10ÔÂ20ÈÕ
 *      Author: kimen
 */

#ifndef PTHREAD_WORKER_H_
#define PTHREAD_WORKER_H_

#include <pthread.h>
#include "worker.h"

class pthread_worker:public parallel_worker
{
private:
	pthread_t m_pthread_id;
	static void *thread_routine(void *arg);
protected:
	virtual bool receive_request_pdu(request_pdu_t *p_request,int size);
	virtual bool send_response_pdu(response_pdu_t &response);
public:
	pthread_worker();
	~pthread_worker();

	virtual bool init();
	virtual void exit();

	pthread_t get_pid(){
		return m_pthread_id;
	}
};

#endif /* PTHREAD_WORKER_H_ */
