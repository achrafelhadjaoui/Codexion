/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   activity_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 20:58:35 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 21:03:01 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

int	wait_for_activity(t_coder *coder, int duration)
{
	long	current_time;
	long	end_time;

	current_time = convert_to_milisecond();
	end_time = current_time + duration;
	if (checking_burnout(coder))
	{
		mention_to_stop_threads(coder->monitor);
		return (1);
	}
	while (current_time < end_time)
	{
		if (simulation_stopped(coder))
			return (1);
		usleep(1000);
		current_time = convert_to_milisecond();
	}
	return (0);
}

int	wait_remaining(long remaining, t_coder *coder)
{
	long	current_time;
	long	end_time;

	current_time = convert_to_milisecond();
	end_time = current_time + remaining;
	while (current_time < end_time)
	{
		if (simulation_stopped(coder))
			return (0);
		usleep(1000);
		current_time = convert_to_milisecond();
	}
	return (1);
}

void	handle_burnout(long remaining, t_coder *coder)
{
	if (remaining > 0)
	{
		if (!wait_remaining(remaining, coder))
			return ;
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
