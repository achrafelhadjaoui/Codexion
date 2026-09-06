/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf_handling.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:44:44 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:44:50 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

void	edf_implementation(t_dongle *dongle, t_coder *coder)
{
	t_coder	*tmp;

	/*
		* EDF deadline:
		*
		* last compilation finish time
		* + allowed burnout time.
		*/
	coder->time_until_burnout = coder->last_compile + coder->time_to_burnout;
	if (dongle->ready_coder[0] == NULL)
	{
		dongle->ready_coder[0] = coder;
	}
	else if (dongle->ready_coder[1] == NULL)
	{
		if (coder->time_until_burnout < dongle->ready_coder[0]->time_until_burnout
			|| (coder->time_until_burnout == dongle->ready_coder[0]->time_until_burnout
				&& coder->id < dongle->ready_coder[0]->id))
		{
			tmp = dongle->ready_coder[0];
			dongle->ready_coder[0] = coder;
			dongle->ready_coder[1] = tmp;
		}
		else
		{
			dongle->ready_coder[1] = coder;
		}
	}
}
