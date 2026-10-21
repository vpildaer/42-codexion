/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 13:25:29 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/08 17:53:13 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

struct timespec	get_condwait(struct timespec release, int cooldown)
{
	struct timespec	res;
	long			total_nsec;

	total_nsec = release.tv_nsec + ((long)cooldown * 1000000L);
	res.tv_sec = release.tv_sec + (total_nsec / 1000000000L);
	res.tv_nsec = total_nsec % 1000000000L;
	return (res);
}

void	monitor(t_data *data, int i, int *done_compiling)
{
	struct timespec	now;

	if (data->all_coders[i].compiles_count >= data->parsed_data.nb_compiles)
		(*done_compiling)++;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if ((timespec_diff(data->all_coders[i].last_compile_start, now)
			>= data->parsed_data.burnout
			&& data->all_coders[i].compiles_count > 0)
		|| (timespec_diff(data->start_time, now) >= data->parsed_data.burnout
			&& data->all_coders[i].compiles_count == 0))
	{
		data->simulation_stop = 1;
		pthread_mutex_lock(&data->log_mutex);
		printf("%ld %d burned out\n", get_time(data->start_time), i + 1);
		pthread_mutex_unlock(&data->log_mutex);
		pthread_mutex_unlock(&data->all_coders[i].coder_mutex);
		return ;
	}
}

void	*monitor_routine(void *data_p)
{
	int				i;
	int				done_compiling;
	t_data			*data;

	data = (t_data *) data_p;
	while (!data->simulation_stop)
	{
		i = -1;
		done_compiling = 0;
		while (++i < data->parsed_data.nb_coders)
		{
			pthread_mutex_lock(&data->all_coders[i].coder_mutex);
			monitor(data, i, &done_compiling);
			if (data->simulation_stop == 1)
				return (NULL);
			pthread_mutex_unlock(&data->all_coders[i].coder_mutex);
		}
		if (done_compiling == data->parsed_data.nb_coders)
		{
			data->simulation_stop = 1;
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
