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

long	convert_to_milisecond(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((long)tv.tv_sec * 1000L) + ((long)tv.tv_usec / 1000L));
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

/*
	* The two dongles and the compile start are one single event, so they
	* are printed as one. Logging them separately let another coder slip a
	* line between a coder's two "has taken a dongle" lines, which breaks
	* the "two takes, then compiling" sequence the logs are read for.
	*/
void	log_compile_start(t_coder *coder)
{
	long	timestamp;

	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	if (coder->monitor->burnout_detected != 1)
	{
		timestamp = convert_to_milisecond() - coder->simulation->start_time;
		pthread_mutex_lock(&coder->simulation->logging_mutex);
		printf("%ld %d has taken a dongle\n", timestamp, coder->id);
		printf("%ld %d has taken a dongle\n", timestamp, coder->id);
		printf("%ld %d is compiling\n", timestamp, coder->id);
		pthread_mutex_unlock(&coder->simulation->logging_mutex);
	}
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
}
