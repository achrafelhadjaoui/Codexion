// #include "../head.h"

// static void request_one_dongle(CODER *coder, DONGLE *dongle)
// {
//     pthread_mutex_lock(&dongle->dongle_mutex);

//     /*
//      * Add the coder to this dongle's waiting queue.
//      * FIFO or EDF decides who should get the dongle.
//      */
//     if (strcmp(dongle->policy, "fifo") == 0)
//         fifo_implementation(dongle, coder);
//     else
//         edf_implementation(dongle, coder);

//     /*
//      * Wait until this coder actually owns the dongle.
//      *
//      * pthread_cond_wait() releases dongle_mutex while sleeping
//      * and locks it again before returning.
//      */
//     while (dongle->ready_coder[0] != coder || dongle->is_used != 0)
//     {
//         if (dongle->is_used == 2)
//         {

//             struct timespec tv;

//             pthread_cond_timedwait(&coder->coder_cond, &dongle->dongle_mutex, &dongle->cooldown_until);
//             /* Check if cooldown has finished */
//             clock_gettime(CLOCK_MONOTONIC, &tv);
//             // check also tthe tv.nsec
//             if (tv.tv_sec >= dongle->cooldown_until.tv_sec)
//                 dongle->is_used = 0;
//         }
//         else
//         {
//             pthread_cond_wait(&coder->coder_cond, &dongle->dongle_mutex);
//         }
//     }

//     /*
//      * We only protect the dongle state with this mutex.
//      * The coder keeps logical ownership of the dongle after
//      * releasing the mutex.
//      */
//     dongle->is_used = 1;
//     pthread_mutex_unlock(&dongle->dongle_mutex);
// }

// void request_dongles(CODER *coder)
// {
//     printf("routine %d: inside request dongle\n", coder->id);
//     if (coder->id % 2 == 0)
//     {
//         /*
//          * Even coder:
//          *
//          * left -> right
//          */
//         request_one_dongle(coder, coder->left);
//         request_one_dongle(coder, coder->right);
//     }
//     else
//     {
//         /*
//          * Odd coder:
//          *
//          * right -> left
//          */
//         request_one_dongle(coder, coder->right);
//         request_one_dongle(coder, coder->left);
//     }
// }


#include "../head.h"

static void request_one_dongle(CODER *coder, DONGLE *dongle)
{
    // int ret;

    pthread_mutex_lock(&dongle->dongle_mutex);

    /*
     * Add coder to the waiting queue.
     */
    //printf("CODER %i, REAUEST DONGLE\n", coder->id);
    if (strcmp(dongle->policy, "fifo") == 0)
    {

        fifo_implementation(dongle, coder);
    }
    else
    {

        edf_implementation(dongle, coder);
    }

    while (dongle->ready_coder[0] != coder ||
           dongle->is_used != 0)
    {
        if (dongle->is_used == 2)
        {
            /*
             * Dongle is in cooldown.
             */
            //printf("BEFORE TIMED WAITING CODER %d\n", coder->id);
            pthread_cond_timedwait(
                &coder->coder_cond,
                &dongle->dongle_mutex,
                &dongle->cooldown_until);

            //printf("AFTER TIMED WAITING CODER %d\n", coder->id);
            

            /*
             * Do not use ETIMEDOUT.
             *
             * Check the current time against cooldown_until.
             */
            struct timespec now;

            clock_gettime(CLOCK_MONOTONIC, &now);

            if (now.tv_sec >= dongle->cooldown_until.tv_sec)
            {
                dongle->is_used = 0;
            }
            // else if (ret != 0)
            // {
            //     /*
            //      * pthread_cond_timedwait() failed for
            //      * something other than the cooldown expiring.
            //      */


            //     pthread_mutex_unlock(&dongle->dongle_mutex);
            //     return;
            // }
        }
        else
        {
            /*
             * Dongle is currently being used.
             */
            printf("BEFORE WAITING CODER %d\n", coder->id);
            pthread_cond_wait(
                &coder->coder_cond,
                &dongle->dongle_mutex
            );
            printf("AFTER WAITING CODER %d\n", coder->id);
            // if (ret != 0)
            // {

            //     pthread_mutex_unlock(&dongle->dongle_mutex);
            //     return;
            // }
        }
    }

    /*
     * We are first in the queue and the dongle
     * is available.
     */
    dongle->is_used = 1;
    printf("%d %d has taken a dongle\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);

    pthread_mutex_unlock(&dongle->dongle_mutex);
}

void request_dongles(CODER *coder)
{
    if (coder->id % 2 == 0)
    {
        request_one_dongle(coder, coder->left);
        request_one_dongle(coder, coder->right);

    }
    else
    {
        request_one_dongle(coder, coder->right);
        request_one_dongle(coder, coder->left);
    }
}






