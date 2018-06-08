
//$ g++ -std=c++11 -O3 -c threads.cpp

#include "threads.hpp"

#define CUSTOM_BEGIN
#include "macros.hpp"

#define BEGIN(...) BEGIN_1("Thread", __VA_ARGS__)
#define END(...) BEGIN_1("Thread", __VA_ARGS__)

// creates new thread and return thread-id
uint8_t Thread::create(void* (*foo)(void*), void* arg) {
    BEGIN("void*(*))(void*), void*");
    uint8_t id = 0, state;

    // find inactive thread
    while (active[id])
        id++;
    DEBUG("  -> id = %i", id);

    // create thread
    state = pthread_create(&thread[id], NULL, foo, arg);

    // succes: state = 0, otherwise error (ex. 11)
    if (state) error("error %i while creating thread %i\n", state, id);

    // update active state
    active[id] = true;
    END();
    return id;
}

// return void-pointer of thread result
void* Thread::result(uint8_t id) {
    BEGIN("uint8_t id=%i", id);
    void* result;

    //"call" thread
    pthread_join(thread[id], &result);
    END();
    return result;
}

void Thread::cancel(uint8_t id) {
    BEGIN("uint8_t id=%i", id);
    // cancel thread
    pthread_cancel(thread[id]);

    // update active state
    active[id] = false;
    END();
}
