#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;

void *thread1_start(void *arg)
{
    int *arg1 = (int *)arg;
    printf("thread1 recevice an arg is %x\n", *arg1);

    pthread_mutex_lock(&mutex1);
    printf("thread1 has locked mutex1\n");
    // ensure that mutex1 is still locked when thread2 begins to run
    sleep(3);
    printf("thread1 unlock mutex1\n");
    pthread_mutex_unlock(&mutex1);

    printf("thread1 exit\n");
}

void *thread2_start(void *arg)
{
    int *arg2 = (int *)arg;
    printf("thread2 recevice an arg is %x\n", *arg2);

    if (pthread_mutex_trylock(&mutex2) == 0)
        printf("thread2 trylock mutex2 sucess\n");

    if (pthread_mutex_trylock(&mutex1) == EBUSY)
        printf("thread2 trylock mutex1 failed cause mutex1 is hold by thread1\n");

    pthread_mutex_unlock(&mutex2);

    printf("thread2 exit\n");
}

void *thread3_start(void *arg)
{
    struct timespec time;
    struct tm *tmp_time;
    char s[64];
    int err;

    int *retvalue = (int *)malloc(8);
    *retvalue = 456;

    pthread_mutex_lock(&mutex3);
    printf("thread3 has locked mutex3\n");

    // get current time
    clock_gettime(CLOCK_REALTIME, &time);
    tmp_time = localtime(&time.tv_sec);
    strftime(s, sizeof(s), "%r", tmp_time);
    printf("current time is %s\n", s);

    // lock up to 10s
    time.tv_sec = time.tv_sec + 10;

    /*mutex3 is expected to be locked with timeout 10s*/
    if (pthread_mutex_timedlock(&mutex3, &time) == ETIMEDOUT)
        printf("thread3 pthread_mutex_timedlock(mutex3) timeout\n");

    // get current time again
    clock_gettime(CLOCK_REALTIME, &time);
    tmp_time = localtime(&time.tv_sec);
    strftime(s, sizeof(s), "%r", tmp_time);
    printf("the time is now %s\n", s);

    pthread_mutex_unlock(&mutex3);

    printf("thread3 exit\n");

    return (void *)retvalue;
}

int main()
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    int thread1_arg = 0XEFADC;
    int thread2_arg = 0XAAABB;
    int *thread3_retvalule = NULL;

    /*test for pthread_mutex_lock() and pthread_mutex_trylock()*/
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    pthread_create(&tid1, NULL, thread1_start, &thread1_arg);
    sleep(1);
    pthread_create(&tid2, NULL, thread2_start, &thread2_arg);

    if (pthread_join(tid1, NULL) == 0)
    {
        pthread_mutex_destroy(&mutex1);
    }

    if (pthread_join(tid2, NULL) == 0)
    {
        pthread_mutex_destroy(&mutex2);
    }

    /*Test for pthread_mutex_timedlock()*/
    pthread_mutex_init(&mutex3, NULL);
    pthread_create(&tid3, NULL, thread3_start, NULL);

    if (pthread_join(tid3, (void **)&thread3_retvalule) == 0)
    {
        pthread_mutex_destroy(&mutex3);
        printf("thread3 return value is %d\n", *thread3_retvalule);
        free(thread3_retvalule);
    }

    return 0;
}