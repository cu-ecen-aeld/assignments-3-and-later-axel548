// threading.c

#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_LOG(msg, ...) printf("threading: " msg "\n", ##__VA_ARGS__)
#define ERROR_LOG(msg, ...) printf("threading ERROR: " msg "\n", ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    struct thread_data* thread_func_args = (struct thread_data*)thread_param;

    // Wait for a specified time
    usleep(thread_func_args->wait_to_obtain_ms * 1000);

    // Obtain the mutex
    if (pthread_mutex_lock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to obtain mutex");
        return NULL;
    }

    // Wait for another specified time
    usleep(thread_func_args->wait_to_release_ms * 1000);

    // Release the mutex
    if (pthread_mutex_unlock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to release mutex");
        return NULL;
    }

    return thread_param;
}

bool start_thread_obtaining_mutex(pthread_t* thread, pthread_mutex_t* mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    // Allocate memory for thread_data
    struct thread_data* thread_args = (struct thread_data*)malloc(sizeof(struct thread_data));
    if (thread_args == NULL) {
        ERROR_LOG("Failed to allocate memory for thread_data");
        return false;
    }

    // Initialize thread_data structure
    thread_args->mutex = mutex;
    thread_args->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_args->wait_to_release_ms = wait_to_release_ms;

    // Create the thread
    if (pthread_create(thread, NULL, threadfunc, (void*)thread_args) != 0) {
        ERROR_LOG("Failed to create thread");
        free(thread_args);
        return false;
    }

    return true;
}
