/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 18:21:49 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 18:21:53 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "head.h"

void	stop_threads(t_monitor *monitor)
{
	pthread_mutex_lock(&monitor->monitor_mutex);
	monitor->burnout_detected = 1;
	pthread_cond_broadcast(&monitor->monitor_cond);
	pthread_cond_broadcast(&monitor->activity_cond);
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
