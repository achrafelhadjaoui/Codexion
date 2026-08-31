// #include "../head.h"

// void edf_implementation(DONGLE *dongle, CODER *coder)
// {
//     CODER *tmp;
//     int time_now, res;

//     time_now = convert_to_milisecond();
//     coder->time_until_burnout = (time_now + coder->last_compile) - coder->time_to_burnout;

//     if (!dongle->ready_coder[0] && !dongle->ready_coder[1])
//     {
//         dongle->ready_coder[0] = coder;
//     }
//     else if (dongle->ready_coder[0] && !dongle->ready_coder[1])
//     {
//         if (coder->time_until_burnout < dongle->ready_coder[0]->time_until_burnout ||
//             (coder->time_until_burnout == dongle->ready_coder[0]->time_until_burnout &&
//              coder->id < dongle->ready_coder[0]->id))
//         {
//             tmp = dongle->ready_coder[0];
//             dongle->ready_coder[0] = coder;
//             dongle->ready_coder[1] = tmp;
//         }
//         else
//         {
//             dongle->ready_coder[1] = coder;
//         }
//     }
// }




#include "../head.h"

void edf_implementation(DONGLE *dongle, CODER *coder)
{
    CODER *tmp;

    /*
     * EDF deadline:
     *
     * last compilation finish time
     * + allowed burnout time.
     */
    coder->time_until_burnout =
        coder->last_compile + coder->time_to_burnout;

    if (dongle->ready_coder[0] == NULL)
    {
        dongle->ready_coder[0] = coder;
    }
    else if (dongle->ready_coder[1] == NULL)
    {
        if (coder->time_until_burnout <
                dongle->ready_coder[0]->time_until_burnout ||
            (coder->time_until_burnout ==
                dongle->ready_coder[0]->time_until_burnout &&
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
