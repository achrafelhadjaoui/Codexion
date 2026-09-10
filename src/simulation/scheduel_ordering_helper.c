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
