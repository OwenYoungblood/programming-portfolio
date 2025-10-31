#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include "hfactor.h"

int hfactor_process(uint64_t target)
{
    mqd_t mq;
    sem_t *sem;
    int shm;
    ssize_t bytes;
    unsigned int prio;
    uint64_t data;
    mq = mq_open(QUEUE_NAME, O_RDONLY, 0600);
    if(mq < 0)
    {
        perror("mq_open");
        return -1;
    }
    shm = shm_open(SHM_NAME, O_RDWR | O_EXCL, 0600);
    if(shm == -1)
    {
        perror("shm_open");
        return -1;
    }
    sem = sem_open(SEMAPHORE_NAME, O_EXCL | O_RDWR, 0600);
    if(sem == NULL)
    {
        perror("sem_open");
        return -1;
    }
    for(;;)
    {
        bytes = mq_receive(mq, (char *)&data, sizeof(data), &prio);
        if(bytes < 0)
        {
            perror("mq_receive");
            break;
        }
        else if(data == 0)
        {
            break;
        }
        else if(target % data == 0)
        {
            sem_wait(sem);
            uint64_t next = 0;
            pread(shm, &next, sizeof(next), 0);
            ftruncate(shm, 8 + (next + 1) * 8);
            pwrite(shm, &data, sizeof(data), 8 + next * 8);
            next++;
            pwrite(shm, &next, sizeof(next), 0);
            sem_post(sem);
        }
    }
    mq_close(mq);
    sem_close(sem);
    close(shm);
    return 0;
}

int hfactor_init(mqd_t *q, sem_t **sem, int *shm)
{
    struct mq_attr attr;
    mqd_t mq;
    sem_t *semaphore;
    int new_shm;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(uint64_t);
    attr.mq_curmsgs = 0;
    mq = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT, 0600, &attr);
    if(mq < 0)
    {
        perror("mq_open");
        return -1;
    }
    new_shm = shm_open(SHM_NAME, O_RDWR | O_EXCL | O_CREAT, 0600);
    if(new_shm == -1)
    {
        perror("shm_open");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        return -1;
    }
    if(ftruncate(new_shm, sizeof(uint64_t)) == -1)
    {
        perror("ftruncate");
        mq_close(mq);
        close(new_shm);
        mq_unlink(QUEUE_NAME);
        shm_unlink(SHM_NAME);
        return -1;
    }
    uint64_t zero = 0;
    if(pwrite(new_shm, &zero, sizeof(zero), 0) != sizeof(zero))
    {
        perror("pwrite");
        mq_close(mq);
        close(new_shm);
        mq_unlink(QUEUE_NAME);
        shm_unlink(SHM_NAME);
        return -1;
    }
    semaphore = sem_open(SEMAPHORE_NAME, O_EXCL | O_RDWR | O_CREAT, 0600, 1);
    if(semaphore == NULL)
    {
        perror("sem_open");
        mq_close(mq);
        close(new_shm);
        mq_unlink(QUEUE_NAME);
        shm_unlink(SHM_NAME);
        return -1;
    }
    *q = mq;
    *sem = semaphore;
    *shm = new_shm;
    return 0;
}

void hfactor_deinit(mqd_t q, sem_t *sem, int shm)
{
    mq_close(q);
    close(shm);
    sem_close(sem);
    mq_unlink(QUEUE_NAME);
    shm_unlink(SHM_NAME);
    sem_unlink(SEMAPHORE_NAME);
}

int hfactor_report(const char *reporter, int shm)
{
    pid_t pid;
    int status;
    int mypipes[2];
    pipe(mypipes);
    pid = fork();
    if(pid == 0)
    {
        dup2(mypipes[0], STDIN_FILENO);
        close(mypipes[0]);
        close(mypipes[1]);
        execlp(reporter, reporter, NULL);
        perror("execlp");
        return -1;
    }
    else
    {
        close(mypipes[0]);
        uint64_t size = 0;
        pread(shm, &size, sizeof(size), 0);
        ssize_t byte_size = write(mypipes[1], &size, sizeof(size));
        if(byte_size != sizeof(size))
        {
            perror("pwrite");
            close(mypipes[1]);
            return -1;
        }
        for(uint64_t i = 0; i < size; i++)
        {
            uint64_t pot_factor = 0;
            pread(shm, &pot_factor, sizeof(pot_factor), 8 + i * 8);
            byte_size = write(mypipes[1], &pot_factor, sizeof(pot_factor));
            if(byte_size != sizeof(pot_factor))
            {
                perror("pwrite");
                close(mypipes[1]);
                return -1;
            }
        }
        close(mypipes[1]);
        waitpid(pid, &status, 0);
    }
    return 0;
}

int hfactor_calculate(const char *reporter, uint64_t target, uint64_t num_workers)
{
    mqd_t mq;
    sem_t *sem;
    int shm;
    int status;
    unsigned int prio = 0;
    int init_result = hfactor_init(&mq, &sem, &shm);
    if(init_result == -1)
    {
        perror("hfactor_init");
        return -1;
    }
    pid_t *pids = malloc(sizeof(pid_t) * num_workers);
    for(uint64_t i = 0; i < num_workers; i++)
    {
        pids[i] = fork();
        if(pids[i] == 0)
        {
            int result = hfactor_process(target);
            exit(result);
        }
    }
    for(uint64_t i = 2; i <= target/2; i++)
    {
        int result = mq_send(mq, (char *)&i, sizeof(i), prio);
        if(result == -1)
        {
            perror("mq_send");
            return -1;
        }
    }
    uint64_t quit = 0;
    for(uint64_t i = 0; i < num_workers; i++)
    {
        int result = mq_send(mq, (char *)&quit, sizeof(quit), prio);
        if(result == -1)
        {
            perror("mq_send");
            return -1;
        }
    }
    for(uint64_t i = 0; i < num_workers; i++)
    {
        waitpid(pids[i], &status, 0);
    }
    int factor_result = hfactor_report(reporter, shm);
    if(factor_result == -1)
    {
        perror("hfactor_report");
        hfactor_deinit(mq, sem, shm);
        return -1;
    }
    free(pids);
    hfactor_deinit(mq, sem, shm);
    return 0;
}
