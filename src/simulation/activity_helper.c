/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   activity_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 20:58:35 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 21:03:01 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

int	wait_for_activity(t_coder *coder, int duration)
{
	long	end_time;
	long	remaining;
	int		tick;

	end_time = convert_to_milisecond() + duration;
	tick = 0;
	while (1)
	{
		if (tick % 10 == 0 && simulation_stopped(coder))
			return (1);
		tick++;
		remaining = end_time - convert_to_milisecond();
		if (remaining <= 0)
			break ;
		if (remaining > 1)
			usleep(1000);
		else
			usleep(100);
	}
	return (simulation_stopped(coder));
}
