/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:24:31 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/27 15:41:30 by vpildaer         ###   ########.fr       */
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
		data->all_coders[i].compiles_count = 0;
		data->all_coders[i].coder_status = 0;
		i++;
	}
	data->all_coders[i].coder_id = i + 1;
	data->all_coders[i].left_dongle = &(data->all_dongles[i]);
	data->all_coders[i].right_dongle = &(data->all_dongles[0]);
	data->all_coders[i].compiles_count = 0;
	data->all_coders[i].coder_status = 0;
}

int	init_simulation(t_data *data, t_pars parsed_data)
{
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &(data->start_time));
	data->simulation_stop = 0;
	data->all_dongles = malloc(sizeof(t_dongle) * (parsed_data.nb_coders));
	if (data->all_dongles == NULL)
		return (-1);
	data->all_coders = malloc(sizeof(t_coder) * (parsed_data.nb_coders));
	if (data->all_coders == NULL)
	{
		free(data->all_dongles);
		return (-1);
	}
	data->parsed_data = parsed_data;
	init_coders(data);
}
