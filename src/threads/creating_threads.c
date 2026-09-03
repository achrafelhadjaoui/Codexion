#include "../head.h"
static void *routine(void *args)
{
    CODER *coder;

    coder = (CODER *)args;

    // while (coder->nub_of_compiles > 0)
    // {
    //     coder->start = convert_to_milisecond();
    //     request_dongles(coder);


    //     /*
    //      * compile
    //      */
    //     pthread_mutex_lock(&coder->simulation->logging_mutex);
    //     printf("%ld %d is compiling\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);
    //     pthread_mutex_unlock(&coder->simulation->logging_mutex);

    //     usleep(convert_to_microsecond(coder->time_to_compile));
    //     coder->last_compile = convert_to_milisecond();

    //     release_dongles(coder);
    //     //printf("CODER %i RELEASE DONGLE\n", coder->id);
    //     debuging_and_refactoring(coder);

    //     coder->nub_of_compiles--;
    // }

    while (coder->nub_of_compiles > 0)
    {
        coder->start = convert_to_milisecond();

        if (request_dongles(coder))
            return NULL;

        if (compiling(coder))
        {
            release_dongles(coder);
            return (NULL);
        }

        coder->last_compile = convert_to_milisecond();

        release_dongles(coder);

        if (debuging(coder))
            return (NULL);

        if (refactoring(coder))
            return (NULL);

        coder->nub_of_compiles--;
    }

    pthread_mutex_lock(&coder->monitor->monitor_mutex);
    coder->monitor->finish_running -= 1;
    pthread_cond_signal(&coder->monitor->monitor_cond);
    pthread_mutex_unlock(&coder->monitor->monitor_mutex);

    return (NULL);
}


static void *monitor_routine(void *arg)
{
    MONITOR *monitor = (MONITOR *) arg;

    pthread_mutex_lock(&monitor->monitor_mutex);
    while ( monitor-> burnout_detected != 1)
    {
        pthread_cond_wait(&monitor->monitor_cond, &monitor->monitor_mutex);
    //     if (monitor->burnout_detected == 1 || monitor->finish_running == 0)
    //         break;
    }

    pthread_cond_broadcast(&monitor->activity_cond);

    pthread_mutex_unlock(&monitor->monitor_mutex);
    return NULL;
}

void coder_and_monitor_thread_creation(CODER *coder, int size, MONITOR *monitor, DONGLE *dongle)
{
    int i;

    i = 0;
    while (i < size)
    {
        if (pthread_create(&coder[i].thread, NULL, routine, &coder[i]) != 0)
        {
            exit(3);
        }
        i++;
    }

    if (pthread_create(&monitor->monitor_thread, NULL, monitor_routine, monitor) != 0)
        exit(3);


    i = 0;
    while (i < size)
    {
        if (pthread_join(coder[i].thread, NULL) != 0)
            exit(3);

        i++;
    }

    if (pthread_join(monitor->monitor_thread, NULL) != 0)
        exit(3);

    i = 0;
    while (i < size)
    {
        pthread_cond_destroy(&coder[i].coder_cond);
        pthread_mutex_destroy(&dongle[i].dongle_mutex);
        i++;
    }

    pthread_mutex_destroy(&monitor->monitor_mutex);
    pthread_cond_destroy(&monitor->monitor_cond);
}
