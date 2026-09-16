/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:42:26 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/16 18:27:39 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H

# define CODEXION_H

# include <unistd.h>

# include <stdlib.h>

# include <stdio.h>

# include <string.h>

# include <pthread.h>

typedef struct s_pars
{
  int nb_coders;
  int burnout;
  int compile;
  int debug;
  int refactor;
  int nb_compiles;
  int cooldown;
  int scheduler;
}              t_pars;

void  error(int parser_res);

#endif
