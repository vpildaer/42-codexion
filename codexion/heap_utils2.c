/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 13:48:11 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/02 14:56:45 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    swap(t_request *a, t_request *b)
{
    t_request   temp;
    
    temp = *a;
    *a = *b;
    *b = temp;
}

void    heapify(t_heap *pq, int index)
{
    if (index && pq->count == 2 &&
        pq->compare(&pq->requests[(index - 1) / 2], &pq->requests[index]) < 0)
            swap(&pq->requests[(index - 1) / 2], &pq->requests[index]);
}

void    enqueue(t_heap *pq, t_request req)
{
    if (pq->count == 2)
        return ;
    pq->requests[pq->count++] = req;
    heapify(pq, pq->count - 1);
}

int dequeue(t_heap *pq)
{
    int coder_id;
    
    if (!pq->count)
        return (-1);
    coder_id = pq->requests[0].coder_id;
    pq->requests[0] = pq->requests[--(pq->count)];
    return (coder_id);
}

int peek(t_heap *pq)
{
    if (!pq->count)
        return (-1);
    return (pq->requests[0].coder_id);
}