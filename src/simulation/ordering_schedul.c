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

/*
	* Both dongle mutexes have to be held at the same time, so they are
	* always locked in id order.
	*
	* Two coders can therefore never hold them in opposite order, which is
	* what makes the pair atomic without deadlocking.
	*/
static void	lock_two_dongles(t_dongle *left, t_dongle *right)
{
	if (left->id < right->id)
	{
		pthread_mutex_lock(&left->dongle_mutex);
		pthread_mutex_lock(&right->dongle_mutex);
	}
	else
	{
		pthread_mutex_lock(&right->dongle_mutex);
		pthread_mutex_lock(&left->dongle_mutex);
	}
}

static void	unlock_two_dongles(t_dongle *left, t_dongle *right)
{
	pthread_mutex_unlock(&left->dongle_mutex);
	pthread_mutex_unlock(&right->dongle_mutex);
}

/*
	* Can the coder at the head of this dongle's queue actually use it
	* right now, or is it stuck on its OTHER dongle?
	*
	* A head that cannot move must not block the coder behind it: that
	* gate serialised the whole ring, one compile at a time, and every
	* coder then burned out exactly time_to_burnout after its last one.
	*
	* The head publishes the answer itself: on every attempt it records
	* whether each of its two dongles was available to it, and it does so
	* while holding BOTH dongle mutexes. Reading those two fields here is
	* therefore protected by this dongle's own mutex - the writer holds
	* it too - and no coder ever touches a dongle that is not its own.
	*
	* A hungrier head is never overtaken, blocked or not. Without that
	* floor the ring settles into a stable alternation - two pairs
	* compiling every cooldown, the fifth coder always bypassed on
	* whichever of its two dongles happens to be free that instant - and
	* that coder burns out while the others keep compiling.
	*/
static int	head_is_blocked(t_dongle *dongle, t_coder *coder)
{
	t_coder	*head;

	head = dongle->ready_coder[0];
	if (!head)
		return (1);
	if (head->last_compile < coder->last_compile)
		return (0);
	if (head->left == dongle)
		return (head->right_free_last == 0);
	return (head->left_free_last == 0);
}

/*
	* A dongle is available to this coder when its cooldown is over and
	* either the coder is at the head of its ready queue - which is where
	* the fifo/edf order is enforced - or that head cannot take it now.
	*
	* The cooldown flip lives here because nothing else ever clears it.
	*/
static int	dongle_is_free(t_dongle *dongle, t_coder *coder)
{
	if (dongle->is_used == 2
		&& dongle->cooldown_until - convert_to_milisecond() <= 0)
		dongle->is_used = 0;
	if (dongle->is_used != 0)
		return (0);
	return (dongle->ready_coder[0] == coder
		|| head_is_blocked(dongle, coder));
}

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