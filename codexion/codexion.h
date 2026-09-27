/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:42:26 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/27 15:41:32 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H

# define CODEXION_H

# include <unistd.h>

# include <stdlib.h>

# include <stdio.h>

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
	struct timespec	deadline;
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
	int				compiles_count;
	t_coder_stat	coder_status;
	pthread_mutex_t	coder_mutex;
}				t_coder;

typedef struct s_data
{
	struct timespec	start_time;
	int				simulation_stop;
	pthread_mutex_t	global_mutex;
	pthread_mutex_t	log_mutex;
	t_dongle		*all_dongles;
	t_coder			*all_coders;
	t_pars			parsed_data;
}				t_data;

void	error(int parser_res);

int		parser(t_pars *parsed_data, char **argv);

int		parse_int(char *arg);

int		parse_sched(char *arg);

int		store_data(t_pars *parsed_data, int temp[]);

int		init_simulation(t_data *data, t_pars parsed_data);

void	init_coders(t_data *data);

int		init_heap(t_heap *pq, t_pars parsed_data);

int		timespec_cmp(struct timespec a, struct timespec b);

int		fifo_compare(t_request *a, t_request *b);

int		edf_compare(t_request *a, t_request *b);

#endif
