#include <iostream>
#include <list>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

#include "parallel.h"
#include "config.h"
#include "master.h"
#include "worker.h"
#include "pthread_worker.h"

#ifdef PTHREAD_TEST
#include "pthread_master.h"
#endif

using namespace std;

enum DIRC {u,l,lu,ld,d,r,rd,ru};

class Piece{
	public:
		Piece(int inx,int iny,string str,int s){
			x=inx;
			y=iny;
			speed=s;
			if(str.compare("L")==0){
				dirc=DIRC::l;
			}
			else if(str.compare("D")==0){
				dirc=DIRC::d;
			}
			else if(str.compare("U")==0){
				dirc=DIRC::u;
			}
			else if(str.compare("D")==0){
				dirc=DIRC::d;
			}
			else if(str.compare("LU")==0){
				dirc=DIRC::lu;
			}
			else if(str.compare("LD")==0){
				dirc=DIRC::ld;
			}
			else if(str.compare("RU")==0){
				dirc=DIRC::ru;
			}
			else if(str.compare("RD")==0){
				dirc=DIRC::rd;
			}
			else{}
		}
		int x;
		int y;
		DIRC dirc;
		int speed;
};

list<Piece> pieces;
unordered_map<int,vector<list<Piece>::iterator>> state;


void run(Piece &piece){
	for(int i=0;i<piece.speed;++i){
		switch(piece.dirc){
			case DIRC::u:
				if(piece.y==900){
					if(piece.x==1600){
						--piece.x;
						piece.dirc=DIRC::l;
					}
					else{
						--piece.y;
						++piece.x;
						piece.dirc=DIRC::rd;
					}
				}
				else{
					++piece.y;
				}
				break;
			case DIRC::d:
				if(piece.y==1){
					if(piece.x==1){
						++piece.x;
						piece.dirc=DIRC::r;
					}
					else{
						++piece.y;
						--piece.x;
						piece.dirc=DIRC::lu;
					}
				}
				else{
					--piece.y;
				}
				break;
			case DIRC::l:
				if(piece.x==1){
					if(piece.y==900){
						--piece.y;
						piece.dirc=DIRC::d;
					}
					else{
						++piece.x;
						++piece.y;
						piece.dirc=DIRC::ru;
					}
				}
				else{
					--piece.x;
				}
				break;
			case DIRC::r:
				if(piece.x==1600){
					if(piece.y==1){
						++piece.y;
						piece.dirc=DIRC::u;
					}
					else{
						--piece.x;
						--piece.y;
						piece.dirc=DIRC::ld;
					}
				}
				else{
					++piece.x;
				}
				break;
			case DIRC::lu:
				if(piece.x==1||piece.y==900){
					++piece.x;
					piece.dirc=DIRC::r;
				}
				else{
					--piece.x;
					++piece.y;
				}
				break;
			case DIRC::ld:
				if(piece.x==1||piece.y==1){
					++piece.y;
					piece.dirc=DIRC::u;
				}
				else{
					--piece.x;
					--piece.y;
				}
				break;
			case DIRC::ru:
				if(piece.x==1600||piece.y==900){
					--piece.y;
					piece.dirc=DIRC::d;
				}
				else{
					++piece.x;
					++piece.y;
				}
				break;
			case DIRC::rd:
				if(piece.x==1600||piece.y==1){
					--piece.x;
					piece.dirc=DIRC::l;
				}
				else{
					++piece.x;
					--piece.y;
				}
				break;
			default:
				break;
		}
	}
}

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

int main(int argc,char **argv){
	config cfg;
	int errcode = 0;
	bool b_master = false;
	parallel_master *p_master = NULL;
	parallel_worker *worker = NULL;

	cfg.load_config(DEFAULT_CONFIG_PATH);
	int T = cfg.get_run_period();
	b_master = is_master();
	if(b_master){

		p_master = create_master();
		if(!p_master){
			parallel_error("create master failed");
			errcode = -1;
			goto err;
		}

		errcode = p_master->run(T,cfg.get_expaditions(),cfg.get_expaditon_cnt());
	}else{
		//else the thread is worker
		worker = create_worker();
		if(!worker){
			parallel_error("create worker error");
			errcode = -1;
			goto err;
		}
		errcode = worker->run();
	}

	if(b_master){
		exit_master(p_master);
		delete p_master;
		p_master = NULL;
	}
err:
	return errcode;

	ifstream input("input.txt");
	int tmpx,tmpy,tmps;
	string str;
	input >> T;
	while(input >> tmpx){
		input >> tmpy >> str >> tmps;
		pieces.emplace_back(tmpx,tmpy,str,tmps);
	}
	input.close();

	return 0;
}

