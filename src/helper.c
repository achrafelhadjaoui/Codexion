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
	t_simulation	*sim;

	sim = monitor->coders[0].simulation;
	pthread_mutex_lock(&sim->logging_mutex);
	sim->stop_logging = 1;
	pthread_mutex_unlock(&sim->logging_mutex);
	pthread_mutex_lock(&monitor->monitor_mutex);
	monitor->burnout_detected = 1;
	pthread_mutex_unlock(&monitor->monitor_mutex);
	wake_all_coders(monitor->coders);
}

int	simulation_stopped(t_coder *coder)
{
	int	stopped;

	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	stopped = coder->monitor->burnout_detected;
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
	return (stopped);
}

void	notify_sibling(t_dongle *dongle)
{
	if (dongle->sibling == dongle)
		return ;
	pthread_mutex_lock(&dongle->sibling->dongle_mutex);
	pthread_cond_broadcast(&dongle->sibling->dongle_cond);
	pthread_mutex_unlock(&dongle->sibling->dongle_mutex);
}

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
