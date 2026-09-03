#include "../head.h"



static void dongle_initialisation(int *arg, DONGLE *dongle, char *policy)
{
    int i;
    
    i = 0;
    while (i < arg[0])
    {
        dongle[i].id = i;
        dongle[i].is_used = 0;
        dongle[i].cool_down = arg[6];
        dongle[i].policy = policy;
        dongle[i].ready_coder[0] = NULL;
        dongle[i].ready_coder[1] = NULL;
        dongle[i].used_by = NULL;

        pthread_mutex_init(&dongle[i].dongle_mutex, NULL);
        i++;
    }
}


static void coder_initialisation(
    int *arg,
    DONGLE *dongle,
    CODER *coder,
    SIM_AND_MON *sim_mon)
{
    MONITOR *monitor;
    SIMULATION *simulation;
    int i;

    monitor = sim_mon->mon;
    simulation = sim_mon->sim;
    i = 0;
    while (i < arg[0])
    {
        coder[i].id = i;

        coder[i].left =
            &dongle[(i - 1 + arg[0]) % arg[0]];

        coder[i].right =
            &dongle[i];

        coder[i].time_to_burnout = arg[1];
        coder[i].time_to_compile = arg[2];
        coder[i].time_to_debug = arg[3];
        coder[i].time_to_refac = arg[4];
        coder[i].nub_of_compiles = arg[5];
        coder[i].last_compile = simulation->start_time;
        coder[i].monitor = monitor;

        /*
         * All coders point to the SAME simulation.
         */
        coder[i].simulation = simulation;

        pthread_cond_init(
            &coder[i].coder_cond,
            NULL
        );

        i++;
    }
}


static void monitor_initialisation(MONITOR *monitor, int num_of_running)
{
    monitor->burnout_detected = 0;
    monitor->finish_running = num_of_running;

    pthread_mutex_init(&monitor->monitor_mutex, NULL);
    pthread_cond_init(&monitor->monitor_cond, NULL);
    pthread_cond_init(&monitor->activity_cond, NULL);
}

void initialisation_and_creating_threads(int *arg, char *policy)
{
    CODER      *coder;
    DONGLE     *dongle;
    SIMULATION simulation;
    MONITOR monitor;
    SIM_AND_MON sim_mon;

    dongle = malloc(sizeof(*dongle) * arg[0]);
    if (!dongle)
    {
        free(arg);
        exit(3);
    }

    coder = malloc(sizeof(*coder) * arg[0]);
    if (!coder)
    {
        free(dongle);
        free(arg);
        exit(3);
    }

    /*
     * The simulation starts here.
     *
     * This value is shared by all coders.
     */
    simulation.start_time = convert_to_milisecond();
    sim_mon.sim = &simulation;
    sim_mon.mon = &monitor;
    pthread_mutex_init(&simulation.logging_mutex, NULL);


    dongle_initialisation(arg, dongle, policy);

    monitor_initialisation(&monitor, arg[0]);

    coder_initialisation(
        arg,
        dongle,
        coder,
        &sim_mon
    );

   

    coder_and_monitor_thread_creation(coder, arg[0], &monitor, dongle);

    free(coder);
    free(dongle);
}