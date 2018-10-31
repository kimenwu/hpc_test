/*
 * worker.cpp
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 */

#include "expadition.h"
#include "worker.h"
#include <string.h>

parallel_worker::parallel_worker()
{

}

parallel_worker::~parallel_worker()
{

}

void parallel_worker::do_stop()
{
	response_pdu_t response;
	memset(&response,0,sizeof(response));
	response.ret_code = RESPONSE_STOPPED;
	this->send_response_pdu(response);
}

bool parallel_worker::do_walk(request_pdu_t &request)
{
	response_pdu_t response;
	expadition exp(request.obj);
	exp.do_walk();

	response.ret_code 	= RESPONSE_WALK_FIN;
	response.indx 		= exp.get_indx();
	response.obj_cnt 	= DEFAULT_OBJ_CNT;

	exp.copy_to_attribute(&response.obj);
	response.obj.p_expadition = request.obj.p_expadition;

#ifdef PTHREAD_TEST
	if(request.obj.p_expadition->m_id != exp.m_id){
		parallel_error("bad id:expect:%d,but %d",request.obj.p_expadition->m_id,exp.m_id);
		PARALLEL_BUG();
	}
#endif

	if(!this->send_response_pdu(response)){
		parallel_error("send response error");
		return false;
	}

	return true;
}

void parallel_worker::do_cleanup()
{
	response_pdu_t response;
	memset(&response,0,sizeof(response));
	response.ret_code = RESPONSE_READY;

	if(!this->send_response_pdu(response)){
		parallel_error("send response error");
	}
}

bool parallel_worker::do_conflict_detect(request_pdu_t &request)
{
	int obj_cnt = request.obj_cnt;
	bool alive = true;
	int min_speed = request.objs[0].speed;
	response_pdu_t response;
	memset(&response,0,sizeof(response));
	expadition_attribute_t *attr = &request.objs[0];

	for(int i=1;i<obj_cnt;i++){
		if(request.objs[i].speed < min_speed){
			alive = true;
			min_speed = request.objs[i].speed;
			attr = &request.objs[i];
		}else if(request.objs[i].speed < min_speed){
			alive = false;
		}else{

		}
	}


	if(alive){
		expadition(*attr).copy_to_attribute(&response.obj);
	}else{
		response.obj.id = EXP_INVALID_ID;
	}
	response.ret_code = RESPONSE_CONFLICT_DETECT_FIN;
	response.indx = request.index;

	if(!this->send_response_pdu(response)){
		parallel_error("send response error");
		return false;
	}

	return true;
}

bool parallel_worker::process_request(request_pdu_t &request)
{
	bool should_stop = false;

	switch(request.op){
		case REQUEST_NEXT_PDU_SIZE:
			/*We need response for this ops*/
			m_next_pdu_obj_cnt = request.obj_cnt;
			m_privious_command = REQUEST_NEXT_PDU_SIZE;
			break;
		case REQUEST_DO_WALK:
			if(!do_walk(request)){
				parallel_error("do walk error");
			}
			m_privious_command = REQUEST_DO_WALK;
			break;
		case REQUEST_PRE_START:
			do_cleanup();
			m_privious_command = REQUEST_PRE_START;
			break;
		case REQUEST_STOP:
			do_stop();
			should_stop = true;
			m_privious_command = REQUEST_PRE_START;
			break;
		case REQUEST_CONFLICT_DETECT:
			if(!do_conflict_detect(request)){
				parallel_error("do conflict detect failed");
			}
			m_privious_command = REQUEST_CONFLICT_DETECT;
			break;
		default:
			parallel_error("could not recognise opcode:%d",request.op);
			m_privious_command = 0;
			break;
	}

	if(request.op != REQUEST_NEXT_PDU_SIZE){

		/*
		 * if the previous request is not REQUEST_NEXT_PDU_SIZE,
		 * We set m_next_pdu_obj_cnt to zero.So,
		 * m_next_pdu_obj_cnt = 0 indicate that we use the DEFAULT_OBJ_CNT.
		 */
		m_next_pdu_obj_cnt = 0 ;
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
	request_pdu_t default_request,*p_req = NULL;
	m_next_pdu_obj_cnt = DEFAULT_OBJ_CNT;
	bool need_free = false;
	int size = 0;

	while(!should_stop){
		need_free = false;
		if(m_next_pdu_obj_cnt >  DEFAULT_OBJ_CNT){
			size = sizeof(request_pdu_t)+(m_next_pdu_obj_cnt-1)*sizeof(expadition_attribute_t);
			p_req = (request_pdu_t *)malloc(size);
			if(!p_req){
				parallel_error("alloce memory error");
				err_code = -1;
				goto out;
			}
			need_free = true;
		}else{
			p_req = &default_request;
			size = sizeof(request_pdu_t);
		}

		memset(p_req,0,size);
		if(!this->receive_request_pdu(p_req,size)){
			parallel_error("receive pdu error");
			if(need_free){
				free(p_req);
				p_req = NULL;
			}

			PARALLEL_BUG();
			continue;
		}

		should_stop = process_request(*p_req);

		if(need_free){
			free(p_req);
			p_req = NULL;
		}
	}

	parallel_debug("the worker is exit")

out:
	return err_code;
}
