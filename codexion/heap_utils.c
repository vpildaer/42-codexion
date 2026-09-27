/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/27 13:03:22 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/27 14:05:39 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	timespec_cmp(struct timespec a, struct timespec b)
{
	double	diff;

	diff = (b.tv_sec - a.tv_sec) * 1e9;
	diff = diff + (b.tv_nsec - a.tv_nsec);
	if (diff < 0)
		return (-1);
	else if (diff > 0)
		return (1);
	return (0);
}

int	fifo_compare(t_request *a, t_request *b)
{
	return (timespec_cmp(a->arrival_time, b->arrival_time));
}

int	edf_compare(t_request *a, t_request *b)
{
	return (timespec_cmp(a->deadline, b->deadline));
}
