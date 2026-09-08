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



/*
* start working on it using heap algo
*/

// static void push(t_coder *ready_queue, t_coder *add_coder, int capacity, int *index)
// {
//     if (ready_queue[(*index - 1)/2])
//     {
//         heapify()
//     }

// }

// static t_coder *heapify()


// void	fifo_implementation(t_coder *coder_ready_queue, t_coder *add_coder, int capacity)
// {
//     int index;

//     index = 0;
//     push(coder_ready_queue, add_coder, capacity, &index)
// }



void	fifo_implementation(t_dongle *dongle, t_coder *coder)
{
	t_coder	*tmp;

	if (!dongle->ready_coder[0] && !dongle->ready_coder[1])
	{
		dongle->ready_coder[0] = coder;
	}
	else if (dongle->ready_coder[0] && !dongle->ready_coder[1])
	{
		if (coder->start < dongle->ready_coder[0]->start
			|| (coder->start == dongle->ready_coder[0]->start
				&& coder->id < dongle->ready_coder[0]->id))
		{
			tmp = dongle->ready_coder[0];
			dongle->ready_coder[0] = coder;
			dongle->ready_coder[1] = tmp;
		}
		else
		{
			dongle->ready_coder[1] = coder;
		}
	}
}
