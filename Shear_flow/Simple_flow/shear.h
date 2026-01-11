#ifndef SHEAR_H
# define SHEAR_H

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
#include <fcntl.h>


# define CHANNEL_WIDTH 140.0e-6 
# define VESICLE_RADIUS 10.0e-6 // 15 um 
# define LAMBDA 1.0
# define NU 0.95
# define SHEAR_RATE 10.0


# define DT 0.01
# define TOTAL_TIME 30.0
# define OUTPUT_INTERVAL 10

typedef struct s_vesicle
{
    double  radius;
    double  lambda;
    double  nu;
    double  position;
}   t_vesicle;

typedef struct s_params
{
	double	channel_width;
	double	shear_rate;
	double	dt;
	double	total_time;
}	t_params;

int run_simulation(t_vesicle *v, t_params *p, const char *outfile);
void print_summary(t_vesicle *v, t_params *p);
void init_params(t_params *p);
void inti_vesicle(t_vesicle *v);
int open_file_output(FILE **fd, const char *filename);
#endif