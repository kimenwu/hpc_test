/*
 * master.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 *      Author: kimen
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <list>

#include "expadition.h"
#include "parallel.h"

using namespace std;

class parallel_master
{
private:
	int get_worker_rank(int x,int y);

	bool dispatch_walk_job(expadition &expa,int rank);
	int dispatch_all_walk_job(list<expadition> &expaditions);
	void redispatch_job(list<expadition> &expaditions,response_pdu_t &response_pdu);

	bool get_expadition_stat(list<expadition> &expaditions);
	expadition* stor_expadition(list<expadition> &expaditions,response_pdu_t &response_pdu);
	void dump_all_rest_expaditon(list<expadition> &expaditions);
	bool update_expadition(list<expadition> &expaditions,int expadition_cnt);
	expadition *find_expadition_byid(list<expadition> &expaditions,int id);

	bool pre_start(int worker_cnt);
	void stop_workers(int worker_cnt);

	bool make_walk(list<expadition> &expaditions);

protected:
	virtual int get_number_of_worker() = 0;

	virtual bool send_request_pdu(request_pdu_t *p_pdu,int rank) = 0;
	virtual bool receive_response_pdu(response_pdu_t &response_pdu) = 0;

public:
	parallel_master();
	virtual ~parallel_master();

	int run( int time,list<expadition> &expaditions,int expadition_cnt);
	void stop();
	virtual void exit() = 0;
	virtual bool init() = 0;
};

#endif /* MASTER_H_ */
