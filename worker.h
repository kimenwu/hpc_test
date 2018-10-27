/*
 * worker.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 */

#ifndef WORKER_H_
#define WORKER_H_

#include "parallel.h"

class parallel_worker
{
private:
	bool process_request(request_pdu_t &request);
	void do_stop();

protected:
	virtual bool receive_request_pdu(request_pdu_t &request) = 0;

public:
	parallel_worker();
	virtual ~parallel_worker();

	int run();
	virtual bool init() = 0;
	virtual void exit() = 0;
};

#endif /* WORKER_H_ */
