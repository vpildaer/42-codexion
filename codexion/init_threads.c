/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_threads.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 12:25:35 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/29 17:18:32 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    init_coder_threads(t_data *data)
{
    int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		pthread_create(&(data->all_coders[i].coder_thread), NULL, routine,
				 (void*) &(data->all_coders[i]));
		i++;
	}
	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		pthread_join(data->all_coders[i].coder_thread, NULL);
		i++;
	}
}

void	init_dongle_mutex(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		pthread_mutex_init(&(data->all_dongles[i].dongle_mutex), NULL);
		i++;
	}
}

void	destroy_dongle_mutex(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		pthread_mutex_destroy(&(data->all_dongles[i].dongle_mutex));
		i++;
	}
}

void	init_dongle_cond(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		pthread_cond_init(&(data->all_dongles[i].dongle_cond), NULL);
		i++;
	}
}

void	destroy_dongle_cond(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		pthread_cond_destroy(&(data->all_dongles[i].dongle_cond));
		i++;	
	}
}