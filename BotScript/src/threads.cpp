
//c++ -std=c++11 -O3 -pthread -o threads.out threads.cpp;./threads.out

#ifndef MAIN
	#define MAIN 4
	int main();
#endif


#include <pthread.h> //pthread_t, pthread_create, pthread_join, pthread_cancel
#include "extern.h"

namespace Thread {

	const uint8_t max = 250; //max threads
	bool active[max];        //active state
	pthread_t thread[max];   //threads


		//creates new thread and return thread-id
	uint8_t create(void *(*foo) (void *), void* arg) {
		uint8_t id(0), state;

			//find inactive thread
		while(active[id]) id++;

			//create thread
		state = pthread_create(&thread[id], NULL, foo, arg);

			//succes: state = 0, otherwise error (ex. 11)
		if(state) error("error %i while creating thread %i\n", state, id);

			//update active state
		active[id] = true;
		return id;
	}

		//return void-pointer of thread result
	void* result(uint8_t id) {
		void* result;

			//"call" thread
		pthread_join(thread[id], &result);
		return result;
	}

	void cancel(uint8_t id) {
			//cancel thread
		pthread_cancel(thread[id]);

			//update active state
		active[id] = false;
	}
};


#if MAIN == 4

/* example: comparison of filling large array *\
	0: with standard for-loop
		~ 2.0 sec

	1: with pthreads
		THD=001: ~ 2.0 sec
		THD=002: ~ 1.4 sec
		THD=010: ~ 0.9 sec
		THD=050: ~ 1.0 sec
		THD=100: ~ 1.1 sec
*/

#define state 1
#define NUM 150000000
int arr[NUM];

#if state == 0
	int main() {
		printf("start\n");

		for(uint32_t i = 0; i<NUM; i++)
			arr[i] = i;

		printf("finished\n");
	}
#else
	#define THD 10
	#define RNG NUM/THD

	void* fill(void* arg) {
		uint32_t i = (uint32_t)arg;
		uint32_t t = i + RNG;
		while(i < t) arr[i] = i, i++;
	}

	int main() {
		printf("start\n");

		//create threads
		for(uint32_t i = 0; i < THD; i++)
			Thread::create(&fill, (void*)(i*RNG));

		//call threads
		for(uint8_t i = 0; i < THD; i++) {
			Thread::result(i);
			//Thread::cancel(i);
		}

		printf("ready!\n");

		//check array
		for(uint32_t i = 0; i < NUM; i++)
			if(i != arr[i]) error("wrong array item at %i: %i", i, arr[i]);

		printf("all right!\n");
		return 0;
	}
#endif

#endif
