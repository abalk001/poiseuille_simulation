/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abalk <abalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 12:00:00 by abalk             #+#    #+#             */
/*   Updated: 2026/01/10 12:00:00 by abalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shear.h"

/*
** Performs linear regression on log-log data to find power law exponent.
** Fits log(v) = a + b*log(y) and returns the slope b.
*/
static double	compute_slope(double *log_y, double *log_v, int n)
{
	double	sum_x;
	double	sum_y;
	double	sum_xy;
	double	sum_xx;
	int		i;

	sum_x = 0.0;
	sum_y = 0.0;
	sum_xy = 0.0;
	sum_xx = 0.0;
	i = 0;
	while (i < n)
	{
		sum_x += log_y[i];
		sum_y += log_v[i];
		sum_xy += log_y[i] * log_v[i];
		sum_xx += log_y[i] * log_y[i];
		i++;
	}
	return ((n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x));
}

/*
** Validates the 1/y^2 scaling law using log-log regression.
** Expected slope is -2.0 for simple shear flow.
*/
void	validate_scaling_law(t_vesicle *v, t_params *p)
{
	double	log_y[100];
	double	log_v[100];
	double	y;
	double	vel;
	int		n;

	n = 0;
	y = 2.0 * v->radius;
	while (y < p->channel_width / 2.0 && n < 100)
	{
		vel = migration_velocity(y, p->shear_rate, v);
		log_y[n] = log(y * 1e6);
		log_v[n] = log(vel * 1e9);
		y += v->radius / 2.0;
		n++;
	}
	printf("\n=== SCALING LAW VALIDATION ===\n");
	printf("Target exponent (theory): -2.00\n");
	printf("Measured exponent: %.4f\n", compute_slope(log_y, log_v, n));
	printf("Result: %s\n\n", fabs(compute_slope(log_y, log_v, n) + 2.0) < 0.01
		? "PASS" : "FAIL");
}

/*
** Generates validation data file with position-velocity pairs.
*/
void	generate_validation_data(t_vesicle *v, t_params *p, const char *file)
{
	FILE	*fp;
	double	y;
	double	vel;

	if (!open_output_file(&fp, file))
		return ;
	fprintf(fp, "distance_from_wall_um,velocity_um_s,shear_rate\n");
	y = 2.0 * v->radius;
	while (y < p->channel_width / 2.0)
	{
		vel = migration_velocity(y, p->shear_rate, v);
		fprintf(fp, "%.6f,%.6e,%.6f\n", y * 1e6, vel * 1e6, p->shear_rate);
		y += v->radius / 2.0;
	}
	close_output_file(fp);
	printf("Validation data saved to: %s\n", file);
}
