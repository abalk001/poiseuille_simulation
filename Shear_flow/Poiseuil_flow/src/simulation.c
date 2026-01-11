#include "poiseuille.h"

/*
** Performs one step of RK4 integration for the Poiseuille migration ODE.
** Uses the classic 4th-order Runge-Kutta method.
**
** @param y: current position
** @param v: vesicle parameters
** @param p: simulation parameters
** @param dt: time step
** @return: new position after one step
*/
double	rk4_step(double y, t_vesicle *v, t_params *p, double dt)
{
	t_rk4	rk;
	double	dy;

	rk.k1 = migration_velocity(y, v, p);
	rk.k2 = migration_velocity(y + 0.5 * dt * rk.k1, v, p);
	rk.k3 = migration_velocity(y + 0.5 * dt * rk.k2, v, p);
	rk.k4 = migration_velocity(y + dt * rk.k3, v, p);
	dy = (dt / 6.0) * (rk.k1 + 2.0 * rk.k2 + 2.0 * rk.k3 + rk.k4);
	return (y + dy);
}

/*
** Applies boundary conditions to keep vesicle inside channel.
** Vesicle center must be at least one radius from each wall.
**
** @param y: current position
** @param r0: vesicle radius
** @param w: channel width
** @return: clamped position
*/
static double	apply_boundaries(double y, double r0, double w)
{
	if (y < r0)
		return (r0);
	if (y > w - r0)
		return (w - r0);
	return (y);
}

/*
** Main simulation loop for Poiseuille flow migration.
** Vesicle starts near the bottom wall and migrates toward centerline.
**
** @param v: vesicle parameters
** @param p: simulation parameters
** @param outfile: output CSV filename
*/
void	run_simulation(t_vesicle *v, t_params *p, const char *outfile)
{
	FILE	*fp;
	int		steps;
	int		i;
	double	y;
	double	t;
	double	gamma;

	if (!open_output_file(&fp, outfile))
		return ;
	write_csv_header(fp);
	steps = (int)(p->total_time / p->dt);
	y = p->channel_width * 0.2;
	i = 0;
	while (i < steps)
	{
		t = i * p->dt;
		gamma = shear_rate(y, p);
		if (i % OUTPUT_INTERVAL == 0)
			write_csv_row(fp, t, y, migration_velocity(y, v, p), gamma);
		y = rk4_step(y, v, p, p->dt);
		y = apply_boundaries(y, v->radius, p->channel_width);
		i++;
	}
	close_output_file(fp);
	printf("Simulation complete.\n");
	printf("Final position: %.1f um (%.1f%% of channel)\n",
		y * 1e6, y / p->channel_width * 100.0);
}
