
#ifndef _THREADS_HPP_
#    define _THREADS_HPP_

#    include <pthread.h> //pthread_t, pthread_create, pthread_join, pthread_cancel

#    define uint8_t unsigned char

namespace Thread {

    const uint8_t max = 250; // max threads
    bool active[max];        // active state
    pthread_t thread[max];   // threads

    // creates new thread and return thread-id
    uint8_t create(void* (*foo)(void*), void* arg);

    // return void-pointer of thread result
    void* result(uint8_t id);

    void cancel(uint8_t id);
}; // namespace Thread

#endif //_THREADS_CPP_

/*
#if MAIN == _THREADS_CPP_

 example: comparison of filling large array *\
    0: with standard for-loop
        ~ 2.0 sec
    1: with pthreads
        THD=001: ~ 2.0 sec
        THD=002: ~ 1.4 sec
        THD=010: ~ 0.9 sec
        THD=050: ~ 1.0 sec
        THD=100: ~ 1.1 sec


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
#endif //STATE == 0

#endif //MAIN == _THREADS_CPP_
*/
