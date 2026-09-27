/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/27 13:03:04 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/27 14:12:44 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_heap(t_heap *pq, t_pars parsed_data)
{
	pq->requests = malloc(sizeof(t_request) * 2);
	if (pq->requests == NULL)
		return (-1);
	pq->count = 0;
	if (parsed_data.scheduler == 1)
		pq->compare = &fifo_compare;
	else if (parsed_data.scheduler == 2)
		pq->compare = &edf_compare;
	return (0);
}
