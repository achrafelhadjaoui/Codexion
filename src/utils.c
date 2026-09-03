#include "head.h"

int convert_to_microsecond(int nb)
{
    return (nb * 1000);
}

long convert_to_milisecond(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return ((long)tv.tv_sec * 1000L) +
           ((long)tv.tv_usec / 1000L);
}


void get_abstime(struct timespec *abstime, int milliseconds)
{
    clock_gettime(CLOCK_REALTIME, abstime);

    abstime->tv_sec += milliseconds / 1000;
    abstime->tv_nsec += (milliseconds % 1000) * 1000000L;

    if (abstime->tv_nsec >= 1000000000L)
    {
        abstime->tv_sec++;
        abstime->tv_nsec -= 1000000000L;
    }
}

int checking_burnout(CODER *coder)
{
    long current_time;
    long time_since_compile;

    current_time = convert_to_milisecond();

    time_since_compile = current_time - coder->last_compile;

    if (time_since_compile >= coder->time_to_burnout)
    {
        printf("%ld %d burned out\n", convert_to_milisecond() - coder->simulation->start_time, coder->id);
        return (1);
    }
    return (0);
}