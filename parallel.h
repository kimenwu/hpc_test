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
	REQUEST_RE_DISPATCH,
	REQUEST_GET_STAT,

}request_op_code_t;

typedef struct request_pdu_s
{
	request_op_code_t op;
}request_pdu_t;

typedef enum response_code_s
{
	RESPONSE_WALK_FIN = 1,
	RESPONSE_CONFLICT_DETECT_FIN,
	RESPONSE_RE_DISPATCH,
	RESPONSE_STAT_INFO,
} response_code_t;

/**
 * response pdu is a pdu(protocal data unit) for represeting the data
 * transfering from worker to master indicate the process result.
 */
typedef struct response_pdu_s
{
	int ret_code;
} response_pdu_t;


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
