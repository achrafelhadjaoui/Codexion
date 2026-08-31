#include "../head.h"

static void release_one_dongle(DONGLE *dongle)
{
    struct timespec now;

    pthread_mutex_lock(&dongle->dongle_mutex);

    /*
     * Dongle enters cooldown.
     */
    dongle->is_used = 2;

    /*
     * Calculate when the cooldown ends.
     */
    clock_gettime(CLOCK_MONOTONIC, &now);

    dongle->cooldown_until = now;
    dongle->cooldown_until.tv_sec += dongle->cool_down / 1000;

    /*
     * Remove the current coder from the ready queue.
     *
     * The coder that was [1] becomes [0].
     */
    dongle->ready_coder[0] = dongle->ready_coder[1];
    dongle->ready_coder[1] = NULL;

    /*
     * Wake the next coder.
     *
     * It will notice that the dongle is in cooldown
     * and use pthread_cond_timedwait().
     */
    if (dongle->ready_coder[0])
    {
        pthread_cond_signal(
            &dongle->ready_coder[0]->coder_cond
        );
    }

    pthread_mutex_unlock(&dongle->dongle_mutex);
}

void release_dongles(CODER *coder)
{
    release_one_dongle(coder->left);
    release_one_dongle(coder->right);

    /*
     * Nothing else here.
     *
     * The coder can now immediately enter
     * debugging/refactoring.
     */
}