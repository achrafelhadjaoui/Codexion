#include "../head.h"

static int wait_for_activity(CODER *coder, int duration)
{
    struct timespec abstime;

    get_abstime(&abstime, duration);

    pthread_mutex_lock(&coder->monitor->monitor_mutex);

    pthread_cond_timedwait(
        &coder->monitor->activity_cond,
        &coder->monitor->monitor_mutex,
        &abstime
    );

    if (coder->monitor->burnout_detected == 1)
    {
        pthread_mutex_unlock(&coder->monitor->monitor_mutex);
        return (1);
    }

    pthread_mutex_unlock(&coder->monitor->monitor_mutex);

    return (0);
}

int debuging(CODER *coder)
{
    printf("%ld %d is debugging\n",
        convert_to_milisecond() - coder->simulation->start_time,
        coder->id);

    return (wait_for_activity(coder, coder->time_to_debug));
}

int refactoring(CODER *coder)
{
    printf("%ld %d is refactoring\n",
        convert_to_milisecond() - coder->simulation->start_time,
        coder->id);

    return (wait_for_activity(coder, coder->time_to_refac));
}

int compiling(CODER *coder)
{
    printf("%ld %d is compiling\n",
        convert_to_milisecond() - coder->simulation->start_time,
        coder->id);

    return (wait_for_activity(coder, coder->time_to_compile));
}