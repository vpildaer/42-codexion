/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 16:03:59 by vpildaer          #+#    #+#             */
/*   Updated: 2026/09/16 18:45:32 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_int(char *arg)
{
	int	i;
	int	res;

	if (arg[0] == '-' || (arg[0] > '2' && strlen(arg) >= 10) || arg == NULL)
		return (-1);
	i = 0;
	res = 0;
  while(arg[i])
  	{
		if !(arg[i] >= '0' && arg[i] <= '9')
	  	return (-1);
		res = res * 10 + (arg[i] - '0');
		i++;
  	}
  	return (res);
}

int parse_sched(char *arg)
{
  	int i;
  	char* fifo;
 	char* edf;

  	i = 0;
  	fifo = "fifo";
  	edf = "edf";
  	if (arg[0] == 'f' && strlen(arg) == 4)
  	{
		while (arg[i] == fifo[i])
	  		i++;
		if (i == 4)
	  		return (1);
  	}
  	else if (arg[0] == 'e' && strlen(arg) == 3)
  	{
		while(arg[i] == edf[i])
	  		i++;
		if (i == 3)
	  		return (2);
  	}
  	return (-1);
}

void  store_data(t_pars *parsed_data, int temp[])
{
  	parsed_data->nb_coders = temp[0];
  	parsed_data->burnout = temp[1];
  	parsed_data->compile = temp[2];
  	parsed_data->debug = temp[3];
  	parsed_data->refactor = temp[4];
  	parsed_data->nb_compiles = temp[5];
  	parsed_data->cooldown = temp[6];
	parsed_data->scheduler = temp[7]
}

int parser(t_pars *parsed_data, char **argv)
{
  	int temp[8];
  	int i;

  	temp[0] = parse_int(argv[1]);
  	temp[1] = parse_int(argv[2]);
  	temp[2] = parse_int(argv[3]);
  	temp[3] = parse_int(argv[4]);
  	temp[4] = parse_int(argv[5]);
  	temp[5] = parse_int(argv[6]);
  	temp[6] = parse_int(argv[7]);
  	temp[7] = parse_int(argv[8]);
  	i = 0;
  	while (i < 7)
  	{
		if (temp[i] < 0)
	  		return (1);
		i++;
  	}
  	if (temp[7] < 0)
		return (2);
  	store_data(parsed_data, temp);
  	return (0);
}
