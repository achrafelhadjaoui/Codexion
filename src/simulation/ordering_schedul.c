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


static void	coder_to_wating_queue(t_coder *coder, t_dongle *dongle)
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


static int	wait_to_aquire_dongle(t_coder *coder, t_dongle *dongle)
{
	long	time_to_wait;

	while (simulation_stopped(coder) != 1
		&& (dongle->ready_coder[0] != coder || dongle->is_used != 0))
	{
		pthread_mutex_lock(&coder->waiting_mutex);
		coder->waiting_dongle = dongle;
		pthread_mutex_unlock(&coder->waiting_mutex);

		if (dongle->is_used == 2)
		{
			time_to_wait = dongle->cooldown_until
				- convert_to_milisecond();
			if (time_to_wait <= 0)
			{
				dongle->is_used = 0;
				continue ;
			}
			pthread_mutex_unlock(&dongle->dongle_mutex);
			if (usleep(1000) != 0)
				return (1);
			pthread_mutex_lock(&dongle->dongle_mutex);
		}
		else
		{
			if (pthread_cond_wait(&coder->coder_cond,
					&dongle->dongle_mutex) != 0)
			{
				pthread_mutex_lock(&coder->waiting_mutex);
				coder->waiting_dongle = NULL;
				pthread_mutex_unlock(&coder->waiting_mutex);

				pthread_mutex_unlock(&dongle->dongle_mutex);
				return (1);
			}
		}
	}

	pthread_mutex_lock(&coder->waiting_mutex);
	coder->waiting_dongle = NULL;
	pthread_mutex_unlock(&coder->waiting_mutex);

	return (0);
}

static int	request_one_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_mutex);

	coder_to_wating_queue(coder, dongle);

	if (wait_to_aquire_dongle(coder, dongle))
	{
		pthread_mutex_unlock(&dongle->dongle_mutex);
		return (1);
	}

	if (checking_burnout(coder))
	{
		pthread_mutex_unlock(&dongle->dongle_mutex);
		stop_threads(coder->monitor);
		return (1);
	}

	dongle->is_used = 1;

	if (simulation_stopped(coder))
	{
		pthread_mutex_unlock(&dongle->dongle_mutex);
		return (1);
	}

	pthread_mutex_lock(&coder->simulation->logging_mutex);
	printf("%ld %d has taken a dongle\n",
		convert_to_milisecond() - coder->simulation->start_time,
		coder->id);
	pthread_mutex_unlock(&coder->simulation->logging_mutex);

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
		pthread_mutex_lock(&coder->monitor->monitor_mutex);
		pthread_mutex_lock(&coder->simulation->logging_mutex);
		printf("%ld %d burned out\n",
			convert_to_milisecond()
			- coder->simulation->start_time,
			coder->id);
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



// static int	wait_to_aquire_dongle(t_coder *coder, t_dongle *dongle)
// {
// 	long	time_to_wait;
// 	int		ret;

// 	while (!simulation_stopped(coder)
// 		&& (dongle->ready_coder[0] != coder || dongle->is_used != 0))
// 	{
// 		pthread_mutex_lock(&coder->waiting_mutex);
// 		coder->waiting_dongle = dongle;

// 		if (dongle->is_used == 2)
// 		{
// 			time_to_wait = dongle->cooldown_until
// 				- convert_to_milisecond();

// 			if (time_to_wait <= 0)
// 			{
// 				dongle->is_used = 0;
// 				coder->waiting_dongle = NULL;
// 				pthread_mutex_unlock(&coder->waiting_mutex);
// 				continue ;
// 			}

// 			pthread_mutex_unlock(&coder->waiting_mutex);
// 			pthread_mutex_unlock(&dongle->dongle_mutex);

// 			if (usleep(1000) != 0)
// 			{
// 				pthread_mutex_lock(&coder->waiting_mutex);
// 				coder->waiting_dongle = NULL;
// 				pthread_mutex_unlock(&coder->waiting_mutex);
// 				return (1);
// 			}

// 			pthread_mutex_lock(&dongle->dongle_mutex);
// 		}
// 		else
// 		{
// 			/*
// 			 * coder_cond is ALWAYS associated with
// 			 * coder->waiting_mutex.
// 			 *
// 			 * Release dongle_mutex before waiting.
// 			 */
// 			pthread_mutex_unlock(&dongle->dongle_mutex);

// 			ret = pthread_cond_wait(&coder->coder_cond,
// 					&coder->waiting_mutex);

// 			pthread_mutex_lock(&dongle->dongle_mutex);

// 			if (ret != 0)
// 			{
// 				coder->waiting_dongle = NULL;
// 				pthread_mutex_unlock(&coder->waiting_mutex);
// 				return (1);
// 			}

// 			coder->waiting_dongle = NULL;
// 			pthread_mutex_unlock(&coder->waiting_mutex);
// 		}
// 	}

// 	pthread_mutex_lock(&coder->waiting_mutex);
// 	coder->waiting_dongle = NULL;
// 	pthread_mutex_unlock(&coder->waiting_mutex);

// 	return (0);
// }

// static int	request_one_dongle(t_coder *coder, t_dongle *dongle)
// {
// 	pthread_mutex_lock(&dongle->dongle_mutex);

// 	coder_to_wating_queue(coder, dongle);

// 	if (wait_to_aquire_dongle(coder, dongle))
// 	{
// 		pthread_mutex_unlock(&dongle->dongle_mutex);
// 		return (1);
// 	}

// 	if (checking_burnout(coder))
// 	{
// 		pthread_mutex_unlock(&dongle->dongle_mutex);
// 		stop_threads(coder->monitor);
// 		return (1);
// 	}

// 	dongle->is_used = 1;

// 	if (simulation_stopped(coder))
// 	{
// 		pthread_mutex_unlock(&dongle->dongle_mutex);
// 		return (1);
// 	}

// 	pthread_mutex_lock(&coder->simulation->logging_mutex);
// 	printf("%ld %d has taken a dongle\n",
// 		convert_to_milisecond() - coder->simulation->start_time,
// 		coder->id);
// 	pthread_mutex_unlock(&coder->simulation->logging_mutex);

// 	dongle->ready_coder[0] = dongle->ready_coder[1];
// 	dongle->ready_coder[1] = NULL;

// 	pthread_mutex_unlock(&dongle->dongle_mutex);
// 	return (0);
// }

// int	request_dongles(t_coder *coder)
// {
// 	if (coder->left == coder->right)
// 	{
// 		usleep(convert_to_microsecond(coder->time_to_burnout));

// 		pthread_mutex_lock(&coder->monitor->monitor_mutex);

// 		pthread_mutex_lock(&coder->simulation->logging_mutex);
// 		printf("%ld %d burned out\n",
// 			convert_to_milisecond()
// 			- coder->simulation->start_time,
// 			coder->id);
// 		pthread_mutex_unlock(&coder->simulation->logging_mutex);

// 		coder->monitor->burnout_detected = 1;
// 		pthread_cond_signal(&coder->monitor->monitor_cond);

// 		pthread_mutex_unlock(&coder->monitor->monitor_mutex);
// 		return (1);
// 	}

// 	if (coder->id % 2 == 0)
// 	{
// 		if (request_one_dongle(coder, coder->left))
// 			return (1);
// 		if (request_one_dongle(coder, coder->right))
// 			return (1);
// 	}
// 	else
// 	{
// 		if (request_one_dongle(coder, coder->right))
// 			return (1);
// 		if (request_one_dongle(coder, coder->left))
// 			return (1);
// 	}

// 	return (0);
// }

// void	wake_all_coders(t_coder *coder)
// {
// 	int	i;
// 	int	size;

// 	size = coder->nub_of_coders;
// 	i = 0;
// 	while (i < size)
// 	{
// 		pthread_mutex_lock(&coder[i].waiting_mutex);

// 		if (coder[i].waiting_dongle != NULL)
// 			pthread_cond_signal(&coder[i].coder_cond);

// 		pthread_mutex_unlock(&coder[i].waiting_mutex);
// 		i++;
// 	}
// }