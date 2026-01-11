#include "poiseuille.h"

/*
** Prints simulation summary with key parameters.
*/
static void	print_summary(t_vesicle *v, t_params *p)
{
	double	gamma_wall;

	gamma_wall = 4.0 * p->u_max / p->channel_width;
	printf("=== Simulation Parameters ===\n");
	printf("Channel width:     %.1f um\n", p->channel_width * 1e6);
	printf("Vesicle radius:    %.1f um\n", v->radius * 1e6);
	printf("Max flow velocity: %.1f um/s\n", p->u_max * 1e6);
	printf("Wall shear rate:   %.1f s^-1\n", gamma_wall);
	printf("Viscosity ratio:   %.2f\n", v->lambda);
	printf("Reduced volume:    %.2f\n", v->nu);
	printf("Poiseuille exp:    %.2f\n", p->delta);
	printf("Time step:         %.4f s\n", p->dt);
	printf("Total time:        %.1f s\n\n", p->total_time);
}

/*
** Main entry point for Poiseuille flow simulation.
** Usage: ./poiseuille [config_file]
** If no config file is provided, uses default parameters.
*/
int	main(int ac, char **av)
{
	t_vesicle	vesicle;
	t_params	params;
	const char	*shear_file;

	print_header();
	init_vesicle(&vesicle);
	init_params(&params);
	if (ac > 1)
		load_config(av[1], &vesicle, &params);
	if (!validate_parameters(&vesicle, &params))
		return (1);
	print_summary(&vesicle, &params);
	printf("=== Running Simulation ===\n");
	run_simulation(&vesicle, &params, "poiseuille_trajectory.csv");
	validate_delta_scaling(&vesicle, &params);
	generate_validation_data(&vesicle, &params, "poiseuille_validation.csv");
	shear_file = "../simple_shear_flow/simple_shear_trajectory.csv";
	compare_with_shear_data(&vesicle, &params, shear_file);
	printf("\n=== Simulation Complete ===\n\n");
	return (0);
}
