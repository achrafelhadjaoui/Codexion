#include "../head.h"

// void *routine(void *args)
// {
//     CODER *coder = (CODER *)args;
    

//     while (coder->nub_of_compiles > 0)
//     {
//         printf("enter to rouutine %d\n", coder->id);
//         coder->start = convert_to_milisecond();

//         request_dongles(coder);
//         // strart thee compiling
//         usleep(convert_to_microsecond(coder->time_to_compile));
//         // dongle must enter the coolddown
//         release_dongles(coder);
//         //  start debuging
//         //  start refactoring
//         debuging_and_refactoring(coder);
//         coder->nub_of_compiles--;
//     }

//     return NULL;
// }




static void *routine(void *args)
{
    CODER *coder;

    coder = (CODER *)args;

    while (coder->nub_of_compiles > 0)
    {
        coder->start = convert_to_milisecond();
        request_dongles(coder);


        /*
         * compile
         */
        printf("%d %d is compiling\n", (convert_to_milisecond() - coder->simulation->start_time), coder->id);
        usleep(convert_to_microsecond(coder->time_to_compile));
        coder->last_compile = convert_to_milisecond();

        release_dongles(coder);
        //printf("CODER %i RELEASE DONGLE\n", coder->id);
        debuging_and_refactoring(coder);

        coder->nub_of_compiles--;
    }

    return (NULL);
}


static void *monitor_routine()
{
    return NULL;
}

void coder_and_monitor_thread_creation(CODER *coder, int size, MONITOR *monitor)
{
    int i;

    i = 0;
    while (i < size)
    {
        if (pthread_create(&coder[i].thread, NULL, routine, &coder[i]) != 0)
        {
            exit(3);
        }
        i++;
    }

    if (pthread_create(&monitor->monitor_thread, NULL, monitor_routine, NULL) != 0)
        exit(3);


    i = 0;
    while (i < size)
    {
        if (pthread_join(coder[i].thread, NULL) != 0)
            exit(3);

        i++;
    }

    if (pthread_join(monitor->monitor_thread, NULL) != 0)
        exit(3);
}
