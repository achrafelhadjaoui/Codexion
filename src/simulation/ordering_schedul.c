/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ordering_schedul.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:45:05 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:45:10 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static void coder_to_wating_queue(t_coder *coder, t_dongle *dongle)
{
	if (strcmp(dongle->policy, "fifo") == 0)
	{
		fifo_implementation(dongle, coder);
	}
	else
	{
		edf_implementation(dongle, coder);
	}
}

static int wait_to_aquire_dongle(t_coder *coder, t_dongle *dongle, t_coder *used_by, struct timespec now)
{
	while (simulation_stopped(coder) != 1 && (dongle->used_by != coder
			|| dongle->is_used != 0))
	{
		if (simulation_stopped(coder))
		{
			dongle->ready_coder[0] = NULL;
			dongle->ready_coder[1] = NULL;
			pthread_mutex_unlock(&dongle->dongle_mutex);
			return (1);
		}
		if (dongle->is_used == 2)
		{
			/*
				* Dongle is in cooldown.
				*/
			pthread_cond_timedwait(&coder->coder_cond, &dongle->dongle_mutex,
				&dongle->cooldown_until);
			clock_gettime(CLOCK_MONOTONIC, &now);
			if (now.tv_sec > dongle->cooldown_until.tv_sec
				|| (now.tv_sec == dongle->cooldown_until.tv_sec
					&& now.tv_nsec >= dongle->cooldown_until.tv_nsec))
			{
				dongle->is_used = 0;
			}
		}
		else
		{
			pthread_cond_wait(&coder->coder_cond, &dongle->dongle_mutex);
		}
	}
}

static int	request_one_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	now;

	pthread_mutex_lock(&dongle->dongle_mutex);
	/*
		* Add coder to the waiting queue.
		*/

	coder_to_wating_queue(coder, dongle);

	// if (strcmp(dongle->policy, "fifo") == 0)
	// {
	// 	fifo_implementation(dongle, coder);
	// }
	// else
	// {
	// 	edf_implementation(dongle, coder);
	// }
	dongle->used_by = dongle->ready_coder[0];
	while (simulation_stopped(coder) != 1 && (dongle->used_by != coder
			|| dongle->is_used != 0))
	{
		if (simulation_stopped(coder))
		{
			dongle->ready_coder[0] = NULL;
			dongle->ready_coder[1] = NULL;
			pthread_mutex_unlock(&dongle->dongle_mutex);
			return (1);
		}
		if (dongle->is_used == 2)
		{
			/*
				* Dongle is in cooldown.
				*/

			while (simulation_stopped(coder) != 1 && dongle->cool_down-- > 0)
			{
				if (usleep() != 0)
				pthread_mutex_unlock(&dongle->dongle_mutex);
				return (1);
			}

			pthread_cond_timedwait(&coder->coder_cond, &dongle->dongle_mutex,
				&dongle->cooldown_until);
			clock_gettime(CLOCK_MONOTONIC, &now);
			if (now.tv_sec > dongle->cooldown_until.tv_sec
				|| (now.tv_sec == dongle->cooldown_until.tv_sec
					&& now.tv_nsec >= dongle->cooldown_until.tv_nsec))
			{
				dongle->is_used = 0;
			}
		}
		else
		{
			/*
				* Dongle is currently being used.
				*/
			pthread_cond_wait(&coder->coder_cond, &dongle->dongle_mutex);
		}
	}
	if (checking_burnout(coder))
	{
		pthread_mutex_lock(&coder->monitor->monitor_mutex);
		coder->monitor->burnout_detected = 1;
		pthread_cond_signal(&coder->monitor->monitor_cond);
		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
		pthread_mutex_unlock(&dongle->dongle_mutex);
		return (1);
	}
	/*
		* We are first in the queue and the dongle
		* is available.
		*/
	dongle->is_used = 1;
	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	if (coder->monitor->burnout_detected)
	{
		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
		pthread_mutex_unlock(&dongle->dongle_mutex);
		return (1);
	}
	pthread_mutex_lock(&coder->simulation->logging_mutex);
	printf("%ld %d has taken a dongle\n", (convert_to_milisecond()
			- coder->simulation->start_time), coder->id);
	pthread_mutex_unlock(&coder->simulation->logging_mutex);
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
	dongle->ready_coder[0] = dongle->ready_coder[1];
	dongle->ready_coder[1] = NULL;
	pthread_mutex_unlock(&dongle->dongle_mutex);
	return (0);
}

int	request_dongles(t_coder *coder)
{
	if (coder->left == coder->right)
	{
		usleep(convert_to_microsecond(coder->time_to_burnout));
		pthread_cond_signal(&coder->monitor->monitor_cond);
		pthread_mutex_lock(&coder->monitor->monitor_mutex);
		pthread_mutex_lock(&coder->simulation->logging_mutex);
		printf("%ld %d burned out\n", convert_to_milisecond()
			- coder->simulation->start_time, coder->id);
		pthread_mutex_unlock(&coder->simulation->logging_mutex);
		coder->monitor->burnout_detected = 1;
		pthread_cond_signal(&coder->monitor->monitor_cond);
		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
		return (1);
	}
	if (coder->id % 2 == 0)
	{
		if (request_one_dongle(coder, coder->left))
			return (1);
		if (request_one_dongle(coder, coder->right))
			return (1);
	}
	else
	{
		if (request_one_dongle(coder, coder->right))
			return (1);
		if (request_one_dongle(coder, coder->left))
			return (1);
	}
	return (0);
}
