/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   release_dongles.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:45:31 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 21:20:02 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static void	release_one_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_mutex);
	dongle->is_used = 2;
	dongle->used_by = NULL;
	dongle->cooldown_until = convert_to_milisecond() + dongle->cool_down;
	pthread_cond_broadcast(&dongle->dongle_cond);
	pthread_mutex_unlock(&dongle->dongle_mutex);
	notify_sibling(dongle);
}

void	release_dongles(t_coder *coder)
{
	release_one_dongle(coder->left);
	release_one_dongle(coder->right);
}
