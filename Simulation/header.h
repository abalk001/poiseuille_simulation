#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


// --- PHYSICAL PARAMETERS ---
#define CHANNEL_WIDTH 140.0e-6    // Channel width in meters (140 μm from paper)
#define VESICLE_RADIUS 10.0e-6    // Vesicle effective radius (10 μm)
#define LAMBDA 1.0                // Viscosity ratio (ηin/ηout)
#define NU 0.95                   // Reduced volume (deflation)
#define MAX_FLOW_VELOCITY 100.0e-6 // Maximum velocity at centerline (m/s)
#define DELTA 1.0                 // Exponent from paper (δ = 1±0.1 for λ=1)

// --- SIMULATION PARAMETERS ---
#define DT 0.01                   // Time step (seconds)
#define TOTAL_TIME 30.0           // Total simulation time (seconds)
#define OUTPUT_INTERVAL 10        // Output every N steps

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
#endif 