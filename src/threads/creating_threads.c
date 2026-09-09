/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creating_threads.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:46:18 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 22:05:17 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static void	*routine(void *args)
{
	t_coder	*coder;
	int		i;

	coder = (t_coder *)args;
	i = 0;
	while (i < coder->nub_of_compiles)
	{
		if (request_dongles(coder))
			return (NULL);
		if (compiling(coder))
		{
			release_dongles(coder);
			return (NULL);
		}
		release_dongles(coder);
		if (debuging(coder))
			return (NULL);
		if (refactoring(coder))
			return (NULL);
		i++;
	}
	coder_finished(coder);
	return (NULL);
}

static void	*monitor_routine(void *arg)
{
	t_monitor	*monitor;

	monitor = (t_monitor *)arg;
	pthread_mutex_lock(&monitor->monitor_mutex);
	while (monitor->burnout_detected != 1 && monitor->finish_running != 0)
		pthread_cond_wait(&monitor->monitor_cond, &monitor->monitor_mutex);
	pthread_mutex_unlock(&monitor->monitor_mutex);
	wake_all_coders(monitor->coders);
	return (NULL);
}

static int	create_threads(t_coder *coder, t_monitor *monitor, int size)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < size)
	{
		if (pthread_create(&coder[i].thread, NULL, routine, &coder[i]) != 0)
			break ;
		i++;
	}
	if (i != size)
	{
		stop_threads(monitor);
		while (j < i)
		{
			if (pthread_join(coder[j].thread, NULL) != 0)
				return (1);
			j++;
		}
		return (1);
	}
	return (0);
}

static int	create_monitor(t_monitor *monitor, int size)
{
	int	i;

	i = 0;
	if (pthread_create(&monitor->monitor_thread, NULL, monitor_routine,
			monitor) != 0)
	{
		stop_threads(monitor);
		while (i < size)
		{
			if (pthread_join(monitor->coders[i].thread, NULL) != 0)
				return (1);
			i++;
		}
		return (1);
	}
	return (0);
}

void	coder_and_monitor_thread_creation(t_coder *coder, int size,
		t_monitor *monitor)
{
	int	i;

	if (create_threads(coder, monitor, size))
		return ;
	if (create_monitor(monitor, size))
		return ;
	i = 0;
	while (i < size)
	{
		if (pthread_join(coder[i].thread, NULL) != 0)
		{
			stop_threads(monitor);
			return ;
		}
		i++;
	}
	if (pthread_join(monitor->monitor_thread, NULL) != 0)
	{
		stop_threads(monitor);
		return ;
	}
}
