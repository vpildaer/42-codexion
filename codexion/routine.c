/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 13:04:56 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/08 16:06:01 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_dongle(t_coder *coder, t_dongle *dongle, struct timespec now)
{
	struct timespec	wait;

	add_to_queue(&dongle->pq, coder);
	while ((dongle->dongle_status != AVAILABLE
			|| peek(&dongle->pq) != coder->coder_id
			|| (timespec_diff(dongle->release_time, now)
				< coder->data->parsed_data.cooldown))
		&& !coder->data->simulation_stop)
	{
		wait = get_condwait(dongle->release_time,
				coder->data->parsed_data.cooldown);
		pthread_cond_timedwait(&dongle->dongle_cond, &dongle->dongle_mutex,
			&wait);
		clock_gettime(CLOCK_MONOTONIC, &now);
	}
	if (peek(&dongle->pq) == coder->coder_id)
		dequeue(&dongle->pq);
}

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	now;

	pthread_mutex_lock(&dongle->dongle_mutex);
	clock_gettime(CLOCK_MONOTONIC, &now);
	if ((dongle->dongle_status != AVAILABLE
			|| timespec_diff(dongle->release_time, now)
			< coder->data->parsed_data.cooldown)
		&& !coder->data->simulation_stop)
		wait_dongle(coder, dongle, now);
	if (coder->data->simulation_stop)
	{
		pthread_mutex_unlock(&dongle->dongle_mutex);
		return (0);
	}
	dongle->dongle_status = TAKEN;
	pthread_mutex_unlock(&dongle->dongle_mutex);
	return (1);
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
		if (coder->data->simulation_stop
			|| coder->data->parsed_data.nb_coders == 1
			|| coder->compiles_count >= coder->data->parsed_data.nb_compiles)
			return (NULL);
		compile(coder);
		if (!coder->data->simulation_stop)
		{
			coder->compiles_count++;
			debug_and_refact(coder);
		}
	}
	return (NULL);
}

void	start(t_data *data)
{
	init_dongle_mutex(data);
	init_dongle_cond(data);
	pthread_mutex_init(&data->log_mutex, NULL);
	pthread_create(&data->monitor, NULL, monitor_routine, (void *) data);
	init_coder_threads(data);
	pthread_join(data->monitor, NULL);
	destroy_dongle_mutex_and_cond(data);
	destroy_coder_mutex(data);
	pthread_mutex_destroy(&data->log_mutex);
}
