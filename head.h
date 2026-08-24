#ifndef LIB_H
#define LIB_H


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


/*=========================================================
   Forward Declarations
   These tell the compiler these structs exist.
=========================================================*/
typedef struct coder CODER;
typedef struct dongle DONGLE;
typedef struct scheduler SCHEDULER;
typedef struct monitor MONITOR;

/*=========================================================
   Enumerations
=========================================================*/
typedef enum state
{
   READY,
   RUNNING,
   WAITING,
   BLOCKED
} STATE;

// typedef enum neighbour
// {
//    LEFT,
//    RIGHT
// } NEIGHBOUR;

/*=========================================================
   DONGLE
=========================================================*/
typedef struct dongle
{
    int id;
    int cool_down;
    int is_used;

   pthread_mutex_t mutex;

    /* Which coder is currently using this dongle? */
    CODER *dongle_used_by;
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

   /* Pointer to the dongle currently assigned */
   // DONGLE *left_dongle;
   // DONGLE *right_dongle;
   DONGLE *own_dongle;
   DONGLE *left;
   DONGLE *right;

   /* Current state of the coder */
   STATE state;


   /* Optional thread for this coder */
   pthread_t thread;
} CODER;



/*=========================================================
   SCHEDULER
=========================================================*/
typedef struct scheduler
{
   CODER **queue;
   char *policy;
} SCHEDULER;

/*=========================================================
   MONITOR
=========================================================*/
struct monitor
{
   /* Add monitoring variables later */
};


/*=========================================================
   Function Prototypes
=========================================================*/
void initialisation(int *data, char *policy, int size);


#endif

