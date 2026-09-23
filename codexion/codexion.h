/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:42:26 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/23 16:36:15 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H

# define CODEXION_H

# include <unistd.h>

# include <stdlib.h>

# include <stdio.h>

# include <string.h>

# include <pthread.h>

#include <time.h>

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
}				t_dongle;

typedef enum e_coder_stat
{
	COMPILING,
	DEBUGGING,
	REFACTORING,
	WAITING
}				t_coder_stat;

typedef struct s_coder
{
	int				num_coder;
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
	t_dongle		*all_dongles;
	t_coder			*all_coders;
	t_pars			parsed_data;
}				t_data;

void	error(int parser_res);

int		parser(t_pars *parsed_data, char **argv);

int		parse_int(char *arg);

int		parse_sched(char *arg);

int		store_data(t_pars *parsed_data, int temp[]);

#endif
