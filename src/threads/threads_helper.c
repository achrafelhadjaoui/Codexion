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

/*
	* A coder that has done all the compiles it was asked for stops
	* requesting dongles for good, so the clock since its last compile
	* keeps growing and would eventually cross time_to_burnout. It is not
	* burning out, it is simply done, and reporting it would end a run
	* that the subject says must stop only once EVERY coder is done.
	*
	* The flag is written under the right dongle mutex because that is the
	* lock the monitor already holds while reading last_compile, so both
	* fields are read as one consistent pair.
	*
	* No signal to send here: the monitor is watching the clock anyway, so
	* it reads this counter on its next pass rather than being woken for
	* it. A condition variable nobody is ever blocked on would only look
	* like synchronisation.
	*/
void	coder_finished(t_coder *coder)
{
	pthread_mutex_lock(&coder->right->dongle_mutex);
	coder->finished = 1;
	pthread_mutex_unlock(&coder->right->dongle_mutex);
	pthread_mutex_lock(&coder->monitor->monitor_mutex);
	coder->monitor->finish_running -= 1;
	pthread_mutex_unlock(&coder->monitor->monitor_mutex);
}
