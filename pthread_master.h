/*
 * pthread_master.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 *      Author: kimen
 */

#ifndef PTHREAD_MASTER_H_
#define PTHREAD_MASTER_H_
#include <vector>

#include "master.h"
#include "pthread_worker.h"

using namespace std;

class pthread_master:public parallel_master
{
private:
	//For pthread mode,we must create workers manualy.the responsibility of this
	//job is master
	typedef vector< pthread_worker * > worker_vector;
	worker_vector workers;

private:
	bool create_workers();
	void exit_workers();

public:
	pthread_master();
	virtual ~pthread_master();
	virtual void exit();
	virtual bool init();
	virtual int get_number_of_worker();
	virtual bool send_request_pdu(request_pdu_t*, int);
	virtual bool receive_response_pdu(response_pdu_t &response_pdu);
};

#endif /* PTHREAD_MASTER_H_ */
