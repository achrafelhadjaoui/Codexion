/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   head.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:41:30 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:41:44 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIB_H
#define LIB_H


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>



/*=========================================================
   Forward Declarations
   These tell the compiler these structs exist.
=========================================================*/
typedef struct coder CODER;
typedef struct dongle DONGLE;
typedef struct monitor MONITOR;
typedef struct s_simulation SIMULATION;
typedef struct sim_and_mon SIM_AND_MON;


/*=========================================================
   SIMULATION
=========================================================*/
typedef struct s_simulation
{
   long start_time;
   pthread_mutex_t logging_mutex;
} SIMULATION;


/*=========================================================
   SIM & MON
=========================================================*/
typedef struct sim_and_mon
{
   SIMULATION *sim;
   MONITOR *mon;
} SIM_AND_MON;


/*=========================================================
   DONGLE
=========================================================*/
typedef struct dongle
{
    int id;
    int cool_down;
    int is_used;
    char *policy;
    struct timespec cooldown_until;

    pthread_mutex_t dongle_mutex;

    CODER *used_by;
    CODER *ready_coder[2];
} DONGLE;


/*=========================================================
   CODER
=========================================================*/
typedef struct coder
{
   int id;

   int nub_of_coders;
   int time_to_burnout;
   int time_to_compile;
   int time_to_debug;
   int time_to_refac;
   int nub_of_compiles;
   long start;
   long last_compile;
   int time_until_burnout;
   SIMULATION *simulation;
   MONITOR *monitor;

   DONGLE *left;
   DONGLE *right;

   pthread_t thread;
   pthread_cond_t coder_cond;
} CODER;


/*=========================================================
   MONITOR
=========================================================*/
typedef struct monitor
{

   int burnout_detected;
   int finish_running;
   CODER *coders;

   pthread_t monitor_thread;
   pthread_mutex_t monitor_mutex;
   pthread_cond_t monitor_cond;
   pthread_cond_t activity_cond;
} MONITOR;

/*=========================================================
   Function Prototypes
=========================================================*/
void initialisation_and_creating_threads(int *data, char *policy);
void coder_and_monitor_thread_creation(CODER *coder, int size, MONITOR *montor, DONGLE *dongle);
long convert_to_milisecond();
long convert_to_microsecond(int nb);
int request_dongles(CODER *coder);
void fifo_implementation(DONGLE *dongle, CODER *coder);
void edf_implementation(DONGLE *dongle, CODER *coder);
void release_dongles(CODER *coder);
int debuging(CODER *coder);
int refactoring(CODER *coder);
void get_abstime(struct timespec *abstime, long milliseconds);
int compiling(CODER *coder);
int checking_burnout(CODER *coder);
void wake_all_coders(CODER *coder);


#endif

