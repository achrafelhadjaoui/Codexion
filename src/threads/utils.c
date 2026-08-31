#include "../head.h"

int convert_to_microsecond(int nb)
{
    return (nb * 1000);
}

int convert_to_milisecond()
{
    int res;
    struct timeval tv;
    gettimeofday(&tv, NULL);

    res = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return res;
}