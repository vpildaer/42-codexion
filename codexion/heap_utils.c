/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/27 13:03:22 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/29 16:42:37 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	timespec_diff(struct timespec a, struct timespec b)
{
	long	diff;

	diff = (b.tv_sec - a.tv_sec) * 1e9;
	diff = diff + (b.tv_nsec - a.tv_nsec);
	return (diff);
}

int	fifo_compare(t_request *a, t_request *b)
{
	long	diff;
	
	diff = timespec_diff(a->arrival_time, b->arrival_time);
	if (diff < 0)
		return (-1);
	else if (diff > 0)
		return (1);
	return (0);
}

int	edf_compare(t_request *a, t_request *b)
{
	long	diff;
	diff = b->deadline - a->deadline;
	if (diff < 0)
		return (-1);
	else if (diff > 0)
		return (1);
	return (0);
}

long	calc_deadline(t_coder *coder)
{
	long	res;
	
	res = timespec_diff(coder->data->start_time, coder->last_compile_start);
	res = res / 1e6;
	res = res + coder->data->parsed_data.burnout;
	return (res);
}

struct timespec	timespec_add(struct timespec a, struct timespec b)
{
	struct timespec	res;

	res.tv_nsec = a.tv_nsec + b.tv_nsec;
	res.tv_sec = a.tv_sec + b.tv_sec;
	return (res);
}