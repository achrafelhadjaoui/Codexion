/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compile_debug_refactor.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:43:07 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:44:18 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static int wait_for_activity(CODER *coder, int duration)
{
    struct timespec abstime;

 
    get_abstime(&abstime, duration);

    // cheking if burnout is happen 
    if (checking_burnout(coder))
    {
        pthread_mutex_lock(&coder->monitor->monitor_mutex);
        coder->monitor->burnout_detected = 1;
        pthread_cond_signal(&coder->monitor->monitor_cond);
        pthread_mutex_unlock(&coder->monitor->monitor_mutex);
        return 1;
    }

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
    long current_time;
    long debug_end;
    long burnout_deadline;
    long remaining;
    struct timespec abstime;

    current_time = convert_to_milisecond();

    /*
     * Debug starts now, so its end time is:
     *
     * current time + debug duration
     */
    debug_end = current_time + coder->time_to_debug;

    /*
     * Burnout deadline is based on the last compilation.
     *
     * last_compile is a timestamp.
     * time_to_burnout is a duration.
     */
    burnout_deadline =
        coder->last_compile + coder->time_to_burnout;

    pthread_mutex_lock(&coder->simulation->logging_mutex);

    printf("%ld %d is debugging\n",
        convert_to_milisecond() - coder->simulation->start_time,
        coder->id);

    pthread_mutex_unlock(&coder->simulation->logging_mutex);

    /*
     * Debug finishes before the burnout deadline.
     */
    if (debug_end < burnout_deadline)
    {
        return (wait_for_activity(coder, coder->time_to_debug));
    }

    /*
     * Debug would finish at or after the burnout deadline.
     *
     * Therefore we only wait until the burnout deadline.
     */
    remaining = burnout_deadline - current_time;

    if (remaining > 0)
    {
        get_abstime(&abstime, remaining);

        pthread_mutex_lock(&coder->monitor->monitor_mutex);

        pthread_cond_timedwait(
            &coder->monitor->activity_cond,
            &coder->monitor->monitor_mutex,
            &abstime
        );

        pthread_mutex_unlock(&coder->monitor->monitor_mutex);
    }

    /*
     * We reached the burnout deadline.
     */
    pthread_mutex_lock(&coder->monitor->monitor_mutex);

    if (coder->monitor->burnout_detected != 1)
    {
        coder->monitor->burnout_detected = 1;

        pthread_cond_signal(
            &coder->monitor->monitor_cond
        );
        pthread_mutex_lock(&coder->simulation->logging_mutex);
        printf("%ld %d burned out\n", convert_to_milisecond() - coder->simulation->start_time, coder->id);
        pthread_mutex_unlock(&coder->simulation->logging_mutex);
        
    }

    pthread_mutex_unlock(&coder->monitor->monitor_mutex);

    return (1);
}




int refactoring(CODER *coder)
{
    long current_time;
    long refac_end;
    long burnout_deadline;
    long remaining;
    struct timespec abstime;

    current_time = convert_to_milisecond();

    /*
     * Refactoring starts now.
     */
    refac_end = current_time + coder->time_to_refac;

    /*
     * Burnout deadline is:
     *
     * last compilation timestamp
     * +
     * allowed time before burnout
     */
    burnout_deadline =
        coder->last_compile + coder->time_to_burnout;

    pthread_mutex_lock(&coder->simulation->logging_mutex);

    printf("%ld %d is refactoring\n",
        convert_to_milisecond() - coder->simulation->start_time,
        coder->id);

    pthread_mutex_unlock(&coder->simulation->logging_mutex);

    /*
     * Refactoring finishes before burnout.
     */
    if (refac_end < burnout_deadline)
    {
        return (wait_for_activity(coder, coder->time_to_refac));
    }

    /*
     * Refactoring would finish at or after burnout.
     *
     * Wait only until the burnout deadline.
     */
    remaining = burnout_deadline - current_time;

    if (remaining > 0)
    {
        get_abstime(&abstime, remaining);

        pthread_mutex_lock(&coder->monitor->monitor_mutex);

        pthread_cond_timedwait(
            &coder->monitor->activity_cond,
            &coder->monitor->monitor_mutex,
            &abstime
        );

        pthread_mutex_unlock(&coder->monitor->monitor_mutex);
    }

    /*
     * Burnout deadline has been reached.
     */
    pthread_mutex_lock(&coder->monitor->monitor_mutex);

    if (coder->monitor->burnout_detected != 1)
    {
        coder->monitor->burnout_detected = 1;

        pthread_cond_signal(
            &coder->monitor->monitor_cond
        );
        pthread_mutex_lock(&coder->simulation->logging_mutex);
        printf("%ld %d burned out\n", convert_to_milisecond() - coder->simulation->start_time, coder->id);
        pthread_mutex_unlock(&coder->simulation->logging_mutex);
        
    }

    pthread_mutex_unlock(&coder->monitor->monitor_mutex);

    return (1);
}

int compiling(CODER *coder)
{
    long current_time;
    long compile_end;
    long burnout_deadline;
    long remaining;
    struct timespec abstime;

    current_time = convert_to_milisecond();

    /*
     * Compilation starts now.
     *
     * compile_end is an absolute timestamp.
     */
    compile_end = current_time + coder->time_to_compile;

    /*
     * Burnout deadline is based on the last compilation start.
     */
    burnout_deadline =
        coder->last_compile + coder->time_to_burnout;

    pthread_mutex_lock(&coder->simulation->logging_mutex);

    printf("%ld %d is compiling\n",
        convert_to_milisecond() - coder->simulation->start_time,
        coder->id);

    pthread_mutex_unlock(&coder->simulation->logging_mutex);

    /*
     * Compilation finishes before the burnout deadline.
     */
    if (compile_end < burnout_deadline)
    {
        return (wait_for_activity(coder, coder->time_to_compile));
    }

    /*
     * Compilation would finish at or after the burnout
     * deadline.
     *
     * Wait only until burnout happens.
     */
    remaining = burnout_deadline - current_time;

    if (remaining > 0)
    {
        get_abstime(&abstime, remaining);

        pthread_mutex_lock(&coder->monitor->monitor_mutex);

        pthread_cond_timedwait(
            &coder->monitor->activity_cond,
            &coder->monitor->monitor_mutex,
            &abstime
        );

        pthread_mutex_unlock(&coder->monitor->monitor_mutex);
    }

    /*
     * Burnout deadline has been reached.
     */
    pthread_mutex_lock(&coder->monitor->monitor_mutex);

    if (coder->monitor->burnout_detected != 1)
    {
        coder->monitor->burnout_detected = 1;

        pthread_cond_signal(
            &coder->monitor->monitor_cond
        );
        pthread_mutex_lock(&coder->simulation->logging_mutex);
        printf("%ld %d burned out\n", convert_to_milisecond() - coder->simulation->start_time, coder->id);
        pthread_mutex_unlock(&coder->simulation->logging_mutex);
        
    }

    pthread_mutex_unlock(&coder->monitor->monitor_mutex);

    return (1);
}
