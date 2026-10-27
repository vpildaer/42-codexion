/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 16:10:18 by vpildaer          #+#    #+#             */
/*   Updated: 2026/10/27 17:08:36 by vpildaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	error(int parser_res)
{
	if (parser_res == 1)
		printf("Input Error: negative/non-integers numbers are not allowed\n");
	else if (parser_res == 2)
		printf("Input Error: scheduler has to either be fifo or edf\n");
	else if (parser_res == 3)
		printf("Input Error: number_of_coders must be >= 1\n");
	else if (parser_res == 4)
		printf("Input Error: number_of_compiles_required must be >= 1\n");
	else if (parser_res == 5)
		printf("Input Error: time_to_compile must be greater or equal to 1\n");
	else if (parser_res == 6)
		printf("Input Error: time_to_burnout must be greater or equal to 1\n");
	else if (parser_res == 7)
		printf("Input Error: simulation will always fail with given times\n");
	else
		printf("Input Error\n");
}

void	ft_free(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->parsed_data.nb_coders)
	{
		free(data->all_dongles[i].pq.requests);
		i++;
	}
	free(data->all_coders);
	free(data->all_dongles);
}

int	main(int argc, char **argv)
{
	int		parser_res;
	t_pars	parsed_data;
	t_data	data;

	if (argc != 9)
	{
		printf("Input Error: 8 arguments required\n");
		return (0);
	}
	memset(&parsed_data, 0, sizeof(t_pars));
	parser_res = parser(&parsed_data, argv);
	if (parser_res != 0)
	{
		error(parser_res);
		return (0);
	}
	init_simulation(&data, parsed_data);
	if (data.simulation_stop != 1)
	{
		start(&data);
		ft_free(&data);
	}
	return (0);
}
