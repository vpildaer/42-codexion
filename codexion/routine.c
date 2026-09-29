/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 13:04:56 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/29 15:42:19 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*routine(void *coder)
{
	t_coder	*coder_p;
	//long	runtime;

	coder_p = (t_coder*) coder;
	compile(coder_p);
	// pthread_mutex_lock(&(coder_p->data->all_dongles[0].dongle_mutex));
	// runtime = get_time(coder_p->data->start_time);
	// printf("%ld routine for coder %d\n", runtime, coder_p->coder_id);
	// usleep(100000);
	// pthread_mutex_unlock(&(coder_p->data->all_dongles[0].dongle_mutex));
	return (NULL);
}

void	start(t_data *data)
{
	init_dongle_mutex(data);
	init_coder_threads(data);
	destroy_dongle_mutex(data);
}
