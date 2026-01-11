#include "poiseuille.h"

/*
** Performs linear regression on log-log data to find power law exponent.
** Fits log(v) = a + b*log(y) and returns the slope b.
*/
static double	compute_slope(double *log_d, double *log_v, int n)
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
		sum_x += log_d[i];
		sum_y += log_v[i];
		sum_xy += log_d[i] * log_v[i];
		sum_xx += log_d[i] * log_d[i];
		i++;
	}
	return ((n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x));
}

/*
** Validates the 1/(y-R0)^delta scaling law using log-log regression.
** For Poiseuille flow: v = U * gamma(y) * R^(d+1) / (y-R0)^d
** So we need to normalize by gamma(y) to isolate the scaling.
** v/gamma ~ 1/d^delta, so log(v/gamma) vs log(d) should give slope = -delta
*/
void	validate_delta_scaling(t_vesicle *v, t_params *p)
{
	double	log_d[100];
	double	log_v_norm[100];
	double	y;
	double	d;
	double	vel;
	double	gamma;
	double	slope;
	int		n;

	n = 0;
	y = 2.0 * v->radius;
	while (y < p->channel_width / 3.0 && n < 100)
	{
		vel = fabs(migration_velocity(y, v, p));
		gamma = shear_rate(y, p);
		d = y - v->radius;
		if (vel > 0.0 && d > 0.0 && gamma > 0.0)
		{
			log_d[n] = log(d * 1e6);
			log_v_norm[n] = log((vel / gamma) * 1e6);
			n++;
		}
		y += v->radius / 2.0;
	}
	slope = compute_slope(log_d, log_v_norm, n);
	printf("\n=== SCALING LAW VALIDATION ===\n");
	printf("Target exponent (theory): -%.2f\n", p->delta);
	printf("Measured exponent (v/gamma vs d): %.4f\n", slope);
	printf("Result: %s\n\n", fabs(slope + p->delta) < 0.15 ? "PASS" : "FAIL");
}

/*
** Generates validation data file with position-velocity pairs.
*/
void	generate_validation_data(t_vesicle *v, t_params *p, const char *file)
{
	FILE	*fp;
	double	y;
	double	vel;
	double	gamma;

	if (!open_output_file(&fp, file))
		return ;
	fprintf(fp, "position_um,distance_um,velocity_um_s,shear_rate\n");
	y = 1.5 * v->radius;
	while (y < p->channel_width / 2.0)
	{
		vel = migration_velocity(y, v, p);
		gamma = shear_rate(y, p);
		fprintf(fp, "%.6f,%.6f,%.6e,%.6f\n",
			y * 1e6, (y - v->radius) * 1e6, vel * 1e6, gamma);
		y += v->radius / 2.0;
	}
	close_output_file(fp);
	printf("Validation data saved to: %s\n", file);
}

/*
** Compares Poiseuille simulation output with simple shear data.
** Reads the simple shear trajectory and computes statistics.
*/
void	compare_with_shear_data(t_vesicle *v, t_params *p, const char *file)
{
	FILE	*fp;
	char	line[256];
	double	t;
	double	y_shear;
	double	v_shear;
	double	gamma_shear;
	double	y_norm;
	int		count;

	fp = fopen(file, "r");
	if (!fp)
	{
		printf("Cannot open shear data file: %s\n", file);
		return ;
	}
	printf("\n=== COMPARISON WITH SIMPLE SHEAR ===\n");
	printf("%-8s %-12s %-12s %-12s\n",
		"Time", "Shear_y", "Pois_vel", "Shear_vel");
	printf("%-8s %-12s %-12s %-12s\n", "(s)", "(um)", "(um/s)", "(um/s)");
	fgets(line, sizeof(line), fp);
	count = 0;
	while (fgets(line, sizeof(line), fp) && count < 10)
	{
		if (sscanf(line, "%lf,%lf,%lf,%lf,%lf",
				&t, &y_shear, &v_shear, &gamma_shear, &y_norm) == 5)
		{
			printf("%-8.1f %-12.2f %-12.4e %-12.4e\n",
				t, y_shear,
				migration_velocity(y_shear * 1e-6, v, p) * 1e6,
				v_shear);
			count++;
		}
	}
	fclose(fp);
	(void)v;
	(void)p;
}
