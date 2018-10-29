/*
 * pthread_master.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 *      Author: kimen
 */

#ifndef PTHREAD_MASTER_H_
#define PTHREAD_MASTER_H_

#include <vector>

#include "pthread_worker.h"
#include "master.h"
#include "pthread_connector.h"

using namespace std;
class pthread_worker;

class pthread_master:public parallel_master
{
private:
	//For pthread mode,we must create workers manualy.the responsibility of this
	//job is master
	typedef vector<pthread_worker*> worker_vector;
	worker_vector workers;
	pthread_connector connector;

private:
	bool create_workers();
	void exit_workers();
	pthread_worker *get_worker_by_id(unsigned int rank);

public:
	pthread_master();
	virtual ~pthread_master();
	virtual void exit();
	virtual bool init();
	virtual int get_number_of_worker();
	virtual bool send_request_pdu(request_pdu_t* p_req, unsigned int bank);
	virtual bool receive_response_pdu(response_pdu_t &response_pdu);

	bool send_to_me(response_pdu_t *p_response);
};

#endif /* PTHREAD_MASTER_H_ */
