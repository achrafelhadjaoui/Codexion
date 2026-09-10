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

/*
	* A cooldown running out is the one state change no thread can
	* announce: nobody causes it, it just happens when time passes.
	*
	* The monitor turns it into a real event here. That is what lets every
	* coder block on a plain pthread_cond_wait() instead of polling, and
	* what gives dongle_cond an actual waiter to wake up.
	*/
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

/*
	* Only the monitor decides that a coder burned out.
	*
	* last_compile is read under that coder's right dongle mutex: a coder
	* only ever writes it while holding BOTH of its dongle mutexes, so
	* holding one of the two already excludes the writer.
	*
	* The log line is printed inside the same critical section, so the
	* coder cannot start a compile between the check and the message.
	*/
static int	check_one_coder(t_coder *coder)
{
	t_monitor	*monitor;
	int			burned;

	monitor = coder->monitor;
	burned = 0;
	pthread_mutex_lock(&coder->right->dongle_mutex);
	if (convert_to_milisecond() - coder->last_compile
		>= coder->time_to_burnout)
	{
		pthread_mutex_lock(&monitor->monitor_mutex);
		if (monitor->burnout_detected != 1)
		{
			monitor->burnout_detected = 1;
			pthread_mutex_lock(&coder->simulation->logging_mutex);
			printf("%ld %d burned out\n", convert_to_milisecond()
				- coder->simulation->start_time, coder->id);
			pthread_mutex_unlock(&coder->simulation->logging_mutex);
		}
		pthread_mutex_unlock(&monitor->monitor_mutex);
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
