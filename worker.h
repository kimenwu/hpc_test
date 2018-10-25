/*
 * worker.h
 *
 *  Created on: 2018��10��19��
 */

#ifndef WORKER_H_
#define WORKER_H_

class parallel_worker
{
public:
	parallel_worker();
	virtual ~parallel_worker();

	int run();
	virtual bool init() = 0;
	virtual void exit() = 0;
};

#endif /* WORKER_H_ */
