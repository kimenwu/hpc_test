/*
 * worker.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 */

#ifndef WORKER_H_
#define WORKER_H_

#include "parallel.h"
#include "execinfo.h"

class parallel_worker
{
private:
	int m_next_pdu_obj_cnt;
	//used for debug
	int m_privious_command;
	bool process_request(request_pdu_t &request);
	void do_stop();
	bool do_walk(request_pdu_t &request);
	void do_cleanup();
	bool do_conflict_detect(request_pdu_t &request);

protected:
	virtual bool receive_request_pdu(request_pdu_t *p_request,int size) = 0;
	virtual bool send_response_pdu(response_pdu_t &response) =0;
public:
	parallel_worker();
	virtual ~parallel_worker();

	int run();
	virtual bool init() = 0;
	virtual void exit() = 0;
};

#endif /* WORKER_H_ */
