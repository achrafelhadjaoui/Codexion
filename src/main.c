/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:42:07 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:42:13 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "head.h"

static int ft_atoi(char *nb)
{
    long    res;
    int     i;

    res = 0;
    i = 0;

    if (nb[i] == '-')
        return (-1);

    while (nb[i])
    {
        if (nb[i] < '0' || nb[i] > '9')
            return (-1);

        res = (res * 10) + (nb[i] - '0');

        if (res > 2147483647)
            return (-1);

        i++;
    }

    return ((int)res);
}

static void to_lower(char *str)
{
    int i;

    i = 0;
    while (str[i])
    {
        if(str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + 32;
        }
        i++;
    }
}

int main(int ac, char *av[])
{
    int     *arg;
    int     i;
    int     res;
    char    *policy;

    if (ac != 9)
        return (0);

    arg = malloc(sizeof(*arg) * 7);
    if (!arg)
        return (0);

    /*
     * Read the 7 numeric arguments.
     */
    i = 0;
    while (i < 7)
    {
        res = ft_atoi(av[i + 1]);

        if (res == -1)
        {
            free(arg);
            return (0);
        }

        arg[i] = res;
        i++;
    }

    /*
     * The policy is the 8th argument.
     */
    policy = av[8];
    to_lower(policy);
    if (strcmp(policy, "fifo") != 0 &&
        strcmp(policy, "edf") != 0)
    {
        free(arg);
        printf("invalid policy\n");
        return (0);
    }

    /*
     * 7 numeric arguments + policy.
     */
    initialisation_and_creating_threads(arg, policy);

    free(arg);

    return (0);
}
