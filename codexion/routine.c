/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 13:04:56 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/02 17:00:45 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_mutex);
	if (dongle->dongle_status != AVAILABLE)
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
	usleep(coder->data->parsed_data.cooldown);
	pthread_cond_broadcast(&dongle->dongle_cond);
	pthread_mutex_unlock(&dongle->dongle_mutex);
}

void	*routine(void *data_void)
{
	t_data	*data;
	int		i;
	//long	runtime;

	data = (t_data *) data_void;
	i = 0;
	while (!data->simulation_stop)
	{
		if (i == data->parsed_data.nb_coders)
			i = 0;
		check_simulation(data);
		if (data->simulation_stop)
			return (NULL);
		compile(&data->all_coders[i]);
		debug_and_refact(&data->all_coders[i]);
		check_simulation(data);
		i++;
	}
	return (NULL);
}

void	start(t_data *data)
{
	init_dongle_mutex(data);
	init_dongle_cond(data);
	init_coder_threads(data);
	destroy_dongle_mutex(data);
	destroy_dongle_cond(data);
}