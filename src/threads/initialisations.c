// #include "../head.h"

// void initialisation(int *arg, char *policy, int size)
// {
//     CODER *coder;
//     DONGLE *dongle;
//     // check the using of size
//     (void)size;
    
//     dongle = malloc(sizeof(*dongle) * arg[0]);
//     if (!dongle)
//     {
//         free(arg);
//         exit(3);
//     }

//     coder = malloc(sizeof(*coder) * arg[0]);
//     if (!coder)
//     {
//         free(arg);
//         exit(3);


//     }

//     dongle_initialisation(arg, dongle, size, policy);
//     coder_initialisation(arg, dongle, coder, size);
// }

// void dongle_initialisation(int *arg, DONGLE *dongle,int size, char *policy)
// {

//     // check the usiing of size or deleete it
//     (void)size;
//     int i;
    
//     i = 0;
//     while (i < arg[0])
//     {
//         dongle[i].id = i;
//         dongle[i].is_used = 0;
//         dongle[i].cool_down = arg[6];
//         dongle[i].policy = policy;
//         dongle[i].ready_coder[0] = NULL;
//         dongle[i].ready_coder[1] = NULL;

//         pthread_mutex_init(&dongle[i].dongle_mutex, NULL);
//         i++;
//     }
// }

// void coder_initialisation(int *arg, DONGLE *dongle, CODER *coder, int size)
// {
//     // check the using of size
//     (void)size;
//     int i;
    
//     i = 0;
//     while(i < arg[0])
//     {
//         coder[i].id = i;
//         coder[i].left = &dongle[(i - 1 + arg[0]) % arg[0]];
//         coder[i].right = &dongle[i];
//         coder[i].time_to_burnout = arg[1];
//         coder[i].time_to_compile = arg[2];
//         coder[i].time_to_debug = arg[3];
//         coder[i].time_to_refac = arg[4];
//         coder[i].nub_of_compiles = arg[5];
//         pthread_cond_init(&coder[i].coder_cond, NULL);
//         // thread_creation(&coder[i]);
//         i++;
//     }

//     i = 0;
//     while (i < arg[0])
//     {
//         thread_creation(&coder[i]);
//         i++;
//     }

//     i = 0;
//     while (i < arg[0])
//     {
//         if (pthread_join(coder[i].thread, NULL) != 0)
//             exit(3);
//         i++;
//     }
// }










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

        pthread_mutex_init(&dongle[i].dongle_mutex, NULL);
        i++;
    }
}


static void coder_initialisation(
    int *arg,
    DONGLE *dongle,
    CODER *coder,
    SIMULATION *simulation)
{
    int i;

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


static void monitor_initialisation(CODER *coder, MONITOR *monitor, int burnout)
{
    monitor->time_to_burnout = burnout;
    monitor->burnout_detected = 0;
    monitor->coder = coder;

    pthread_mutex_init(&monitor->monitor_mutex, NULL);
    pthread_cond_init(&monitor->monitor_cond, NULL);
}

void initialisation_and_creating_threads(int *arg, char *policy)
{
    CODER      *coder;
    DONGLE     *dongle;
    SIMULATION simulation;
    MONITOR monitor;

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


    dongle_initialisation(arg, dongle, policy);


    coder_initialisation(
        arg,
        dongle,
        coder,
        &simulation
    );

    monitor_initialisation(coder, &monitor, arg[1]);

    coder_and_monitor_thread_creation(coder, arg[0], &monitor);

    free(coder);
    free(dongle);
}