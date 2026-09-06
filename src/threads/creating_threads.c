/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creating_threads.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:46:18 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:46:21 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"



static void *routine(void *args)
{
    CODER *coder;
    int i;

    coder = (CODER *)args;
    i = 0;
    while (i < coder->nub_of_compiles)
    {
        coder->start = convert_to_milisecond();

        if (request_dongles(coder))
            return NULL;

        
        coder->last_compile = convert_to_milisecond();

        if (compiling(coder))
        {
            release_dongles(coder);
            return (NULL);
        }

        // coder->last_compile = convert_to_milisecond();

        release_dongles(coder);

        if (debuging(coder))
            return (NULL);

        if (refactoring(coder))
            return (NULL);

        i++;
    }

    pthread_mutex_lock(&coder->monitor->monitor_mutex);
    coder->monitor->finish_running -= 1;
    pthread_cond_signal(&coder->monitor->monitor_cond);
    pthread_mutex_unlock(&coder->monitor->monitor_mutex);

    return (NULL);
}


static void *monitor_routine(void *arg)
{
    MONITOR *monitor = (MONITOR *) arg;

    pthread_mutex_lock(&monitor->monitor_mutex);
    while ( monitor-> burnout_detected != 1 || monitor->finish_running != 0)
    {
        
        pthread_cond_wait(&monitor->monitor_cond, &monitor->monitor_mutex);

        if(monitor->burnout_detected == 1)
            break;
        if (monitor->finish_running == 0)
        {
            pthread_mutex_unlock(&monitor->monitor_mutex);
            return NULL;
        }
       
    }
    // if (monitor->finish_running == 0)
    // {
    //     pthread_mutex_unlock(&monitor->monitor_mutex);
    //      return NULL;
    // }

    pthread_cond_broadcast(&monitor->activity_cond);
    pthread_mutex_unlock(&monitor->monitor_mutex);

    // also signal all the coders being waiting for dongle cooldown
    wake_all_coders(monitor->coders);
    
    return NULL;
}

void coder_and_monitor_thread_creation(CODER *coder, int size, MONITOR *monitor, DONGLE *dongle)
{
    int i;

    i = 0;
    while (i < size)
    {
        if (pthread_create(&coder[i].thread, NULL, routine, &coder[i]) != 0)
        {
            exit(3);
        }
        i++;
    }

    if (pthread_create(&monitor->monitor_thread, NULL, monitor_routine, monitor) != 0)
        exit(3);


    i = 0;
    while (i < size)
    {
        if (pthread_join(coder[i].thread, NULL) != 0)
            exit(3);

        i++;
    }

    if (pthread_join(monitor->monitor_thread, NULL) != 0)
        exit(3);

    i = 0;
    while (i < size)
    {
        pthread_cond_destroy(&coder[i].coder_cond);
        pthread_mutex_destroy(&dongle[i].dongle_mutex);
        i++;
    }

    pthread_mutex_destroy(&monitor->monitor_mutex);
    pthread_cond_destroy(&monitor->monitor_cond);
}
