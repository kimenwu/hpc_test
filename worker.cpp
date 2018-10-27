/*
 * worker.cpp
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 */


#include "worker.h"


parallel_worker::parallel_worker()
{

}

parallel_worker::~parallel_worker()
{

}

bool parallel_worker::process_request(request_pdu_t &request)
{
	bool should_stop = false;

	switch(request.op){
		case REQUEST_DO_WALK:
			break;
		case REQUEST_PRE_START:
			break;
		case REQUEST_STOP:
//			do_stop();
			should_stop = true;
			break;
		default:
			parallel_error("could not recognise opcode:%d",request.op);
			break;
	}

	return should_stop;
}

/**
 * run until be stop by master
 */
int parallel_worker::run()
{
	int err_code = 0;
	bool should_stop = false;
	request_pdu_t request;

	while(!should_stop){

		if(!this->receive_request_pdu(request)){
			parallel_error("receive pdu error");
			continue;
		}

		should_stop = process_request(request);
	}

	return err_code;
}
