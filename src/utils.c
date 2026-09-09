/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:42:21 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 04:23:49 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "head.h"

long	convert_to_microsecond(int nb)
{
	return (nb * 1000);
}

long	convert_to_milisecond(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((long)tv.tv_sec * 1000L) + ((long)tv.tv_usec / 1000L));
}

void	get_abstime(struct timespec *abstime, long milliseconds)
{
	clock_gettime(CLOCK_REALTIME, abstime);
	abstime->tv_sec += milliseconds / 1000;
	abstime->tv_nsec += (milliseconds % 1000) * 1000000L;
	if (abstime->tv_nsec >= 1000000000L)
	{
		abstime->tv_sec++;
		abstime->tv_nsec -= 1000000000L;
	}
}

/*
	* The stop flag and the log line are written inside the SAME
	* monitor_mutex critical section.
	*
	* Every other log goes through log_state(), which also takes
	* monitor_mutex, so nothing can be printed after "burned out".
	*/
int	checking_burnout(t_coder *coder)
{
	long	current_time;
	long	time_since_compile;

	current_time = convert_to_milisecond();
	time_since_compile = current_time - coder->last_compile;
	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	if ((time_since_compile >= coder->time_to_burnout)
		&& coder->monitor->burnout_detected != 1)
	{
		coder->monitor->burnout_detected = 1;
		pthread_mutex_lock(&coder->simulation->logging_mutex);
		printf("%ld %d burned out\n", convert_to_milisecond()
			- coder->simulation->start_time, coder->id);
		pthread_mutex_unlock(&coder->simulation->logging_mutex);
		pthread_cond_signal(&coder->monitor->monitor_cond);
		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
		return (1);
	}
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
	return (0);
}

void	log_state(t_coder *coder, char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	if (coder->monitor->burnout_detected != 1)
	{
		timestamp = convert_to_milisecond() - coder->simulation->start_time;
		pthread_mutex_lock(&coder->simulation->logging_mutex);
		printf("%ld %d %s\n", timestamp, coder->id, msg);
		pthread_mutex_unlock(&coder->simulation->logging_mutex);
	}
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
}
