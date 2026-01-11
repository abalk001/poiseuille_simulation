/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abalk <abalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 12:00:00 by abalk             #+#    #+#             */
/*   Updated: 2026/01/10 12:00:00 by abalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shear.h"

/*
** Prints simulation summary with key parameters.
*/
static void	print_summary(t_vesicle *v, t_params *p)
{
	printf("=== Simulation Parameters ===\n");
	printf("Channel width:   %.1f um\n", p->channel_width * 1e6);
	printf("Vesicle radius:  %.1f um\n", v->radius * 1e6);
	printf("Shear rate:      %.1f s^-1\n", p->shear_rate);
	printf("Viscosity ratio: %.2f\n", v->lambda);
	printf("Reduced volume:  %.2f\n", v->nu);
	printf("Time step:       %.4f s\n", p->dt);
	printf("Total time:      %.1f s\n\n", p->total_time);
}

/*
** Main entry point for simple shear flow simulation.
** Usage: ./simple_shear [config_file]
** If no config file is provided, uses default parameters.
*/
int	main(int ac, char **av)
{
	t_vesicle	vesicle;
	t_params	params;

	print_header();
	init_vesicle(&vesicle);
	init_params(&params);
	if (ac > 1)
		load_config(av[1], &vesicle, &params);
	if (!validate_parameters(&vesicle, &params))
		return (1);
	print_summary(&vesicle, &params);
	printf("=== Running Simulation ===\n");
	run_simulation(&vesicle, &params, "simple_shear_trajectory.csv");
	validate_scaling_law(&vesicle, &params);
	generate_validation_data(&vesicle, &params, "validation_input.csv");
	printf("=== Simulation Complete ===\n\n");
	return (0);
}
