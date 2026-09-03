#include "../head.h"

static int request_one_dongle(CODER *coder, DONGLE *dongle)
{

    pthread_mutex_lock(&dongle->dongle_mutex);

    /*
     * Add coder to the waiting queue.
     */
    if (strcmp(dongle->policy, "fifo") == 0)
    {

        fifo_implementation(dongle, coder);
    }
    else
    {

        edf_implementation(dongle, coder);
    }

    dongle->used_by = dongle->ready_coder[0];

    while (dongle->used_by != coder ||
           dongle->is_used != 0)
    {
        if (dongle->is_used == 2)
        {
            /*
             * Dongle is in cooldown.
             */
            pthread_cond_timedwait(
                &coder->coder_cond,
                &dongle->dongle_mutex,
                &dongle->cooldown_until);

            

            /*
             * Do not use ETIMEDOUT.
             *
             * Check the current time against cooldown_until.
             */
            struct timespec now;

            clock_gettime(CLOCK_MONOTONIC, &now);

            if (now.tv_sec > dongle->cooldown_until.tv_sec ||
            (now.tv_sec == dongle->cooldown_until.tv_sec &&
            now.tv_nsec >= dongle->cooldown_until.tv_nsec))
            {
                dongle->is_used = 0;
            }

        }
        else
        {
            /*
             * Dongle is currently being used.
             */
            pthread_cond_wait(
                &coder->coder_cond,
                &dongle->dongle_mutex
            );
      
        }
    }

    /*
     * We are first in the queue and the dongle
     * is available.
     */
    dongle->is_used = 1;
    pthread_mutex_lock(&coder->simulation->logging_mutex);
    printf("%ld %d has taken a dongle\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);
    pthread_mutex_unlock(&coder->simulation->logging_mutex);    


    dongle->ready_coder[0] = dongle->ready_coder[1];
    dongle->ready_coder[1] = NULL;

    pthread_mutex_unlock(&dongle->dongle_mutex);

    if (checking_burnout(coder))
    {
        pthread_mutex_lock(&coder->monitor->monitor_mutex);
        coder->monitor->burnout_detected = 1;
        pthread_cond_signal(&coder->monitor->monitor_cond);
        pthread_mutex_unlock(&coder->monitor->monitor_mutex);
        return 1;
    }
    return 0;
}

int request_dongles(CODER *coder)
{
    if (coder->id % 2 == 0)
    {
        if (request_one_dongle(coder, coder->left))
            return (1);

        if (request_one_dongle(coder, coder->right))
            return (1);
    }
    else
    {
        if (request_one_dongle(coder, coder->right))
            return (1);

        if (request_one_dongle(coder, coder->left))
            return (1);
    }

    return (0);
}






