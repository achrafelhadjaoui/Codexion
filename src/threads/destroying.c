/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroying.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 22:22:35 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 22:22:42 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

void	free_mem(t_coder *coder, t_dongle *dongle)
{
	free(dongle);
	free(coder);
}

void	destroying_initilized_dongles(t_dongle *dongle, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		pthread_mutex_destroy(&dongle[i].dongle_mutex);
		pthread_cond_destroy(&dongle[i].dongle_cond);
		i++;
	}
}

void	destroying_initilaized_monitor(t_monitor *monitor)
{
	pthread_mutex_destroy(&monitor->monitor_mutex);
	pthread_cond_destroy(&monitor->monitor_cond);
	pthread_cond_destroy(&monitor->activity_cond);
}

void	destroy_and_free(t_coder *coder, t_dongle *dongle,
		t_sim_and_mon *sim_and_mon, int size)
{
	t_monitor		*monitor;
	t_simulation	*sim;

	monitor = sim_and_mon->mon;
	sim = sim_and_mon->sim;
	pthread_mutex_destroy(&sim->logging_mutex);
	destroying_initilized_dongles(dongle, size);
	destroying_initilaized_monitor(monitor);
	free_mem(coder, dongle);
}
