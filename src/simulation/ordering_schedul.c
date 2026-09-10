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
	dongle->ready_coder[0] = dongle->ready_coder[1];
	dongle->ready_coder[1] = NULL;
}

/*
	* Take BOTH dongles or neither, and only ever from the head of BOTH
	* queues. A coder that is not first in line on one of its two dongles
	* takes nothing at all, even if the dongle in front of it is free.
	*
	* That is what makes fifo/edf mean something: a coder never overtakes
	* the coder the policy put ahead of it, and it never sits on one
	* dongle while queueing for the other.
	*
	* This cannot lock up: the queues are ordered by one strict total
	* order, so the smallest-key queued coder is by definition the head
	* of both of its queues and can always move forward.
	*/
static int	take_both_if_head(t_coder *coder)
{
	int	left_free;
	int	right_free;

	left_free = dongle_is_free(coder->left, coder);
	right_free = dongle_is_free(coder->right, coder);
	if (!left_free || !right_free)
		return (0);
	coder->last_compile = convert_to_milisecond();
	take_one_dongle(coder, coder->left);
	take_one_dongle(coder, coder->right);
	log_compile_start(coder);
	return (1);
}

/*
	* Queued on BOTH dongles for the whole request, so the policy sees the
	* coder on every dongle it is competing for.
	*
	* start is stamped here and last_compile when the pair is taken, both
	* under the two dongle mutexes, so the keys a neighbour reads while
	* ordering its own queue are never written concurrently.
	*
	* The very first request keeps the stamp it was born with. Every coder
	* becomes ready at the same instant, the start of the simulation, so
	* that is genuinely when its first request arrives; reading the clock
	* here would record how long pthread_create() happened to take, and
	* that is enough to drop two coders into different milliseconds and
	* lose the tie the ring needs to start off in step.
	*/
static void	queue_on_both(t_coder *coder)
{
	if (!coder->first_request)
		coder->start = convert_to_milisecond();
	coder->first_request = 0;
	coder_to_wating_queue(coder, coder->left);
	coder_to_wating_queue(coder, coder->right);
}

/*
	* Blocks until something actually changes, instead of polling.
	*
	* The left mutex is dropped first, but the right one is held without a
	* gap until pthread_cond_wait() releases it. Every wake-up aimed at
	* this coder (a dongle released, a cooldown expired, the simulation
	* stopping) is broadcast while holding that same right mutex, so the
	* signal can never slip through before the wait starts.
	*/
static void	wait_on_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left->dongle_mutex);
	pthread_cond_wait(&coder->right->dongle_cond,
		&coder->right->dongle_mutex);
	pthread_mutex_unlock(&coder->right->dongle_mutex);
	lock_two_dongles(coder->left, coder->right);
}

int	request_dongles(t_coder *coder)
{
	int	taken;

	if (coder->left == coder->right)
		return (wait_alone(coder));
	taken = 0;
	lock_two_dongles(coder->left, coder->right);
	queue_on_both(coder);
	while (!taken && !simulation_stopped(coder))
	{
		taken = take_both_if_head(coder);
		if (!taken)
			wait_on_dongles(coder);
	}
	unlock_two_dongles(coder->left, coder->right);
	return (!taken);
}
