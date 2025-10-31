#include "tpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

struct ThreadPool
{
    uint64_t size;
    bool die;
    pthread_t *tids;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    const int *inputs;
    uint64_t *outputs;
    int num_work;
    Executor executor;
    int end;
    int next_work;
};
//worker function for the pthread_create function
void *worker(void *arg)
{
    struct ThreadPool *thread_pool = (struct ThreadPool *)arg;
    pthread_mutex_lock(&thread_pool->lock);
    for(;;)
    {
        while(thread_pool->next_work >= thread_pool->num_work && !thread_pool->die)
        {
            pthread_cond_wait(&thread_pool->not_empty, &thread_pool->lock);
        }
        if(thread_pool->die)
        {
            pthread_mutex_unlock(&thread_pool->lock);
            break;
        }
        //giving each thread the assigned work and having them call the executor function.
        int next = thread_pool->next_work++;
        if(next < thread_pool->num_work)
        {
            int new_val = thread_pool->inputs[next];
            pthread_mutex_unlock(&thread_pool->lock);
            uint64_t result = thread_pool->executor(new_val);
            pthread_mutex_lock(&thread_pool->lock);
            //store results and increment to next thread
            thread_pool->outputs[next] = result;
            thread_pool->end++;
            pthread_cond_broadcast(&thread_pool->not_empty);
        }
    }
    return NULL;

}
void *thread_pool_open(int num_threads)
{
    if(num_threads < 1 || num_threads > 32)
    {
        return NULL;
    }
    //thread pool structure setup.
    struct ThreadPool *thread_pool = malloc(sizeof(struct ThreadPool));
    thread_pool->tids = calloc(num_threads, sizeof(pthread_t));
    pthread_mutex_init(&thread_pool->lock, NULL);
    pthread_cond_init(&thread_pool->not_empty, NULL);
    thread_pool->size = num_threads;
    thread_pool->num_work = 0;
    thread_pool->next_work = 0;
    thread_pool->end = 0;
    thread_pool->die = false;
    for(int i = 0; i < num_threads; i++)
    {
        pthread_create(&thread_pool->tids[i], NULL, worker, thread_pool);
    }

    return (void *)thread_pool;
}
uint64_t *thread_pool_execute(void *handle, const int *work_vals, int num_work, const Executor executor)
{
    if(handle == NULL)
    {
        return NULL;
    }
    //set up thread pool with passed values.
    uint64_t *results = calloc(num_work, sizeof(uint64_t));
    struct ThreadPool *thread_pool = (struct ThreadPool *)handle;
    pthread_mutex_lock(&thread_pool->lock);
    thread_pool->outputs = results;
    thread_pool->inputs = work_vals;
    thread_pool->num_work = num_work;
    thread_pool->executor = executor;
    thread_pool->next_work = 0;
    thread_pool->end = 0;
    //signal threads and wait for threads to end before unlocking mutex.
    pthread_cond_broadcast(&thread_pool->not_empty);
    while(thread_pool->end < num_work)
    {
        pthread_cond_wait(&thread_pool->not_empty, &thread_pool->lock);
    }
    pthread_mutex_unlock(&thread_pool->lock);
    return results;
}
void thread_pool_close(void *handle)
{
    if(handle == NULL)
    {
        return;
    }
    //lock each thread and tell them to die.
    struct ThreadPool *thread_pool = (struct ThreadPool *)handle;
    pthread_mutex_lock(&thread_pool->lock);
    thread_pool->die = true;
    pthread_cond_broadcast(&thread_pool->not_empty);
    pthread_mutex_unlock(&thread_pool->lock);
    //join all threads and clean up
    for(uint64_t i = 0; i < thread_pool->size; i++)
    {
        pthread_join(thread_pool->tids[i], NULL);
    }
    pthread_mutex_destroy(&thread_pool->lock);
    pthread_cond_destroy(&thread_pool->not_empty);
    free(thread_pool->tids);
    free(thread_pool);
}
//hash features implemented based off pseudocode in lab writeup.
uint64_t hash32(int fd)
{
    uint32_t digest = 2166136261u;
    unsigned char byte;
    ssize_t read_bytes;
    lseek(fd, 0, SEEK_SET);
    while((read_bytes = read(fd, &byte, 1)) > 0)
    {
        digest ^= byte;
        digest *= 16777619u;
    }
    return (uint64_t)digest;
}
uint64_t hash64(int fd)
{
    uint64_t digest = 14695981039346656037ull;
    unsigned char byte;
    ssize_t read_bytes;
    lseek(fd, 0, SEEK_SET);
    while((read_bytes = read(fd, &byte, 1)) > 0)
    {
        digest ^= byte;
        digest *= 1099511628211ull;
    }
    return digest;
}
