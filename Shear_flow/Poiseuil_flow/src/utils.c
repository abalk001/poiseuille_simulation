#include "poiseuille.h"

/*
** Initializes vesicle structure with default parameters.
*/
void	init_vesicle(t_vesicle *v)
{
	v->radius = VESICLE_RADIUS;
	v->lambda = LAMBDA;
	v->nu = NU;
	v->position = 0.0;
}

/*
** Initializes simulation parameters with default values.
*/
void	init_params(t_params *p)
{
	p->channel_width = CHANNEL_WIDTH;
	p->u_max = U_MAX;
	p->delta = DELTA;
	p->dt = DT;
	p->total_time = TOTAL_TIME;
}

/*
** Validates physical parameters before simulation.
** Returns 1 if valid, 0 otherwise.
*/
int	validate_parameters(t_vesicle *v, t_params *p)
{
	int	valid;

	valid = 1;
	printf("\n=== Parameter Validation ===\n");
	if (p->channel_width <= 2.0 * v->radius)
	{
		printf("ERROR: Channel too narrow for vesicle\n");
		valid = 0;
	}
	if (v->nu <= 0.0 || v->nu > 1.0)
	{
		printf("ERROR: Reduced volume must be in (0,1]\n");
		valid = 0;
	}
	if (p->u_max <= 0.0)
	{
		printf("ERROR: Maximum velocity must be positive\n");
		valid = 0;
	}
	if (p->delta <= 0.0)
	{
		printf("ERROR: Poiseuille exponent must be positive\n");
		valid = 0;
	}
	if (valid)
		printf("All parameters valid.\n\n");
	return (valid);
}

/*
** Prints simulation header with project info.
*/
void	print_header(void)
{
	printf("\n");
	printf("==============================================\n");
	printf("  POISEUILLE FLOW VESICLE MIGRATION\n");
	printf("  Based on Coupier et al. (2009)\n");
	printf("==============================================\n");
	printf("\n");
	printf("Physics: dy/dt = U(l,v) * gamma(y) * R0^(d+1) / (y-R0)^d\n");
	printf("Where:   gamma(y) = 4*U_max/W * |2y/W - 1|\n");
	printf("Method:  4th-order Runge-Kutta\n\n");
}
