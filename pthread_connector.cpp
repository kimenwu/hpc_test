#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "parallel.h"
#include "pthread_connector.h"

pthread_connector::pthread_connector()
{
	//initialize the lock used to protect the bufer
	pthread_mutex_init(&m_buf_lock,NULL);
	pthread_cond_init(&m_buf_cond,NULL);
}

pthread_connector::~pthread_connector()
{
	pthread_mutex_destroy(&m_buf_lock);
	pthread_cond_destroy(&m_buf_cond);
}

bool pthread_connector::send_to_me(char *data,unsigned int size)
{
	int buf_size = 0;
	char *buf = NULL;
	pdu_elment_t *p_pdu_el = NULL;

	if(!data || size <= 0 ){
		return false;
	}

	//alloc memory to stor the struct and the buf
	//in term of performance,we allocate two partition altogether
	buf_size = sizeof(pdu_elment_t)+size;
	buf = (char *)malloc(buf_size);
	if(!buf){
		parallel_error("alloc buffer error");
	}

	p_pdu_el = (pdu_elment_t *)buf;
	p_pdu_el->data=(char *)(p_pdu_el + 1);
	p_pdu_el->size = size;
	//copy data to buffer
	memcpy(p_pdu_el->data,data,size);
	
	//put it into the list
	pthread_mutex_lock(&m_buf_lock);
	m_buf_queue.push(p_pdu_el);

	//wakeup receive thread if needed
	if(m_buf_queue.size() == 1){
		pthread_cond_signal(&m_buf_cond);
	}
	pthread_mutex_unlock(&m_buf_lock);

	return true;		
}

bool pthread_connector::receive(char *data,int size)
{
	pdu_elment_t *p_pdu = NULL;

	pthread_mutex_lock(&m_buf_lock);
	if(m_buf_queue.empty()){
		pthread_cond_wait(&m_buf_cond,&m_buf_lock);
	}
	p_pdu = m_buf_queue.front();
	if(!p_pdu){
		parallel_error("Get the element failed\n");
		goto err;
	}

	if(p_pdu->size != size){
		parallel_error("expacted a continous memory is %d,rather than %d",
				p_pdu->size,size);
		goto err;
	}

	m_buf_queue.pop();
	pthread_mutex_unlock(&m_buf_lock);

	memcpy(data,p_pdu->data,p_pdu->size);
	free(p_pdu);

	return true;
err:
	pthread_mutex_unlock(&m_buf_lock);
	return false;
}
