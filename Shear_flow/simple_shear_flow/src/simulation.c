/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abalk <abalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 12:00:00 by abalk             #+#    #+#             */
/*   Updated: 2026/01/10 12:00:00 by abalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shear.h"

/*
** Performs one step of RK4 integration for the migration ODE.
** Uses the classic 4th-order Runge-Kutta method.
**
** @param y: current position
** @param gamma: shear rate
** @param v: vesicle parameters
** @param dt: time step
** @return: new position after one step
*/
double	rk4_step(double y, double gamma, t_vesicle *v, double dt)
{
	t_rk4	rk;
	double	dy;

	rk.k1 = migration_velocity(y, gamma, v);
	rk.k2 = migration_velocity(y + 0.5 * dt * rk.k1, gamma, v);
	rk.k3 = migration_velocity(y + 0.5 * dt * rk.k2, gamma, v);
	rk.k4 = migration_velocity(y + dt * rk.k3, gamma, v);
	dy = (dt / 6.0) * (rk.k1 + 2.0 * rk.k2 + 2.0 * rk.k3 + rk.k4);
	return (y + dy);
}

/*
** Applies boundary conditions to keep vesicle inside channel.
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
** Main simulation loop for simple shear flow.
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

	if (!open_output_file(&fp, outfile))
		return ;
	write_csv_header(fp);
	steps = (int)(p->total_time / p->dt);
	y = p->channel_width * 0.2;
	i = 0;
	while (i < steps)
	{
		t = i * p->dt;
		if (i % OUTPUT_INTERVAL == 0)
			write_csv_row(fp, t, y, migration_velocity(y, p->shear_rate, v),
				p->shear_rate);
		y = rk4_step(y, p->shear_rate, v, p->dt);
		y = apply_boundaries(y, v->radius, p->channel_width);
		i++;
	}
	close_output_file(fp);
	printf("Final position: %.1f um\n", y * 1e6);
}
