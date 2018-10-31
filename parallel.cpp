#include <iostream>
#include <list>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

#include "pthread_worker.h"
#include "parallel.h"
#include "config.h"
#include "master.h"
#include "worker.h"

#ifdef PTHREAD_TEST
#include "pthread_master.h"
#endif

using namespace std;

static parallel_master *create_master()
{

	parallel_master *p_master = NULL;
#ifdef PTHREAD_TEST
	p_master = new pthread_master();
#endif
	if(p_master && !p_master->init()){
		parallel_error("init master failed");
		delete p_master;
		p_master = NULL;
	}else{
		parallel_debug("create master success");
	}

	return p_master;
}

static void exit_master( parallel_master *&p_master)
{
	if(!p_master){
		parallel_debug("the p_master pointer is null");
		return;
	}

	p_master->exit();
}

/**
 * is_master - check if the main thread is master
 * In general,the cluster has only on master to manager all workers.For *pthread*
 * mode,the main thread is the master thread,nevertheless in MPI context,there is
 * only one thread on a node/process,we specify the node whose id equal to 0 as the
 * master node.
 */
static bool is_master()
{
#ifdef PTHREAD_TEST
	return true;
#endif
	return true;
}

parallel_worker* create_worker()
{
	parallel_worker *worker = NULL;
#ifdef PTHREAD_TEST
	return worker;
#endif
	return worker;

}

config cfg;

int main(int argc,char **argv){
	int errcode = 0;
	bool b_master = false;
	parallel_master *p_master = NULL;
	parallel_worker *worker = NULL;

	parallel_debug("loading config");
	cfg.load_config(DEFAULT_CONFIG_PATH);

	int T = cfg.get_run_period();
	b_master = is_master();
	if(b_master){
		parallel_debug("create master");
		p_master = create_master();
		if(!p_master){
			parallel_error("create master failed");
			errcode = -1;
			goto err;
		}

		parallel_debug("running job");
		errcode = p_master->run(T,cfg.get_expaditions(),cfg.get_expaditon_cnt());
		if(errcode){
			parallel_error("run the paralle task error");
		}
		p_master->stop();
	}else{
		//else the thread is worker
		worker = create_worker();
		if(!worker){
			parallel_error("create worker error");
			errcode = -1;
			goto err;
		}

		//The work must stop by master.
		errcode = worker->run();
	}

	if(b_master){
		exit_master(p_master);
		delete p_master;
		p_master = NULL;
	}
err:
	return errcode;
}

