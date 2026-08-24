#include "../head.h"

void *routine(void *args)
{
    CODER *coder = (CODER *)args;

    while (coder->nub_of_compiles > 0)
    {
        coder->state = READY;
        
    









        // usleep(convert_to_microsecond(coder->time_to_compile));
    }
}

void thread_creation(CODER *coder)
{
    SCHEDULER *sched;

    if (pthread_create(&coder->thread, NULL, routine, coder) != 0)
    {
        exit(3);
    }
    printf("thread is created\n");
}