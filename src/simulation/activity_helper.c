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

/*
	* Compiling, debugging and refactoring all end because a duration has
	* passed, not because anything happened, so there is nothing to wait
	* for and this stays a timed sleep.
	*
	* It is cut into 1 ms slices only so that a coder notices the
	* simulation stopping instead of finishing its whole phase first.
	* Watching for the burnout itself is not this coder's job any more;
	* the monitor does that.
	*/
int	wait_for_activity(t_coder *coder, int duration)
{
	long	end_time;

	end_time = convert_to_milisecond() + duration;
	while (convert_to_milisecond() < end_time)
	{
		if (simulation_stopped(coder))
			return (1);
		usleep(1000);
	}
	return (simulation_stopped(coder));
}
