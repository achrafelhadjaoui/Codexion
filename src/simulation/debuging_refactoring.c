#include "../head.h"

void debuging_and_refactoring(CODER *coder)
{
    printf("%d %d is debugging\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);
    usleep(convert_to_microsecond(coder->time_to_debug));
    printf("%d %d is refactoring\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);
    usleep(convert_to_microsecond(coder->time_to_refac));
}