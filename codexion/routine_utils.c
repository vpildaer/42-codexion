/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 13:10:33 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/08 16:06:16 by vpildaer         ###   ########.fr       */
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
	if (coder->data->simulation_stop)
	{
		pthread_mutex_unlock(&(coder->data->log_mutex));
		return ;
	}
	if (coder->coder_status == COMPILING)
		printf("%ld %d is compiling\n",
			get_time(coder->data->start_time), coder->coder_id);
	else if (coder->coder_status == DEBUGGING)
		printf("%ld %d is debugging\n",
			get_time(coder->data->start_time), coder->coder_id);
	else if (coder->coder_status == REFACTORING)
		printf("%ld %d is refactoring\n",
			get_time(coder->data->start_time), coder->coder_id);
	pthread_mutex_unlock(&(coder->data->log_mutex));
}

void	compile(t_coder *coder)
{
	int	l_res;
	int	r_res;

	if (coder->left_dongle == coder->right_dongle)
		return ;
	if (coder->left_dongle < coder->right_dongle)
	{
		l_res = take_dongle(coder, coder->left_dongle);
		r_res = take_dongle(coder, coder->right_dongle);
	}
	else
	{
		r_res = take_dongle(coder, coder->right_dongle);
		l_res = take_dongle(coder, coder->left_dongle);
	}
	finish_compile(coder, l_res, r_res);
}

void	finish_compile(t_coder *coder, int l_res, int r_res)
{
	if (l_res && r_res)
	{
		pthread_mutex_lock(&coder->coder_mutex);
		coder->coder_status = COMPILING;
		clock_gettime(CLOCK_MONOTONIC, &coder->last_compile_start);
		output_coder(coder);
		pthread_mutex_unlock(&coder->coder_mutex);
		usleep(coder->data->parsed_data.compile * 1000);
	}
	if (coder->left_dongle < coder->right_dongle)
	{
		if (l_res)
			release_dongle(coder, coder->left_dongle);
		if (r_res)
			release_dongle(coder, coder->right_dongle);
	}
	else
	{
		if (r_res)
			release_dongle(coder, coder->right_dongle);
		if (l_res)
			release_dongle(coder, coder->left_dongle);
	}
}

void	debug_and_refact(t_coder *coder)
{
	if (coder->data->simulation_stop)
		return ;
	pthread_mutex_lock(&coder->coder_mutex);
	coder->coder_status = DEBUGGING;
	output_coder(coder);
	pthread_mutex_unlock(&coder->coder_mutex);
	usleep(coder->data->parsed_data.debug * 1000);
	pthread_mutex_lock(&coder->coder_mutex);
	coder->coder_status = REFACTORING;
	output_coder(coder);
	pthread_mutex_unlock(&coder->coder_mutex);
	usleep(coder->data->parsed_data.refactor * 1000);
}
