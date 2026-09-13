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

int	dongle_is_free(t_dongle *dongle, t_coder *coder)
{
	if (dongle->is_used == 1)
		return (0);
	if (dongle->is_used == 2
		&& convert_to_milisecond() < dongle->cooldown_until)
		return (0);
	return (dongle->ready_coder[0] == coder);
}

int	wait_alone(t_coder *coder)
{
	pthread_mutex_lock(&coder->right->dongle_mutex);
	while (!simulation_stopped(coder))
		pthread_cond_wait(&coder->right->dongle_cond,
			&coder->right->dongle_mutex);
	pthread_mutex_unlock(&coder->right->dongle_mutex);
	return (1);
}
