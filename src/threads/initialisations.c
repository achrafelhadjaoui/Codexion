/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialisations.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:46:48 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 22:15:28 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static void	dongle_initialisation(int *arg, t_dongle *dongle, char *policy)
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
		dongle[i].cooldown_until = 0;
		dongle[i].sibling = &dongle[(i + 1) % arg[0]];
		pthread_mutex_init(&dongle[i].dongle_mutex, NULL);
		pthread_cond_init(&dongle[i].dongle_cond, NULL);
		i++;
	}
}

static void	coder_initialisation(int *arg, t_dongle *dongle, t_coder *coder,
		t_sim_and_mon *sim_mon)
{
	int				i;
	t_monitor		*monitor;
	t_simulation	*simulation;

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
		coder[i].start = simulation->start_time;
		coder[i].first_request = 1;
		coder[i].last_compile = simulation->start_time;
		coder[i].monitor = monitor;
		coder[i].simulation = simulation;
		i++;
	}
}

void	prefill_waiting_queues(t_coder *coder, int size)
{
	int	i;

	coder->simulation->stop_logging = 0;
	i = 0;
	while (i < size)
	{
		coder[i].finished = 0;
		coder_to_wating_queue(&coder[i], coder[i].left);
		coder_to_wating_queue(&coder[i], coder[i].right);
		i++;
	}
}

static void	monitor_initialisation(t_monitor *monitor, t_coder *coder,
		int num_of_running)
{
	monitor->burnout_detected = 0;
	monitor->finish_running = num_of_running;
	monitor->coders = coder;
	pthread_mutex_init(&monitor->monitor_mutex, NULL);
}

void	initialisation_and_creating_threads(int *arg, char *policy)
{
	t_coder			*coder;
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
	simulation.start_time = convert_to_milisecond();
	sim_mon.sim = &simulation;
	sim_mon.mon = &monitor;
	pthread_mutex_init(&simulation.logging_mutex, NULL);
	dongle_initialisation(arg, dongle, policy);
	monitor_initialisation(&monitor, coder, arg[0]);
	coder_initialisation(arg, dongle, coder, &sim_mon);
	prefill_waiting_queues(coder, arg[0]);
	coder_and_monitor_thread_creation(coder, arg[0], &monitor);
	destroy_and_free(coder, dongle, &sim_mon, arg[0]);
}
