#ifndef POISEUILLE_H
# define POISEUILLE_H

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>

/* Physical parameters - default values */
# define CHANNEL_WIDTH 140.0e-6
# define VESICLE_RADIUS 10.0e-6
# define LAMBDA 1.0
# define NU 0.95
# define U_MAX 100.0e-6
# define DELTA 1.0

/* Simulation parameters */
# define DT 0.01
# define TOTAL_TIME 30.0
# define OUTPUT_INTERVAL 10
# define MIN_DISTANCE 0.1

/* Structures */
typedef struct s_vesicle
{
	double	radius;
	double	lambda;
	double	nu;
	double	position;
}	t_vesicle;

typedef struct s_params
{
	double	channel_width;
	double	u_max;
	double	delta;
	double	dt;
	double	total_time;
}	t_params;

typedef struct s_rk4
{
	double	k1;
	double	k2;
	double	k3;
	double	k4;
}	t_rk4;

/* Function prototypes - physics.c */
double	lift_coefficient(double lambda, double nu);
double	shear_rate(double y, t_params *p);
double	migration_velocity(double y, t_vesicle *v, t_params *p);
double	poiseuille_velocity(double y, t_params *p);

/* Function prototypes - simulation.c */
void	run_simulation(t_vesicle *v, t_params *p, const char *outfile);
double	rk4_step(double y, t_vesicle *v, t_params *p, double dt);

/* Function prototypes - validation.c */
void	validate_delta_scaling(t_vesicle *v, t_params *p);
void	generate_validation_data(t_vesicle *v, t_params *p, const char *file);
void	compare_with_shear_data(t_vesicle *v, t_params *p, const char *file);

/* Function prototypes - utils.c */
int		validate_parameters(t_vesicle *v, t_params *p);
void	init_vesicle(t_vesicle *v);
void	init_params(t_params *p);
void	print_header(void);

/* Function prototypes - output.c */
int		open_output_file(FILE **fp, const char *filename);
void	write_csv_header(FILE *fp);
void	write_csv_row(FILE *fp, double t, double y, double v, double gamma);
void	close_output_file(FILE *fp);

/* Function prototypes - config.c */
int		load_config(const char *filename, t_vesicle *v, t_params *p);
void	create_default_config(const char *filename);

#endif
