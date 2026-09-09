/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:42:07 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 04:12:53 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "head.h"

static int	ft_atoi(char *nb)
{
	long	res;
	int		i;

	res = 0;
	i = 0;
	if (nb[i] == '-' || nb[i] == '\0')
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

static void	to_lower(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
		i++;
	}
}

static int	checking_type(int *arg, char *av[], char **policy)
{
	int	i;
	int	res;

	i = 0;
	while (i < 7)
	{
		res = ft_atoi(av[i + 1]);
		if (res == -1)
			return (0);
		arg[i] = res;
		i++;
	}
	if (arg[0] < 1)
		return (0);
	*policy = av[8];
	to_lower(*policy);
	if (strcmp(*policy, "fifo") != 0 && strcmp(*policy, "edf") != 0)
		return (0);
	return (1);
}

int	main(int ac, char *av[])
{
	int		*arg;
	char	*policy;

	if (ac != 9)
		return (0);
	arg = malloc(sizeof(*arg) * 7);
	if (!arg)
		return (0);
	if (!checking_type(arg, av, &policy))
	{
		free(arg);
		return (0);
	}
	initialisation_and_creating_threads(arg, policy);
	free(arg);
	return (0);
}
