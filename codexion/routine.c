/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 13:04:56 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/06 15:28:56 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	now;

	pthread_mutex_lock(&dongle->dongle_mutex);
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (dongle->dongle_status != AVAILABLE
		|| timespec_diff(dongle->release_time, now)
		> coder->data->parsed_data.cooldown)
	{
		add_to_queue(&dongle->pq, coder);
		while (dongle->dongle_status != AVAILABLE
			|| peek(&dongle->pq) != coder->coder_id)
			pthread_cond_wait(&dongle->dongle_cond, &dongle->dongle_mutex);
		dequeue(&dongle->pq);
	}
	dongle->dongle_status = TAKEN;
	pthread_mutex_unlock(&dongle->dongle_mutex);
	return ;
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_mutex);
	dongle->dongle_status = AVAILABLE;
	clock_gettime(CLOCK_MONOTONIC, &dongle->release_time);
	pthread_mutex_unlock(&dongle->dongle_mutex);
	pthread_cond_broadcast(&dongle->dongle_cond);
}

void	*routine(void *coder_p)
{
	t_coder	*coder;

	coder = (t_coder *) coder_p;
	while (!coder->data->simulation_stop)
	{
		check_simulation(coder->data);
		if (coder->data->simulation_stop)
			return (NULL);
		compile(coder);
		debug_and_refact(coder);
		check_simulation(coder->data);
	}
	return (NULL);
}

void	start(t_data *data)
{
	init_dongle_mutex(data);
	init_dongle_cond(data);
	pthread_mutex_init(&data->log_mutex, NULL);
	pthread_mutex_init(&data->global_mutex, NULL);
	init_coder_threads(data);
	destroy_dongle_mutex(data);
	destroy_dongle_cond(data);
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->global_mutex);
}
