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

static void	take_one_dongle(t_coder *coder, t_dongle *dongle)
{
	dongle->is_used = 1;
	dongle->used_by = coder;
	if (dongle->ready_coder[0] == coder)
	{
		dongle->ready_coder[0] = dongle->ready_coder[1];
		dongle->ready_coder[1] = NULL;
	}
	else if (dongle->ready_coder[1] == coder)
		dongle->ready_coder[1] = NULL;
	log_state(coder, "has taken a dongle");
}

/*
	* Take BOTH dongles or neither.
	*
	* Holding one dongle while waiting for the other is what starved
	* coders: a coder could sit on a dongle a neighbour was queued for
	* while itself waiting seconds for its second one, and the neighbour
	* burned out on a dongle nobody was using.
	*
	* Either both are free inside the same critical section, or the coder
	* takes nothing and stays queued on both.
	*/
static int	try_to_take_both(t_coder *coder)
{
	int	left_free;
	int	right_free;

	lock_two_dongles(coder->left, coder->right);
	left_free = dongle_is_free(coder->left, coder);
	right_free = dongle_is_free(coder->right, coder);
	coder->left_free_last = left_free;
	coder->right_free_last = right_free;
	if (left_free && right_free)
	{
		coder->last_compile = convert_to_milisecond();
		if (coder->id % 2 == 0)
		{
			take_one_dongle(coder, coder->left);
			take_one_dongle(coder, coder->right);
		}
		else
		{
			take_one_dongle(coder, coder->right);
			take_one_dongle(coder, coder->left);
		}
	}
	unlock_two_dongles(coder->left, coder->right);
	return (left_free && right_free);
}

/*
	* Queued on BOTH dongles for the whole request.
	*
	* This is what makes fifo/edf actually order the coders: a coder that
	* is only queued on the dongle it is currently after keeps losing its
	* place on the other one.
	*
	* start is stamped here, and last_compile when the pair is taken, so
	* that both keys the neighbours read are only ever written under the
	* dongle mutexes that protect the queues.
	*/
static void	queue_on_both(t_coder *coder)
{
	lock_two_dongles(coder->left, coder->right);
	coder->start = convert_to_milisecond();
	coder->left_free_last = 1;
	coder->right_free_last = 1;
	coder_to_wating_queue(coder, coder->left);
	coder_to_wating_queue(coder, coder->right);
	unlock_two_dongles(coder->left, coder->right);
}

static int	burned_out_alone(t_coder *coder)
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

int	request_dongles(t_coder *coder)
{
	if (coder->left == coder->right)
		return (burned_out_alone(coder));
	queue_on_both(coder);
	while (simulation_stopped(coder) != 1)
	{
		if (try_to_take_both(coder))
			return (0);
		if (checking_burnout(coder))
		{
			stop_threads(coder->monitor);
			return (1);
		}
		usleep(500);
	}
	return (1);
}
