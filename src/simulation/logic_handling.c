/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logic_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 03:45:19 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/06 03:45:24 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

/*
	* Arrival order, which is the whole of the fifo policy and also what
	* settles a tie under edf.
	*
	* Coders that really did arrive together need a rule, and the obvious
	* one - lowest id wins - is the worst possible one here. It orders the
	* ring into a single chain c1 < c2 < ... < cN, and a chain around a
	* ring has exactly ONE coder that beats both of its neighbours, so the
	* coders get served strictly one at a time.
	*
	* Even ids are never neighbours, so letting them win a genuine tie
	* hands out a whole set of coders that can compile at once instead of
	* just one. It is still completely deterministic, which is all the
	* tie-break is asked to be.
	*/
static int	arrived_before(t_coder *coder1, t_coder *coder2)
{
	if (coder1->start != coder2->start)
		return (coder1->start < coder2->start);
	if (coder1->id % 2 != coder2->id % 2)
		return (coder1->id % 2 == 0);
	return (coder1->id < coder2->id);
}

/*
	* Equal deadlines are not the rare curiosity they look like: a coder
	* that compiled at the very start still carries the deadline it was
	* born with, so it ties with every coder that has not compiled at all.
	*
	* Settling those ties on the id alone would always favour the same low
	* numbers, and the coder it keeps beating never gets a turn. Falling
	* back on arrival order first keeps the policy deterministic without
	* handing the same coder a permanent advantage.
	*/
static int	coder_before(t_coder *coder1, t_coder *coder2, t_dongle *dongle)
{
	long	deadline1;
	long	deadline2;

	if (strcmp(dongle->policy, "edf") != 0)
		return (arrived_before(coder1, coder2));
	deadline1 = coder1->last_compile + coder1->time_to_burnout;
	deadline2 = coder2->last_compile + coder2->time_to_burnout;
	if (deadline1 < deadline2)
		return (1);
	if (deadline1 > deadline2)
		return (0);
	return (arrived_before(coder1, coder2));
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

void	logic_implementation(t_dongle *dongle, t_coder *add_coder)
{
	int	size;

	if (dongle->ready_coder[0] == add_coder
		|| dongle->ready_coder[1] == add_coder)
		return ;
	if (dongle->ready_coder[0] == NULL)
		size = 0;
	else if (dongle->ready_coder[1] == NULL)
		size = 1;
	else
		return ;
	dongle->ready_coder[size] = add_coder;
	heapify_up(dongle->ready_coder, size, dongle);
}
