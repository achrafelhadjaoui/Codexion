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
	* is_used == 2 means "released, still cooling down", and clearing it
	* is the monitor's job alone.
	*
	* A coder must never clear it here, even though it easily could: the
	* coder that notices is not always the coder that is next in line, and
	* whoever clears the flag silently swallows the event. The one still
	* waiting would then sleep on a dongle that quietly became free.
	*
	* A dongle is only ever handed to the coder the policy put at the head
	* of its queue. No overtaking, whatever the queue looks like.
	*/
int	dongle_is_free(t_dongle *dongle, t_coder *coder)
{
	if (dongle->is_used != 0)
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
