/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shear.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abalk <abalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 12:00:00 by abalk             #+#    #+#             */
/*   Updated: 2026/01/10 12:00:00 by abalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHEAR_H
# define SHEAR_H

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>

/* Physical parameters */
# define CHANNEL_WIDTH 140.0e-6
# define VESICLE_RADIUS 10.0e-6
# define LAMBDA 1.0
# define NU 0.95
# define SHEAR_RATE 10.0

/* Simulation parameters */
# define DT 0.01
# define TOTAL_TIME 30.0
# define OUTPUT_INTERVAL 10

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
	double	shear_rate;
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
double	migration_velocity(double y, double gamma, t_vesicle *v);

/* Function prototypes - simulation.c */
void	run_simulation(t_vesicle *v, t_params *p, const char *outfile);
double	rk4_step(double y, double gamma, t_vesicle *v, double dt);

/* Function prototypes - validation.c */
void	validate_scaling_law(t_vesicle *v, t_params *p);
void	generate_validation_data(t_vesicle *v, t_params *p, const char *file);

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
