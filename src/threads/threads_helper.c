/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+      :+:    :+:     */
/*                                                    +:+ +:+         +:+     */
/*   By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 22:04:50 by aelhadja          #+#    #+#             */
/*   Updated: 2026/09/09 22:04:57 by aelhadja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head.h"

void	coder_finished(t_coder *coder)
{
	pthread_mutex_lock(&coder->right->dongle_mutex);
	coder->finished = 1;
	pthread_mutex_unlock(&coder->right->dongle_mutex);
	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	coder->monitor->finish_running -= 1;
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
}
