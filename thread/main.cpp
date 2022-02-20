#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

#define NUM_THREADS 10

void* say_hello(void* args){
	int thread_id = *(static_cast<int*>(args));
	if (thread_id == 4)
	{
		pthread_exit(NULL);
	}
	sleep(1);
	printf("create thread: %d\n", thread_id);
	return 0;
}

int main(int argc, char const *argv[])
{
	pthread_t tids[NUM_THREADS];
	int indexes[NUM_THREADS];
	pthread_attr_t attr;
	void *status;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (int i = 0; i < NUM_THREADS; ++i)
	{
		indexes[i] = i;
		int ret = pthread_create(&tids[i], NULL, say_hello, static_cast<void*>(&indexes[i]));
		if (ret)
		{
			cout << "pthread_create error, error_code=" << ret << endl;
		}
	}

	pthread_attr_destroy(&attr);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		int rc = pthread_join(tids[i], &status);
		cout << "main thread, complete thread " << i << endl;
	}
	
	printf("main thread finish.\n");
	pthread_exit(NULL);
}