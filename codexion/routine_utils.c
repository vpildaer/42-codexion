/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 13:10:33 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/29 17:22:10 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time(struct timespec start)
{
	long			res;
	struct timespec	time;

	clock_gettime(CLOCK_MONOTONIC, &time);
	res = (time.tv_sec - start.tv_sec) * 1e9;
	res = res + (time.tv_nsec - start.tv_nsec);
	res = res / 1e6;
	return (res);
}

void	output_coder(t_coder *coder)
{
	pthread_mutex_lock(&(coder->data->log_mutex));
	if (coder->coder_status == COMPILING)
		printf("%ld %d is compiling\n", get_time(coder->data->start_time), coder->coder_id);
	else if (coder->coder_status == DEBUGGING)
		printf("%ld %d is debugging\n", get_time(coder->data->start_time), coder->coder_id);
	else if (coder->coder_status == REFACTORING)
		printf("%ld %d is refactoring\n", get_time(coder->data->start_time), coder->coder_id);
	pthread_mutex_unlock(&(coder->data->log_mutex));
}

void	compile(t_coder *coder)
{
	coder->left_dongle->dongle_status = TAKEN;
	coder->coder_status = COMPILING;
	pthread_mutex_lock(&(coder->coder_mutex));
	pthread_mutex_lock(&(coder->left_dongle->dongle_mutex));
	pthread_mutex_lock(&(coder->right_dongle->dongle_mutex));
	output_coder(coder);
	usleep(coder->data->parsed_data.compile * 1000);
	pthread_mutex_unlock(&(coder->left_dongle->dongle_mutex));
	pthread_mutex_unlock(&(coder->right_dongle->dongle_mutex));
	pthread_mutex_unlock(&(coder->coder_mutex));
	coder->left_dongle->dongle_status = AVAILABLE;
}

void	wait(t_coder *coder)
{
	add_to_queue(&(coder->left_dongle->pq), coder);
	add_to_queue(&(coder->right_dongle->pq), coder);
	coder->coder_status = WAITING;
	pthread_cond_wait(&(coder->left_dongle->dongle_cond), &(coder->left_dongle->dongle_mutex));
	pthread_cond_wait(&(coder->right_dongle->dongle_cond), &(coder->right_dongle->dongle_mutex));
}