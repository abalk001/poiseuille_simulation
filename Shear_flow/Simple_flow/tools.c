#include "shear.h"




void	print_summary(t_vesicle *v, t_params *p)
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

void inti_vesicle(t_vesicle *v)
{
    v->radius = VESICLE_RADIUS;
    v->lambda = LAMBDA;
    v->nu = NU;
    v->position = 0.0;
}

void init_params(t_params *p)
{
    p->channel_width = CHANNEL_WIDTH;
    p->shear_rate = SHEAR_RATE;
    p->dt = DT;
    p->total_time = TOTAL_TIME;
}