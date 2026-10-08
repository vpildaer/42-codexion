/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:24:31 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/06 15:26:16 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_coders(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders - 1)
	{
		data->all_coders[i].coder_id = i + 1;
		data->all_coders[i].left_dongle = &(data->all_dongles[i]);
		data->all_coders[i].right_dongle = &(data->all_dongles[i + 1]);
		data->all_coders[i].last_compile_start = data->start_time;
		data->all_coders[i].compiles_count = 0;
		data->all_coders[i].coder_status = WAITING;
		data->all_coders[i].data = data;
		i++;
	}
	data->all_coders[i].coder_id = i + 1;
	data->all_coders[i].left_dongle = &(data->all_dongles[i]);
	data->all_coders[i].right_dongle = &(data->all_dongles[0]);
	data->all_coders[i].last_compile_start = data->start_time;
	data->all_coders[i].compiles_count = 0;
	data->all_coders[i].coder_status = WAITING;
	data->all_coders[i].data = data;
}

void	init_dongles(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		data->all_dongles[i].dongle_status = AVAILABLE;
		clock_gettime(CLOCK_MONOTONIC, &(data->all_dongles[i].release_time));
		init_heap(&(data->all_dongles[i].pq), data->parsed_data);
		i++;
	}
}

void	check_simulation(t_data *data)
{
	int				i;
	int				done_compiling;
	struct timespec	now;

	i = 0;
	done_compiling = 0;
	while (i < data->parsed_data.nb_coders)
	{
		if (data->all_coders[i].compiles_count == data->parsed_data.nb_compiles)
			done_compiling++;
		clock_gettime(CLOCK_MONOTONIC, &now);
		if (timespec_diff(data->all_coders[i].last_compile_start, now)
			>= data->parsed_data.burnout)
		{
			data->simulation_stop = 1;
			pthread_mutex_lock(&data->log_mutex);
			printf("%ld %d burned out", get_time(data->start_time), i + 1);
			pthread_mutex_unlock(&data->log_mutex);
			return ;
		}
		i++;
	}
	if (done_compiling == data->parsed_data.nb_coders)
		data->simulation_stop = 1;
	return ;
}

void	init_simulation(t_data *data, t_pars parsed_data)
{
	data->simulation_stop = 0;
	data->all_dongles = malloc(sizeof(t_dongle) * (parsed_data.nb_coders));
	if (data->all_dongles == NULL)
	{
		data->simulation_stop = 1;
		return ;
	}
	data->all_coders = malloc(sizeof(t_coder) * (parsed_data.nb_coders));
	if (data->all_coders == NULL)
	{
		free(data->all_dongles);
		data->simulation_stop = 1;
		return ;
	}
	data->parsed_data = parsed_data;
	clock_gettime(CLOCK_MONOTONIC, &(data->start_time));
	init_coders(data);
	init_dongles(data);
	return ;
}
