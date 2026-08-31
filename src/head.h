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

typedef struct s_simulation
{
   int start_time;
} SIMULATION;



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

    CODER *ready_coder[2];
} DONGLE;


/*=========================================================
   CODER
=========================================================*/
typedef struct coder
{
   int id;


   int time_to_burnout;
   int time_to_compile;
   int time_to_debug;
   int time_to_refac;
   int nub_of_compiles;
   int start;
   int last_compile;
   int time_until_burnout;
   SIMULATION *simulation;

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
   /* Add monitoring variables later */
   int time_to_burnout;
   int burnout_detected;
   CODER *coder;

   pthread_t monitor_thread;
   pthread_mutex_t monitor_mutex;
   pthread_cond_t monitor_cond;
} MONITOR;

/*=========================================================
   Function Prototypes
=========================================================*/
void initialisation_and_creating_threads(int *data, char *policy);
void coder_and_monitor_thread_creation(CODER *coder, int size, MONITOR *montor);
int convert_to_milisecond();
int convert_to_microsecond(int nb);
void request_dongles(CODER *coder);
void fifo_implementation(DONGLE *dongle, CODER *coder);
void edf_implementation(DONGLE *dongle, CODER *coder);
void release_dongles(CODER *coder);
void debuging_and_refactoring(CODER *coder);

#endif

