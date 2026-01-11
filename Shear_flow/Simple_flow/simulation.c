#include "shear.h"

static void write_csv_row(FILE *fd, int *y, double *t, double *v, double *gamma)
{
    // time, position_um, velocity_um_s, shear_rate, y_normalized 
    double y_norm;

    y_norm = *y / CHANNEL_WIDTH; 
    /* y_norm means the vescile's position as a fraction of the channel width
    if the y_norm = 0, the vescile is a at the wall 
    if the y_norm = 1 the vescile is at the opposite wall
    if the y_norm = 0.5 the vescile is at the channel centerline */

    fprintf(fd, "%.3f,%.6f,%.6e,%.6f,%.6f\n",
        t, *y * 1e6, *v * 1e6, *gamma, y_norm);
}

double lift_coefficient(double lambda, double nu)
{
    double deflation; 
    double u_base;
    double lambda_correction;

    if (nu >= 1.0)
        return (0.0);
    if (nu <= 0.6)
        return (0.25);
    deflation = 1.0 - nu;
    u_base = pow(deflation, 0.8);
    lambda_correction = (1.0 + 0.2 * (lambda - 1.0)) / (1.0 + 0.5 * lambda);
    return (u_base * lambda_correction);
}
double migration_velocity(double y, double gamma, t_vesicle *v)
{
    double u_coeff;
    double r_cubed;

    if (y <= 0.0)
        return (0.0);
    u_coeff = lift_coefficient(v->lambda, v->nu);
    r_cubed = pow(v->radius, 3.0);
    return (u_coeff * gamma * r_cubed / pow(y, 2.0));
}
int  run_simulation(t_vesicle *v, t_params *p, const char *outfile)
{
    FILE    *fd;
    int     steps;
    int    i;
    double  y;
    double  t;

    if(open_file_output(&fd, outfile) < 0)
        return (-1);
    steps = (int)(p->total_time / p->dt);
    y = p->channel_width * 0.2; // here we put the particle near the wall by a distanc of 20% of the channel width
    i = 0;
    while (i < steps)
    {
        t = i * p->dt;
        if (i % OUTPUT_INTERVAL == 0)
          write_csv_row(fd, &y, &t, 
            &(migration_velocity(y, p->shear_rate, v)), &(p->shear_rate));
    }    y = rk4_step(y, p->shear_rate, v, p->dt);
        y = apply_boundaries(y, v->radius, p->channel_width);
        i++;
    }
    if (fd != NULL)
		fclose(fd);
    return (0);
}

int open_file_output(FILE **fd, const char *filename)
{
    *fd = fopen(filename, "w");
    if (*fd == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return (-1);
    }
    fprintf(*fd, "time,position_um,velocity_um_s,shear_rate,y_normalized\n");
    return (0);
}
