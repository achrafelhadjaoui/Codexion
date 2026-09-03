#include "../head.h"

void debuging_and_refactoring(CODER *coder)
{
    pthread_mutex_lock(&coder->simulation->logging_mutex);
    printf("%d %d is debugging\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);
    pthread_mutex_unlock(&coder->simulation->logging_mutex);
    usleep(convert_to_microsecond(coder->time_to_debug));
    pthread_mutex_lock(&coder->simulation->logging_mutex);
    printf("%d %d is refactoring\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);
    pthread_mutex_unlock(&coder->simulation->logging_mutex);
    usleep(convert_to_microsecond(coder->time_to_refac));
  
}