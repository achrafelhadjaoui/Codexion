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

/*
	* A phase no longer cuts itself short when the burnout deadline falls
	* inside it. The coder just does its work for the full duration; the
	* monitor is the one watching the deadline and it reports the burnout
	* the millisecond it passes, whatever the coder happens to be doing.
	*/
int	debuging(t_coder *coder)
{
	if (simulation_stopped(coder))
		return (1);
	log_state(coder, "is debugging");
	return (wait_for_activity(coder, coder->time_to_debug));
}

int	refactoring(t_coder *coder)
{
	if (simulation_stopped(coder))
		return (1);
	log_state(coder, "is refactoring");
	return (wait_for_activity(coder, coder->time_to_refac));
}

int	compiling(t_coder *coder)
{
	if (simulation_stopped(coder))
		return (1);
	return (wait_for_activity(coder, coder->time_to_compile));
}
