#include "header.h"

double lift_velocity_coefficient(double lambda, double nu) {

    if (nu >= 1.0) return 0.0;
    
    double base_U = 0.15; 
    double deflation_factor = (1.0 - nu) / (1.0 - 0.6);
    
    return base_U + 0.15 * deflation_factor;
}

double poiseuille_velocity(double y, double W, double U_max) {
    double y_normalized = 2.0 * y / W - 1.0; 
    return U_max * (1.0 - y_normalized * y_normalized);
}

double poiseuille_shear_rate(double y, double W, double U_max) {
    double y_normalized = 2.0 * y / W - 1.0;
    return fabs(4.0 * U_max / W * y_normalized);
}

double migration_velocity_poiseuille(double y, double W, double U_max, 
                                     double vesicle_r0, double lambda, double nu, double delta) {
    if (y <= vesicle_r0)
        return 0.0;
    
    // Don't let vesicle get too close to walls !!!!!!!!!!!!! 
    if (y < 1.5 * vesicle_r0 || y > W - 1.5 * vesicle_r0) {
        return 0.0;
    }
    
    double U = lift_velocity_coefficient(lambda, nu);
    double gamma = poiseuille_shear_rate(y, W, U_max);
    
    // Migration velocity formula
    double numerator = U * gamma * pow(vesicle_r0, delta + 1.0);
    double denominator = pow(y - vesicle_r0, delta);
    
    double v_migration = numerator / denominator;
    
    // Vesicles migrate towards centerline
    double centerline = W / 2.0;
    if (y < centerline) {
        return fabs(v_migration);  // Move up towards center
    } else {
        return -fabs(v_migration); // Move down towards center
    }
}

double migration_velocity_simple_shear(double y, double gamma, double vesicle_r0, 
                                       double lambda, double nu) {
    if (y <= 0) return 0.0;
    
    double U = lift_velocity_coefficient(lambda, nu);
    return U * gamma * pow(vesicle_r0, 3.0) / pow(y, 2.0);
}

// Run simulation with specified parameters ( princess treatement )
void run_simulation(const char* mode, const char* output_file) {
    int steps = (int)(TOTAL_TIME / DT);
    
    double *time = (double*)malloc(steps * sizeof(double));
    double *position = (double*)malloc(steps * sizeof(double));
    double *velocity = (double*)malloc(steps * sizeof(double));
    double *shear_rate = (double*)malloc(steps * sizeof(double));
    
    // Initial conditions: Start vesicle near wall
    double y = CHANNEL_WIDTH * 0.2;  // Start at 20% from bottom wall
    
    printf("\n=== Running %s Simulation ===\n", mode);
    double vesicle_r0 = VESICLE_RADIUS;
    
    printf("Channel width: %.1f μm\n", CHANNEL_WIDTH * 1e6);
    printf("Vesicle radius: %.1f μm\n", vesicle_r0 * 1e6);
    printf("Initial position: %.1f μm\n", y * 1e6);
    printf("Reduced volume: %.2f\n", NU);
    printf("Viscosity ratio: %.1f\n", LAMBDA);
    printf("\n");
    
    for (int i = 0; i < steps; i++) {
        time[i] = i * DT;
        position[i] = y;
        
        if (strcmp(mode, "poiseuille") == 0) {
            velocity[i] = migration_velocity_poiseuille(y, CHANNEL_WIDTH, 
                                                        MAX_FLOW_VELOCITY, vesicle_r0, 
                                                        LAMBDA, NU, DELTA);
            shear_rate[i] = poiseuille_shear_rate(y, CHANNEL_WIDTH, MAX_FLOW_VELOCITY);
        } else {
            // Simple shear for comparison
            double const_shear = 10.0; // 10 s^-1
            velocity[i] = migration_velocity_simple_shear(y, const_shear, vesicle_r0, LAMBDA, NU);
            shear_rate[i] = const_shear;
        }
        
        // Update position (Euler method)
        y += velocity[i] * DT;
        
        // Boundary conditions
        if (y < vesicle_r0) y = vesicle_r0;
        if (y > CHANNEL_WIDTH - vesicle_r0) y = CHANNEL_WIDTH - vesicle_r0;
    }
    
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", output_file);
        free(time);
        free(position);
        free(velocity);
        free(shear_rate);
        return;
    }
    
    fprintf(fp, "time,position_um,velocity_um_s,shear_rate,y_normalized\n");
    for (int i = 0; i < steps; i += OUTPUT_INTERVAL) {
        double y_normalized = position[i] / CHANNEL_WIDTH;
        fprintf(fp, "%.3f,%.6f,%.6e,%.6f,%.6f\n", 
                time[i], 
                position[i] * 1e6,           // Convert to μm
                velocity[i] * 1e6,           // Convert to μm/s
                shear_rate[i],
                y_normalized);
    }
    fclose(fp);
    
    printf("Results saved to: %s\n", output_file);
    printf("Final position: %.1f μm (normalized: %.3f)\n", 
           y * 1e6, y / CHANNEL_WIDTH);
    printf("Target: %.1f μm (centerline)\n\n", CHANNEL_WIDTH * 1e6 / 2.0);
    
    free(time);
    free(position);
    free(velocity);
    free(shear_rate);
}
