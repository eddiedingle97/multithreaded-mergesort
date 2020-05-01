#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

struct threadparams
{
    int *list;
    int size;
    int *threads;
    int *threadsused;
    int level;
    sem_t *sem;
};

static void mergesort_f(int *list, int size);
static void *mergesort_thread(void *params);
static int getthreads(struct threadparams *p);
static void combine(int *list, int mid, int size);

void mergesort(int *list, int size, int threads)
{
    if(size == 1)
        return;
    
    if(threads > 1)
    {
        int *threadsused = malloc(sizeof(int));
        *threadsused = 1;

        char isodd = size & 1;
        int newsize = size >> 1;

        pthread_t left;
        sem_t *sem;
        sem_init(sem, 0, 1);

        struct threadparams pleft;
        pleft.list = list;
        pleft.size = newsize;
        pleft.threads = &threads;
        pleft.threadsused = threadsused;
        pleft.level = 0;
        pleft.sem = sem;

        struct threadparams pright;
        pright.list = list + newsize;
        pright.size = isodd ? newsize + 1 : newsize;
        pright.threads = &threads;
        pright.threadsused = threadsused;
        pright.level = 0;
        pright.sem = sem;

        threads--;

        pthread_create(&left, NULL, mergesort_thread, &pleft);
        mergesort_thread(&pright);

        pthread_join(left, NULL);

        combine(list, newsize, size);

        sem_destroy(sem);
        free(threadsused);
    }

    else
    {
        mergesort_f(list, size);
    }
}

static void mergesort_f(int *list, int size)
{
    if(size < 2)
        return;

    char isodd = size & 1;
    int newsize = size >> 1;

    mergesort_f(list, newsize);
    mergesort_f(list + newsize, isodd ? newsize + 1 : newsize);

    combine(list, newsize, size);
}

static void *mergesort_thread(void *params)
{
    struct threadparams *p = (struct threadparams *)params;

    if(p->size < 2)
        return NULL;

    char threads = getthreads(p);

    if(!threads)
    {
        mergesort_f(p->list, p->size);
    }
    else
    {
        char isodd = p->size & 1;
        int newsize = p->size >> 1;

        pthread_t thread;

        struct threadparams pl;
        pl.list = p->list;
        pl.size = newsize;
        pl.threads = p->threads;
        pl.threadsused = p->threadsused;
        pl.level = p->level + 1;
        pl.sem = p->sem;

        struct threadparams pr;
        pr.list = p->list + newsize;
        pr.size = isodd ? newsize + 1 : newsize;
        pr.threads = p->threads;
        pr.threadsused = p->threadsused;
        pr.level = p->level + 1;
        pr.sem = p->sem;

        pthread_create(&thread, NULL, mergesort_thread, &pl);
        mergesort_thread(&pr);

        pthread_join(thread, NULL);

        combine(p->list, newsize, p->size);
    }

    return NULL;
}

static int getthreads(struct threadparams *p)
{
    int out = 0;
    sem_wait(p->sem);
        char total = *p->threads + *p->threadsused;

        if(*p->threads > 0 && total >= (1 << p->level))
        {
            out++;
            *p->threads--;
            *p->threadsused++;
        }

    sem_post(p->sem);

    return out;
}

static void combine(int *list, int mid, int size)
{
    int i = 0, j = mid, ptr = 0;
    int new[size];

    while(i < mid && j < size)
    {
        if(list[i] <= list[j])
            new[ptr++] = list[i++];

        else if(list[j] < list[i])
            new[ptr++] = list[j++];
    }

    if(i == mid)
        while(j < size)
            new[ptr++] = list[j++];

    if(j == size)
        while(i < mid)
            new[ptr++] = list[i++];

    for(i = 0; i < size; i++)
    {
        list[i] = new[i];
    }
}