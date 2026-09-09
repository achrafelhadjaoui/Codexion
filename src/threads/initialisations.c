/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialisations.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:46:48 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:46:52 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static void free_mem(t_coder *coder, t_dongle *dongle)
{
	free(dongle);
	free(coder);
}

static void destroying_initilized_dongles(t_dongle *dongle, int size)
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

static void destroying_initilaized_monitor(t_monitor *monitor)
{
	pthread_mutex_destroy(&monitor->monitor_mutex);
	pthread_cond_destroy(&monitor->monitor_cond);
	pthread_cond_destroy(&monitor->activity_cond);
}

static void destroy_and_free(t_coder *coder, t_dongle *dongle, t_sim_and_mon *sim_and_mon, int size)
{
	t_monitor *monitor;
	t_simulation *sim;

	monitor = sim_and_mon->mon;
	sim = sim_and_mon->sim;

	pthread_mutex_destroy(&sim->logging_mutex);
	destroying_initilized_dongles(dongle, size);
	destroying_initilaized_monitor(monitor);
	free_mem(coder, dongle);
}

static int	dongle_initialisation(int *arg, t_dongle *dongle, char *policy)
{
	int	i;

	i = 0;
	while (i < arg[0])
	{
		dongle[i].id = i + 1;
		dongle[i].is_used = 0;
		dongle[i].cool_down = arg[6];
		dongle[i].policy = policy;
		dongle[i].ready_coder[0] = NULL;
		dongle[i].ready_coder[1] = NULL;
		dongle[i].used_by = NULL;
		if (pthread_mutex_init(&dongle[i].dongle_mutex, NULL) != 0)
		{
			destroying_initilized_dongles(dongle, i);
			return (1);
		}
		if (pthread_cond_init(&dongle[i].dongle_cond, NULL) != 0)
		{
			pthread_mutex_destroy(&dongle[i].dongle_mutex);
			destroying_initilized_dongles(dongle, i);
			return (1);
		}
		i++;
	}
	return 0;
}


static int	coder_initialisation(int *arg, t_dongle *dongle, t_coder *coder,
		t_sim_and_mon *sim_mon)
{
	t_monitor		*monitor;
	t_simulation	*simulation;
	int			i;

	monitor = sim_mon->mon;
	simulation = sim_mon->sim;
	i = 0;
	while (i < arg[0])
	{
		coder[i].id = i + 1;
		coder[i].left = &dongle[(i - 1 + arg[0]) % arg[0]];
		coder[i].right = &dongle[i];
		coder[i].nub_of_coders = arg[0];
		coder[i].time_to_burnout = arg[1];
		coder[i].time_to_compile = arg[2];
		coder[i].time_to_debug = arg[3];
		coder[i].time_to_refac = arg[4];
		coder[i].nub_of_compiles = arg[5];
		coder[i].last_compile = simulation->start_time;
		coder[i].monitor = monitor;
		/*
			* All coders point to the SAME simulation.
			*
			* Coders own no synchronisation primitive of their own:
			* they always wait on the cond of the dongle they want.
			*/
		coder[i].simulation = simulation;
		i++;
	}
	return 0;
}

static int	monitor_initialisation(t_monitor *monitor, t_coder *coder,
		int num_of_running)
{
	monitor->burnout_detected = 0;
	monitor->finish_running = num_of_running;
	monitor->coders = coder;
	if (pthread_mutex_init(&monitor->monitor_mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&monitor->monitor_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&monitor->monitor_mutex);
		return (1);
	}
	if (pthread_cond_init(&monitor->activity_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&monitor->monitor_mutex);
		pthread_cond_destroy(&monitor->monitor_cond);
		return (1);
	}
	return (0);
}



void	initialisation_and_creating_threads(int *arg, char *policy)
{
	t_coder		*coder;
	t_dongle		*dongle;
	t_simulation	simulation;
	t_monitor		monitor;
	t_sim_and_mon	sim_mon;

	dongle = malloc(sizeof(*dongle) * arg[0]);
	if (!dongle)
		return ;
	coder = malloc(sizeof(*coder) * arg[0]);
	if (!coder)
	{
		free(dongle);
		return ;
	}
	/*
		* The simulation starts here.
		*
		* This value is shared by all coders.
		*/
	simulation.start_time = convert_to_milisecond();
	sim_mon.sim = &simulation;
	sim_mon.mon = &monitor;
	if (pthread_mutex_init(&simulation.logging_mutex, NULL) != 0)
	{
		free_mem(coder, dongle);
		return ;
	}
	if (dongle_initialisation(arg, dongle, policy))
	{
		pthread_mutex_destroy(&simulation.logging_mutex);
		free_mem(coder, dongle);
		return ;
	}
	if (monitor_initialisation(&monitor, coder, arg[0]))
	{
		pthread_mutex_destroy(&simulation.logging_mutex);
		destroying_initilized_dongles(dongle, arg[0]);
		free_mem(coder, dongle);
		return ;

	}
	if (coder_initialisation(arg, dongle, coder, &sim_mon))
	{
		pthread_mutex_destroy(&simulation.logging_mutex);
		destroying_initilized_dongles(dongle, arg[0]);
		destroying_initilaized_monitor(&monitor);
		free_mem(coder, dongle);
		return ;
	}
	coder_and_monitor_thread_creation(coder, arg[0], &monitor);
	destroy_and_free(coder, dongle, &sim_mon, arg[0]);
}