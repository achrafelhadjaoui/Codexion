/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduel_ordering_helper.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 21:08:59 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/10 06:43:11 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

void	coder_to_wating_queue(t_coder *coder, t_dongle *dongle)
{
	logic_implementation(dongle, coder);
}

void	lock_two_dongles(t_dongle *left, t_dongle *right)
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

void	unlock_two_dongles(t_dongle *left, t_dongle *right)
{
	pthread_mutex_unlock(&left->dongle_mutex);
	pthread_mutex_unlock(&right->dongle_mutex);
}

/*
	* is_used == 2 means "released, still cooling down". A cooldown that
	* has already run out is read as free right here rather than waited
	* on, because the only thing left to do is relabel the dongle and the
	* coder about to take it can do that itself by taking it.
	*
	* Treating 2 as busy until the monitor had relabelled it cost a full
	* extra round trip on every single handover - release, wake, look,
	* sleep again, sweep, wake again - and with dongle_cooldown at 0 that
	* is pure loss. It is milliseconds per compile, and it piles up into
	* exactly the kind of drift that turns a deadline a coder should make
	* into one it misses.
	*
	* Clearing the flag without taking the dongle is still not done here:
	* the coder that notices is not always the coder that is next in
	* line, and whoever clears it silently swallows the event that wakes
	* the others. Left at 2, the monitor still announces the expiry to
	* everyone waiting on it.
	*
	* A dongle is only ever handed to the coder the policy put at the head
	* of its queue. No overtaking, whatever the queue looks like.
	*/
int	dongle_is_free(t_dongle *dongle, t_coder *coder)
{
	if (dongle->is_used == 1)
		return (0);
	if (dongle->is_used == 2
		&& convert_to_milisecond() < dongle->cooldown_until)
		return (0);
	return (dongle->ready_coder[0] == coder);
}

/*
	* One coder means one dongle, and compiling needs two held at the same
	* time, so there is no pair here to take and the coder never reaches
	* any state worth logging. It just blocks, and the monitor wakes it
	* once it has reported the burnout.
	*/
int	wait_alone(t_coder *coder)
{
	pthread_mutex_lock(&coder->right->dongle_mutex);
	while (!simulation_stopped(coder))
		pthread_cond_wait(&coder->right->dongle_cond,
			&coder->right->dongle_mutex);
	pthread_mutex_unlock(&coder->right->dongle_mutex);
	return (1);
}
