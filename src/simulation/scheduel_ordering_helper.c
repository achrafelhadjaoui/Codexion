/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduel_ordering_helper.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 21:08:59 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 21:09:03 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

void	coder_to_wating_queue(t_coder *coder, t_dongle *dongle)
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
int	head_is_blocked(t_dongle *dongle, t_coder *coder)
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
int	dongle_is_free(t_dongle *dongle, t_coder *coder)
{
	if (dongle->is_used == 2
		&& dongle->cooldown_until - convert_to_milisecond() <= 0)
		dongle->is_used = 0;
	if (dongle->is_used != 0)
		return (0);
	return (dongle->ready_coder[0] == coder
		|| head_is_blocked(dongle, coder));
}
