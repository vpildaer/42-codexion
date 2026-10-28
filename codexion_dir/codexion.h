/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:42:26 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/28 17:18:01 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H

# define CODEXION_H

# include <unistd.h>

# include <stdlib.h>

# include <stdio.h>

# include <stdatomic.h>

# include <string.h>

# include <pthread.h>

# include <time.h>

typedef struct s_pars
{
	int	nb_coders;
	int	burnout;
	int	compile;
	int	debug;
	int	refactor;
	int	nb_compiles;
	int	cooldown;
	int	scheduler;
}				t_pars;

typedef struct s_request
{
	int				coder_id;
	struct timespec	arrival_time;
	long			deadline;
}				t_request;

typedef struct s_heap
{
	t_request	*requests;
	int			count;
	int			(*compare)(t_request *a, t_request *b);
}				t_heap;

typedef enum e_dong_stat
{
	AVAILABLE,
	TAKEN
}				t_dong_stat;

typedef struct s_dongle
{
	t_dong_stat		dongle_status;
	struct timespec	release_time;
	pthread_mutex_t	dongle_mutex;
	pthread_cond_t	dongle_cond;
	t_heap			pq;
}				t_dongle;

typedef enum e_coder_stat
{
	WAITING,
	COMPILING,
	DEBUGGING,
	REFACTORING
}				t_coder_stat;

typedef struct s_coder
{
	int				coder_id;
	pthread_t		coder_thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	struct timespec	last_compile_start;
	atomic_int		compiles_count;
	t_coder_stat	coder_status;
	pthread_mutex_t	coder_mutex;
	struct s_data	*data;
}				t_coder;

typedef struct s_data
{
	struct timespec	start_time;
	atomic_int		simulation_stop;
	pthread_mutex_t	log_mutex;
	pthread_t		monitor;
	t_dongle		*all_dongles;
	t_coder			*all_coders;
	t_pars			parsed_data;
}				t_data;

void			error(int parser_res);

int				parser(t_pars *parsed_data, char **argv);

int				parse_int(char *arg);

int				parse_sched(char *arg);

int				store_data(t_pars *parsed_data, int temp[]);

void			init_simulation(t_data *data, t_pars parsed_data);

void			init_coders(t_data *data);

int				init_heap(t_heap *pq, t_pars parsed_data);

long			timespec_diff(struct timespec a, struct timespec b);

struct timespec	init_time(t_data *data);

struct timespec	timespec_add(struct timespec a, struct timespec b);

struct timespec	get_condwait(struct timespec release, int cooldown);

long			calc_deadline(t_coder *coder);

int				fifo_compare(t_request *a, t_request *b);

int				edf_compare(t_request *a, t_request *b);

void			swap(t_request *a, t_request *b);

void			heapify(t_heap *pq, int index);

void			enqueue(t_heap *pq, t_request req);

int				dequeue(t_heap *pq);

int				peek(t_heap *pq);

void			*routine(void *coder);

void			start(t_data *data);

void			init_coder_threads(t_data *data);

void			init_coder_mutex(t_data *data);

void			init_dongle_mutex_and_cond(t_data *data);

void			destroy_dongle_mutex_and_cond(t_data *data);

void			destroy_coder_mutex(t_data *data);

long			get_time(struct timespec start);

void			compile(t_coder *coder);

void			finish_compile(t_coder *coder, int l_res, int r_res);

void			debug_and_refact(t_coder *coder);

void			add_to_queue(t_heap *pq, t_coder *coder);

void			init_dongle_cond(t_data *data);

int				take_dongle(t_coder *coder, t_dongle *dongle);

void			wait_dongle(t_coder *coder, t_dongle *dongle,
					struct timespec now);

void			release_dongle(t_dongle *dongle);

void			ft_free(t_data *data);

void			*monitor_routine(void *data);

void			monitor(t_data *data, int i, int *done_compiling);

void			requeue(t_coder *coder);

void			single_coder_case(t_coder *coder);

#endif