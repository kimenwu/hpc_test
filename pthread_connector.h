#include <pthread.h>
#include <queue>

#ifndef __PTHREAD_CONNECTOR__
#define __PTHREAD_CONNECTOR__
using namespace std;


typedef struct pdu_elment_s {
	int size;
	char *data; /*the data stor in the buffer*/		
} pdu_elment_t;

class pthread_connector
{
private:
	pthread_mutex_t m_buf_lock;
	pthread_cond_t m_buf_cond;
	queue< pdu_elment_t* > m_buf_queue;

public:
	pthread_connector();
	~pthread_connector();
	bool send_to_me(char *data,unsigned int size);
	bool receive(char *data,int size);
};

#endif
