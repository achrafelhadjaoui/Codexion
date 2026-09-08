/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compile_debug_refactor.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:43:07 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 10:45:48 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static int	wait_for_activity(t_coder *coder, int duration)
{
	struct timespec	abstime;

	get_abstime(&abstime, duration);
	if (checking_burnout(coder))
	{
		pthread_mutex_lock(&coder->monitor->monitor_mutex);
		coder->monitor->burnout_detected = 1;
		pthread_cond_signal(&coder->monitor->monitor_cond);
		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
		return (1);
	}
	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	pthread_cond_timedwait(&coder->monitor->activity_cond,
		&coder->monitor->monitor_mutex, &abstime);
	if (coder->monitor->burnout_detected == 1)
	{
		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
		return (1);
	}
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
	return (0);
}

static void	handle_burnout(long remaining, t_coder *coder)
{
	struct timespec	abstime;

	if (remaining > 0)
	{
		get_abstime(&abstime, remaining);
		pthread_mutex_lock(&coder->monitor->monitor_mutex);
		pthread_cond_timedwait(&coder->monitor->activity_cond,
			&coder->monitor->monitor_mutex, &abstime);
		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
	}
	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	if (coder->monitor->burnout_detected != 1)
	{
		coder->monitor->burnout_detected = 1;
		pthread_cond_signal(&coder->monitor->monitor_cond);
		pthread_mutex_lock(&coder->simulation->logging_mutex);
		printf("%ld %d burned out\n", convert_to_milisecond()
			- coder->simulation->start_time, coder->id);
		pthread_mutex_unlock(&coder->simulation->logging_mutex);
	}
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
}

int	debuging(t_coder *coder)
{
	long	current_time;
	long	debug_end;
	long	burnout_deadline;
	long	remaining;

	current_time = convert_to_milisecond();
	debug_end = current_time + coder->time_to_debug;
	burnout_deadline = coder->last_compile + coder->time_to_burnout;
	pthread_mutex_lock(&coder->simulation->logging_mutex);
	printf("%ld %d is debugging\n", convert_to_milisecond()
		- coder->simulation->start_time, coder->id);
	pthread_mutex_unlock(&coder->simulation->logging_mutex);
	if (debug_end < burnout_deadline)
		return (wait_for_activity(coder, coder->time_to_debug));
	remaining = burnout_deadline - current_time;
	handle_burnout(remaining, coder);
	return (1);
}

int	refactoring(t_coder *coder)
{
	long	current_time;
	long	refac_end;
	long	burnout_deadline;
	long	remaining;

	current_time = convert_to_milisecond();
	refac_end = current_time + coder->time_to_refac;
	burnout_deadline = coder->last_compile + coder->time_to_burnout;
	pthread_mutex_lock(&coder->simulation->logging_mutex);
	printf("%ld %d is refactoring\n", convert_to_milisecond()
		- coder->simulation->start_time, coder->id);
	pthread_mutex_unlock(&coder->simulation->logging_mutex);
	if (refac_end < burnout_deadline)
		return (wait_for_activity(coder, coder->time_to_refac));
	remaining = burnout_deadline - current_time;
	handle_burnout(remaining, coder);
	return (1);
}

int	compiling(t_coder *coder)
{
	long	current_time;
	long	compile_end;
	long	burnout_deadline;
	long	remaining;

	current_time = convert_to_milisecond();
	compile_end = current_time + coder->time_to_compile;
	burnout_deadline = coder->last_compile + coder->time_to_burnout;
	pthread_mutex_lock(&coder->simulation->logging_mutex);
	printf("%ld %d is compiling\n", convert_to_milisecond()
		- coder->simulation->start_time, coder->id);
	pthread_mutex_unlock(&coder->simulation->logging_mutex);
	if (compile_end < burnout_deadline)
		return (wait_for_activity(coder, coder->time_to_compile));
	remaining = burnout_deadline - current_time;
	handle_burnout(remaining, coder);
	return (1);
}




// #include "../head.h"

// static int	wait_for_activity(t_coder *coder, long duration)
// {
// 	long	start_time;
// 	long	end_time;
// 	long	burnout_deadline;
// 	long	current_time;
// 	long	remaining;

// 	start_time = convert_to_milisecond();
// 	end_time = start_time + duration;
// 	burnout_deadline = coder->last_compile + coder->time_to_burnout;
// 	while (1)
// 	{
// 		current_time = convert_to_milisecond();
// 		if (checking_burnout(coder))
// 		{
// 			pthread_mutex_lock(&coder->monitor->monitor_mutex);
// 			coder->monitor->burnout_detected = 1;
// 			if (pthread_cond_signal(&coder->monitor->monitor_cond) != 0)
// 			{
// 				pthread_mutex_unlock(
// 					&coder->monitor->monitor_mutex);
// 				return (1);
// 			}
// 			if (pthread_mutex_unlock(
// 					&coder->monitor->monitor_mutex) != 0)
// 				return (1);
// 			return (1);
// 		}
// 		if (current_time >= end_time)
// 			break ;
// 		if (current_time >= burnout_deadline)
// 		{
// 			pthread_mutex_lock(&coder->monitor->monitor_mutex);
// 			coder->monitor->burnout_detected = 1;
// 			if (pthread_cond_signal(&coder->monitor->monitor_cond) != 0)
// 			{
// 				pthread_mutex_unlock(
// 					&coder->monitor->monitor_mutex);
// 				return (1);
// 			}
// 			if (pthread_mutex_unlock(
// 					&coder->monitor->monitor_mutex) != 0)
// 				return (1);
// 			return (1);
// 		}
// 		remaining = end_time - current_time;
// 		if (burnout_deadline - current_time < remaining)
// 			remaining = burnout_deadline - current_time;
// 		if (remaining > 1)
// 			remaining = 1;
// 		if (usleep(remaining * 1000) != 0)
// 			return (1);
// 	}
// 	return (checking_burnout(coder));
// }

// static int	handle_burnout(long remaining, t_coder *coder)
// {
// 	if (remaining > 0)
// 	{
// 		while (remaining > 0)
// 		{
// 			if (checking_burnout(coder))
// 				break ;
// 			if (remaining > 1)
// 			{
// 				if (usleep(1000) != 0)
// 					return (1);
// 			}
// 			else
// 			{
// 				if (usleep(1000) != 0)
// 					return (1);
// 			}
// 			remaining = coder->last_compile
// 				+ coder->time_to_burnout
// 				- convert_to_milisecond();
// 		}
// 	}
// 	if (pthread_mutex_lock(&coder->monitor->monitor_mutex) != 0)
// 		return (1);
// 	if (coder->monitor->burnout_detected != 1)
// 	{
// 		coder->monitor->burnout_detected = 1;
// 		if (pthread_cond_signal(&coder->monitor->monitor_cond) != 0)
// 		{
// 			pthread_mutex_unlock(&coder->monitor->monitor_mutex);
// 			return (1);
// 		}
// 		if (pthread_mutex_lock(&coder->simulation->logging_mutex) != 0)
// 		{
// 			pthread_mutex_unlock(&coder->monitor->monitor_mutex);
// 			return (1);
// 		}
// 		printf("%ld %d burned out\n",
// 			convert_to_milisecond()
// 			- coder->simulation->start_time,
// 			coder->id);
// 		if (pthread_mutex_unlock(
// 				&coder->simulation->logging_mutex) != 0)
// 		{
// 			pthread_mutex_unlock(&coder->monitor->monitor_mutex);
// 			return (1);
// 		}
// 	}
// 	if (pthread_mutex_unlock(&coder->monitor->monitor_mutex) != 0)
// 		return (1);
// 	return (0);
// }

// int	debuging(t_coder *coder)
// {
// 	long	current_time;
// 	long	debug_end;
// 	long	burnout_deadline;
// 	long	remaining;

// 	current_time = convert_to_milisecond();
// 	debug_end = current_time + coder->time_to_debug;
// 	burnout_deadline = coder->last_compile + coder->time_to_burnout;
// 	if (pthread_mutex_lock(&coder->simulation->logging_mutex) != 0)
// 		return (1);
// 	printf("%ld %d is debugging\n",
// 		convert_to_milisecond()
// 		- coder->simulation->start_time,
// 		coder->id);
// 	if (pthread_mutex_unlock(&coder->simulation->logging_mutex) != 0)
// 		return (1);
// 	if (debug_end < burnout_deadline)
// 		return (wait_for_activity(coder, coder->time_to_debug));
// 	remaining = burnout_deadline - current_time;
// 	if (handle_burnout(remaining, coder))
// 		return (1);
// 	return (1);
// }

// int	refactoring(t_coder *coder)
// {
// 	long	current_time;
// 	long	refac_end;
// 	long	burnout_deadline;
// 	long	remaining;

// 	current_time = convert_to_milisecond();
// 	refac_end = current_time + coder->time_to_refac;
// 	burnout_deadline = coder->last_compile + coder->time_to_burnout;
// 	if (pthread_mutex_lock(&coder->simulation->logging_mutex) != 0)
// 		return (1);
// 	printf("%ld %d is refactoring\n",
// 		convert_to_milisecond()
// 		- coder->simulation->start_time,
// 		coder->id);
// 	if (pthread_mutex_unlock(&coder->simulation->logging_mutex) != 0)
// 		return (1);
// 	if (refac_end < burnout_deadline)
// 		return (wait_for_activity(coder, coder->time_to_refac));
// 	remaining = burnout_deadline - current_time;
// 	if (handle_burnout(remaining, coder))
// 		return (1);
// 	return (1);
// }

// int	compiling(t_coder *coder)
// {
// 	long	current_time;
// 	long	compile_end;
// 	long	burnout_deadline;
// 	long	remaining;

// 	current_time = convert_to_milisecond();
// 	compile_end = current_time + coder->time_to_compile;
// 	burnout_deadline = coder->last_compile + coder->time_to_burnout;
// 	if (pthread_mutex_lock(&coder->simulation->logging_mutex) != 0)
// 		return (1);
// 	printf("%ld %d is compiling\n",
// 		convert_to_milisecond()
// 		- coder->simulation->start_time,
// 		coder->id);
// 	if (pthread_mutex_unlock(&coder->simulation->logging_mutex) != 0)
// 		return (1);
// 	if (compile_end < burnout_deadline)
// 		return (wait_for_activity(coder, coder->time_to_compile));
// 	remaining = burnout_deadline - current_time;
// 	if (handle_burnout(remaining, coder))
// 		return (1);
// 	return (1);
// }