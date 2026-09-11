/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   head.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:41:30 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/10 06:41:11 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEAD_H
# define HEAD_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_coder			t_coder;
typedef struct s_dongle			t_dongle;
typedef struct s_monitor		t_monitor;
typedef struct s_simulation		t_simulation;
typedef struct s_sim_and_mon	t_sim_and_mon;

typedef struct s_simulation
{
	long						start_time;
	int							stop_logging;
	pthread_mutex_t				logging_mutex;
}								t_simulation;

typedef struct s_sim_and_mon
{
	t_simulation				*sim;
	t_monitor					*mon;
}								t_sim_and_mon;

typedef struct s_dongle
{
	int							id;
	int							cool_down;
	int							is_used;
	char						*policy;
	long						cooldown_until;
	pthread_mutex_t				dongle_mutex;
	pthread_cond_t				dongle_cond;
	t_coder						*used_by;
	t_coder						*ready_coder[2];
	t_dongle					*sibling;
}								t_dongle;

typedef struct s_coder
{
	int							id;
	int							nub_of_coders;
	int							time_to_burnout;
	int							time_to_compile;
	int							time_to_debug;
	int							time_to_refac;
	int							nub_of_compiles;
	int							first_request;
	int							finished;
	long						start;
	long						last_compile;
	t_simulation				*simulation;
	t_monitor					*monitor;
	t_dongle					*left;
	t_dongle					*right;
	pthread_t					thread;
}								t_coder;

typedef struct s_monitor
{
	int							burnout_detected;
	int							finish_running;
	t_coder						*coders;
	pthread_t					monitor_thread;
	pthread_mutex_t				monitor_mutex;
}								t_monitor;

void							initialisation_and_creating_threads(int *data,
									char *policy);
void							coder_and_monitor_thread_creation(
									t_coder *coder,
									int size, t_monitor *montor);
void							*monitor_routine(void *arg);
long							convert_to_milisecond(void);
int								request_dongles(t_coder *coder);
void							logic_implementation(t_dongle *dongle,
									t_coder *coder);
void							release_dongles(t_coder *coder);
int								debuging(t_coder *coder);
int								refactoring(t_coder *coder);
int								compiling(t_coder *coder);
void							log_state(t_coder *coder, char *msg);
void							log_burnout(t_coder *coder);
void							log_compile_start(t_coder *coder);
void							wake_all_coders(t_coder *coder);
void							notify_sibling(t_dongle *dongle);
void							stop_threads(t_monitor *monitor);
int								simulation_stopped(t_coder *coder);
int								wait_for_activity(t_coder *coder, int duration);
void							coder_to_wating_queue(t_coder *coder,
									t_dongle *dongle);
void							lock_two_dongles(t_dongle *left,
									t_dongle *right);
void							unlock_two_dongles(t_dongle *left,
									t_dongle *right);
int								dongle_is_free(t_dongle *dongle,
									t_coder *coder);
int								wait_alone(t_coder *coder);
void							coder_finished(t_coder *coder);
void							prefill_waiting_queues(t_coder *coder,
									int size);
void							free_mem(t_coder *coder, t_dongle *dongle);
void							destroying_initilized_dongles(
									t_dongle *dongle, int size);
void							destroying_initilaized_monitor(
									t_monitor *monitor);
void							destroy_and_free(t_coder *coder,
									t_dongle *dongle,
									t_sim_and_mon *sim_and_mon, int size);

#endif
