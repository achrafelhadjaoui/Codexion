#include "../head.h"

void initialisation(int *arg, char *policy, int size)
{
    CODER *coder;
    DONGLE *dongle;
    
    dongle = malloc(sizeof(*dongle) * arg[0]);
    if (!dongle)
    {
        free(arg);
        exit(3);
    }

    coder = malloc(sizeof(*coder) * arg[0]);
    if (!coder)
    {
        free(arg);
        exit(3);
    }

    dongle_initialisation(arg, dongle, size);
    coder_initialisation(arg, dongle, coder, size);
}

void dongle_initialisation(int *arg, DONGLE *dongle, int size)
{
    int i;
    
    i = 0;
    while (i < size)
    {
        dongle[i].id = i;
        dongle[i].is_used = 0;
        dongle[i].cool_down = 0;
        i++;
    }
}

void coder_initialsation(int *arg, DONGLE *dongle, CODER *coder, int size)
{
    int i;
    
    i = 0;
    while(i < size)
    {
        coder[i].id = i;
        coder[i].own_dongle = &dongle[i];
        coder[i].left = &dongle[(i - 1 + size) % size];
        coder[i].right = &dongle[(i + 1) % size];
        coder[i].state = WAITING;
        thread_creation(&coder[i]);
        i++;
    }
}