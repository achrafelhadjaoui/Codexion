/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 09:12:04 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/10 09:12:07 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static void	refresh_one_dongle(t_dongle *dongle)
{
	int	woke;

	woke = 0;
	pthread_mutex_lock(&dongle->dongle_mutex);
	if (dongle->is_used == 2
		&& convert_to_milisecond() >= dongle->cooldown_until)
	{
		dongle->is_used = 0;
		pthread_cond_broadcast(&dongle->dongle_cond);
		woke = 1;
	}
	pthread_mutex_unlock(&dongle->dongle_mutex);
	if (woke)
		notify_sibling(dongle);
}

static int	check_one_coder(t_coder *coder)
{
	int	burned;

	burned = 0;
	pthread_mutex_lock(&coder->right->dongle_mutex);
	if (!coder->finished && convert_to_milisecond() - coder->last_compile
		>= coder->time_to_burnout)
	{
		log_burnout(coder);
		burned = 1;
	}
	pthread_mutex_unlock(&coder->right->dongle_mutex);
	return (burned);
}

static int	sweep_coders(t_coder *coders, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		refresh_one_dongle(coders[i].right);
		i++;
	}
	i = 0;
	while (i < size)
	{
		if (check_one_coder(&coders[i]))
			return (1);
		i++;
	}
	return (0);
}

static int	everyone_is_done(t_monitor *monitor)
{
	int	done;

	pthread_mutex_lock(&monitor->monitor_mutex);
	done = (monitor->burnout_detected == 1 || monitor->finish_running == 0);
	pthread_mutex_unlock(&monitor->monitor_mutex);
	return (done);
}

/*
	* The monitor is the only thread that watches the clock. Everybody
	* else waits for an event, and this loop is what produces them.
	*/
void	*monitor_routine(void *arg)
{
	t_monitor	*monitor;
	int			size;

	monitor = (t_monitor *)arg;
	size = monitor->coders[0].nub_of_coders;
	while (1)
	{
		if (sweep_coders(monitor->coders, size))
			break ;
		if (everyone_is_done(monitor))
			break ;
		usleep(200);
	}
	stop_threads(monitor);
	return (NULL);
}
