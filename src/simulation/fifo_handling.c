/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:45:19 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:45:24 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

static int	coder_before(t_coder *coder1, t_coder *coder2, t_dongle *dongle)
{
	long	deadline1;
	long	deadline2;

	if (strcmp(dongle->policy, "edf") == 0)
	{
		deadline1 = coder1->last_compile + coder1->time_to_burnout;
		deadline2 = coder2->last_compile + coder2->time_to_burnout;
		if (deadline1 < deadline2)
			return (1);
		if (deadline1 == deadline2 && coder1->id < coder2->id)
			return (1);
		return (0);
	}
	if (coder1->start < coder2->start)
		return (1);
	if (coder1->start == coder2->start && coder1->id < coder2->id)
		return (1);
	return (0);
}

static void	swap(t_coder **a, t_coder **b)
{
	t_coder	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	heapify_up(t_coder **heap, int index, t_dongle *dongle)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (coder_before(heap[index], heap[parent], dongle))
		{
			swap(&heap[index], &heap[parent]);
			index = parent;
		}
		else
			break ;
	}
}

void	fifo_implementation(t_dongle *dongle, t_coder *add_coder)
{
	int	size;

	if (dongle->ready_coder[0] == NULL)
		size = 0;
	else if (dongle->ready_coder[1] == NULL)
		size = 1;
	else
		return ;
	dongle->ready_coder[size] = add_coder;
	heapify_up(dongle->ready_coder, size, dongle);
}