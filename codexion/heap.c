/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/27 13:03:04 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/06 14:42:13 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_heap(t_heap *pq, t_pars parsed_data)
{
	pq->requests = malloc(sizeof(t_request) * 2);
	if (pq->requests == NULL)
		return (-1);
	memset(pq->requests, 0, sizeof(t_request));
	pq->count = 0;
	if (parsed_data.scheduler == 1)
		pq->compare = &fifo_compare;
	else if (parsed_data.scheduler == 2)
		pq->compare = &edf_compare;
	return (0);
}

void	add_to_queue(t_heap *pq, t_coder *coder)
{
	t_request	req;

	req.coder_id = coder->coder_id;
	clock_gettime(CLOCK_MONOTONIC, &(req.arrival_time));
	req.deadline = calc_deadline(coder);
	enqueue(pq, req);
}
