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

/*
	* Only the logging mutex is taken here, and stop_logging lives under
	* it. Reading the burnout flag under the monitor mutex instead meant
	* every state change grabbed two locks, one of them the single mutex
	* the whole simulation shares, while still holding both dongles. The
	* coders woken together by a released pair all piled onto it at the
	* one moment they are trying to start compiling.
	*
	* One lock guards the decision and the line it prints together, which
	* is all the ordering the output ever needed.
	*/
void	log_state(t_coder *coder, char *msg)
{
	t_simulation	*sim;

	sim = coder->simulation;
	pthread_mutex_lock(&sim->logging_mutex);
	if (!sim->stop_logging)
		printf("%ld %d %s\n", convert_to_milisecond() - sim->start_time,
			coder->id, msg);
	pthread_mutex_unlock(&sim->logging_mutex);
}

/*
	* Setting stop_logging and printing the line under the one lock is
	* what makes "burned out" the last line: a coder already inside the
	* lock got there first, and every coder after it finds the flag set
	* and prints nothing.
	*/
void	log_burnout(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->simulation;
	pthread_mutex_lock(&sim->logging_mutex);
	if (!sim->stop_logging)
	{
		sim->stop_logging = 1;
		printf("%ld %d burned out\n",
			convert_to_milisecond() - sim->start_time, coder->id);
	}
	pthread_mutex_unlock(&sim->logging_mutex);
	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	coder->monitor->burnout_detected = 1;
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
	long			timestamp;
	t_simulation	*sim;

	sim = coder->simulation;
	pthread_mutex_lock(&sim->logging_mutex);
	if (!sim->stop_logging)
	{
		timestamp = convert_to_milisecond() - sim->start_time;
		printf("%ld %d has taken a dongle\n", timestamp, coder->id);
		printf("%ld %d has taken a dongle\n", timestamp, coder->id);
		printf("%ld %d is compiling\n", timestamp, coder->id);
	}
	pthread_mutex_unlock(&sim->logging_mutex);
}
