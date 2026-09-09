/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 18:21:49 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 20:22:56 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "head.h"

void	stop_threads(t_monitor *monitor)
{
	pthread_mutex_lock(&monitor->monitor_mutex);
	monitor->burnout_detected = 1;
	pthread_cond_signal(&monitor->monitor_cond);
	pthread_mutex_unlock(&monitor->monitor_mutex);
	wake_all_coders(monitor->coders);
}

void	mention_to_stop_threads(t_monitor *monitor)
{
	pthread_mutex_lock(&monitor->monitor_mutex);
	monitor->burnout_detected = 1;
	pthread_cond_signal(&monitor->monitor_cond);
	pthread_mutex_unlock(&monitor->monitor_mutex);
}

int	simulation_stopped(t_coder *coder)
{
	int	stopped;

	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	stopped = coder->monitor->burnout_detected;
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
	return (stopped);
}

/*
	* Coder i owns dongle i as its right dongle, so walking the coders
	* visits every dongle exactly once.
	*
	* Broadcasting while holding the dongle mutex is what makes the
	* wake-up impossible to miss: a waiter can only be inside
	* pthread_cond_timedwait() or holding that same mutex.
	*/
void	wake_all_coders(t_coder *coder)
{
	int			i;
	int			size;
	t_dongle	*dongle;

	size = coder->nub_of_coders;
	i = 0;
	while (i < size)
	{
		dongle = coder[i].right;
		pthread_mutex_lock(&dongle->dongle_mutex);
		pthread_cond_broadcast(&dongle->dongle_cond);
		pthread_mutex_unlock(&dongle->dongle_mutex);
		i++;
	}
}
