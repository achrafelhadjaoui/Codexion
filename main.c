#include "head.h"

int ft_atoi(char *nb)
{
    long res = 0;
    int i;

    i = 0;
    if (nb[i] == '-')
        return -1;
    while (nb[i])
    {
        if (res > 2147483647)
            return -1;
        res = (res * 10) + (nb[i] - 48);
        i++;
    }
    return res;
}

int main(int ac, char *av[])
{
    int *arg, i = 0, res;
    char **ptr = ++av;

    if (ac < 9)
        return 0;

    arg = (int *)malloc(sizeof(*arg) * 7);
    if (!arg)
        return 0;

    i = 0;
    while (i < 7)
    {
        res = ft_atoi(ptr[i]);
        if (res == -1)
        {
            free(arg);
            return 0;
        }
        arg[i] = res;
        i++;
    }
    if (strcmp(ptr[i], "fifo") == 0 || strcmp(ptr[i], "edf") == 0)
    {
        printf("%s\n", ptr[i]);
        i = 0;
    }
    else
    {
        free(arg);
        printf("not a numbeer\n");
        return 0;
    }

    initialisation(arg, av[ac - 1], 7);
    return 0;
}