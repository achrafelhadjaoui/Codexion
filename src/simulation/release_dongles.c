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
		* Wake every coder queued on THIS dongle.
		*
		* They re-test the predicate: whoever is at the head of
		* ready_coder[] takes it once the cooldown has elapsed.
		*/
	pthread_cond_broadcast(&dongle->dongle_cond);
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
