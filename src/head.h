/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   head.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:41:30 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 04:40:21 by aelhadja         ###   ########.fr       */
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
	t_coder						*used_by;
	t_coder						*ready_coder[2];
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
	long						start;
	long						last_compile;
	int							time_until_burnout;
	t_simulation				*simulation;
	t_monitor					*monitor;
	t_dongle					*left;
	t_dongle					*right;
	t_dongle					*waiting_dongle;
	pthread_t					thread;
	pthread_mutex_t				waiting_mutex;
	pthread_cond_t				coder_cond;
}								t_coder;

typedef struct s_monitor
{
	int							burnout_detected;
	int							finish_running;
	t_coder						*coders;
	pthread_t					monitor_thread;
	pthread_mutex_t				monitor_mutex;
	pthread_cond_t				monitor_cond;
	pthread_cond_t				activity_cond;
}								t_monitor;

void							initialisation_and_creating_threads(int *data,
									char *policy);
void							coder_and_monitor_thread_creation(
									t_coder	*coder,
									int size,
									t_monitor *montor);
long							convert_to_milisecond(void);
long							convert_to_microsecond(int nb);
int								request_dongles(t_coder *coder);
void							fifo_implementation(t_dongle *dongle,
									t_coder *coder);
void							edf_implementation(t_dongle *dongle,
									t_coder *coder);
void							release_dongles(t_coder *coder);
int								debuging(t_coder *coder);
int								refactoring(t_coder *coder);
void							get_abstime(struct timespec *abstime,
									long milliseconds);
int								compiling(t_coder *coder);
int								checking_burnout(t_coder *coder);
void							wake_all_coders(t_coder *coder);
void							stop_threads(t_monitor *monitor);
int								simulation_stopped(t_coder *coder);
void							mention_to_stop_threads(t_monitor *monitor);
#endif