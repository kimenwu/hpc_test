/*
 * master.h
 *
 *  Created on: 2018��10��19��
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
	bool dispatch_all_walk_job(list<expadition> &expaditions);
	bool get_expadition_stat(list<expadition> &expaditions);
	bool receive_response(response_pdu_t &response_pdu);
	void redispatch_job(response_pdu_t &response_pdu);
	void stor_expadition(response_pdu_t &response_pdu);
	void dump_all_rest_expaditon();
	bool pre_start(int worker_cnt);
	bool make_walk(list<expadition> &expaditions,int expadition_cnt);
	bool update_expadition(list<expadition> &expaditions,int expadition_cnt);

protected:
	virtual int get_number_of_worker() = 0;
	virtual bool send_request_pdu(request_pdu_t *p_pdu,int rank) = 0;

public:
	parallel_master();

	int run( int time,list<expadition> &expaditions,int expadition_cnt);

	virtual ~parallel_master();
	virtual void exit() = 0;

	virtual bool init() = 0;
};

#endif /* MASTER_H_ */
