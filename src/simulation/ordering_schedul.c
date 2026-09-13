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

static void	queue_on_both(t_coder *coder)
{
	if (!coder->first_request)
		coder->start = convert_to_milisecond();
	coder->first_request = 0;
	coder_to_wating_queue(coder, coder->left);
	coder_to_wating_queue(coder, coder->right);
}

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
