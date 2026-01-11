#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#define CHANNEL_WIDTH 140.0e-6
#define VESICLE_RADIUS 10.0e-6
#define LAMBDA 1.0
#define NU 0.95
#define MAX_FLOW_VELOCITY 100.0e-6
#define DELTA 1.0

#define DT 0.01
#define TOTAL_TIME 30.0
#define OUTPUT_INTERVAL 10

void generate_validation_data(const char* output_file);
void compare_deflations();
double lift_velocity_coefficient(double lambda, double nu);
double poiseuille_velocity(double y, double W, double U_max);
double poiseuille_shear_rate(double y, double W, double U_max);
double migration_velocity_poiseuille(double y, double W, double U_max, 
                                     double vesicle_r0, double lambda, double nu, double delta);
double migration_velocity_simple_shear(double y, double gamma, double vesicle_r0, 
                                       double lambda, double nu);
void run_simulation(const char* mode, const char* output_file);
int validate_parameters(double W, double r0, double nu, double U_max, double dt);
void run_regression_checks();							 
#endif 