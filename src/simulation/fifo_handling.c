#include "../head.h"

void fifo_implementation(DONGLE *dongle, CODER *coder)
{
    CODER *tmp;

    if (!dongle->ready_coder[0] && !dongle->ready_coder[1])
    {
        dongle->ready_coder[0] = coder;
    }
    else if (dongle->ready_coder[0] && !dongle->ready_coder[1])
    {
        if (coder->start < dongle->ready_coder[0]->start ||
            (coder->start == dongle->ready_coder[0]->start &&
             coder->id < dongle->ready_coder[0]->id))
        {
            tmp = dongle->ready_coder[0];
            dongle->ready_coder[0] = coder;
            dongle->ready_coder[1] = tmp;
        }
        else
        {
            dongle->ready_coder[1] = coder;
        }
    }
}