/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   release_dongles.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:45:31 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:45:39 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static void	release_one_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_mutex);
	/*
		* Dongle enters cooldown.
		*/
	dongle->is_used = 2;

	dongle->cooldown_until = convert_to_milisecond() + dongle->cool_down;

	/*
		* Wake the next coder.
		*
		* It will notice that the dongle is in cooldown
		* and use pthread_cond_timedwait().
		*/
	if (dongle->ready_coder[0])
	{
		pthread_cond_signal(&dongle->ready_coder[0]->coder_cond);
	}
	pthread_mutex_unlock(&dongle->dongle_mutex);
}

void	release_dongles(t_coder *coder)
{
	release_one_dongle(coder->left);
	release_one_dongle(coder->right);
	/*
		* Nothing else here.
		*
		* The coder can now immediately enter
		* debugging/refactoring.
		*/
}
