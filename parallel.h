/*
 * parallel.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 */

#ifndef PARALLEL_H_
#define PARALLEL_H_
#include <iostream>

typedef enum request_op_code
{
	REQUEST_DO_WALK = 1,
	REQUEST_CONFLICT_DETECT,
	REQUEST_NEXT_PDU_SIZE,
	REQUEST_PRE_START,
	REQUEST_STOP,

}request_op_code_t;

typedef struct expadition_attribute{
	int id;
	int indx;
	int speed;
	int status;
	int x;
	int y;
	int direction;
} expadition_attribute_t;

typedef struct request_pdu_s
{
	request_op_code_t op;
	int obj_cnt;
	int index;
	union{
		/**
		 * If we just send a obj along with op we use @objs otherwise we use @objs
		 * instead.
		 * Note that when use objs,the memory of the request struct must be allocate
		 * dynamically.
		 */
		expadition_attribute_t obj;
		expadition_attribute_t objs[1];
	};
}request_pdu_t;

typedef enum response_code_s
{
	RESPONSE_WALK_FIN = 1,
	RESPONSE_GET_STAT_FIN,
	RESPONSE_STAT_INFO,
	RESPONSE_READY,
	RESPONSE_CONFLICT_DETECT_FIN,
	RESPONSE_STOPPED,
} response_code_t;

/**
 * response pdu is a pdu(protocal data unit) for represeting the data
 * transfering from worker to master indicate the process result.
 */
typedef struct response_pdu_s
{
	int ret_code;
	int obj_cnt;
	int indx;
	union{
		/**
		 * If we just send a obj along with op we use @objs otherwise we use @objs
		 * instead.
		 * Note that when use objs,the memory of the request struct must be allocate
		 * dynamically.
		 */
		expadition_attribute_t obj;
		expadition_attribute_t objs[0];
	};
} response_pdu_t;

enum DIRC {u,l,lu,ld,d,r,rd,ru};

#define DEFAULT_CONFIG_PATH "input.txt"
#define MAX_X_RANGE (1600)
#define MAX_Y_RANGE (900)
/**
 * PTHREAD_WORKER_CNT - Predefinition of count of the pthread.
 *
 * Since we can't get the count from configure file or the runtime environment,
 * we define it.
 */
#ifdef PTHREAD_TEST
#define PTHREAD_WORKER_CNT (5)
#endif

#define parallel_error(format,...)				\
	printf("[%s:%d]" format "\n",__FILE__,__LINE__,##__VA_ARGS__);

#ifdef DEBUG
#define parallel_debug(format,...) 				\
	printf("[%s:%d:%s]" format "\n",__FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__);
#else
#define parallel_debug(format,...)

#endif

#endif /* PARALLEL_H_ */
