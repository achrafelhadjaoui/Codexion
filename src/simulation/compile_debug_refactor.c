/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compile_debug_refactor.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:43:07 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 21:02:10 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

int	debuging(t_coder *coder)
{
	long	current_time;
	long	debug_end;
	long	burnout_deadline;
	long	remaining;

	current_time = convert_to_milisecond();
	debug_end = current_time + coder->time_to_debug;
	burnout_deadline = coder->last_compile + coder->time_to_burnout;
	if (simulation_stopped(coder))
		return (1);
	log_state(coder, "is debugging");
	if (debug_end < burnout_deadline)
		return (wait_for_activity(coder, coder->time_to_debug));
	remaining = burnout_deadline - current_time;
	handle_burnout(remaining, coder);
	return (1);
}

int	refactoring(t_coder *coder)
{
	long	current_time;
	long	refac_end;
	long	burnout_deadline;
	long	remaining;

	current_time = convert_to_milisecond();
	refac_end = current_time + coder->time_to_refac;
	burnout_deadline = coder->last_compile + coder->time_to_burnout;
	if (simulation_stopped(coder))
		return (1);
	log_state(coder, "is refactoring");
	if (refac_end < burnout_deadline)
		return (wait_for_activity(coder, coder->time_to_refac));
	remaining = burnout_deadline - current_time;
	handle_burnout(remaining, coder);
	return (1);
}

int	compiling(t_coder *coder)
{
	long	current_time;
	long	compile_end;
	long	burnout_deadline;
	long	remaining;

	current_time = convert_to_milisecond();
	compile_end = current_time + coder->time_to_compile;
	burnout_deadline = coder->last_compile + coder->time_to_burnout;
	if (simulation_stopped(coder))
		return (1);
	log_state(coder, "is compiling");
	if (compile_end < burnout_deadline)
		return (wait_for_activity(coder, coder->time_to_compile));
	remaining = burnout_deadline - current_time;
	handle_burnout(remaining, coder);
	return (1);
}
