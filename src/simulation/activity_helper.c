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
	*
	* The last millisecond is polled finer. A phase that overshoots by
	* most of a slice hands the dongles back late, the coders waiting for
	* them start late, and the whole ring keeps that lateness for the rest
	* of the run - every coder pays it again on every single wave. Costing
	* a handful of extra wake-ups once per phase to hand the dongles over
	* on time instead is a trade worth making.
	*
	* The stop flag is only read every tenth slice, because reading it
	* takes the one mutex every coder shares. Once per millisecond per
	* coder, that was hundreds of locks on a single mutex for every phase
	* and all of the coders queueing on it at once - work that grew with
	* the number of coders and bought nothing, since the flag is checked
	* again under a lock before anything is ever printed. Answering a stop
	* up to ten milliseconds later costs only a slightly later exit.
	*/
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
