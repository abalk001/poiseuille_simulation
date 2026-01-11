#include "header.h"

double lift_velocity_coefficient(double lambda, double nu) {
    if (nu >= 1.0) return 0.0;
    if (nu <= 0.0) return 0.5;

    double nu_factor = (1.0 - nu) / (1.0 - 0.6);
    double lambda_factor = 0.5 * (1.0 + lambda) / (1.0 + lambda);

    double base_U = 0.15 * lambda_factor;
    double deflation_contribution = 0.15 * nu_factor * lambda_factor;
    
    return base_U + deflation_contribution;
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
    if (y <= vesicle_r0 || y >= W - vesicle_r0)
        return 0.0;
    
    double U = lift_velocity_coefficient(lambda, nu);
    double gamma = poiseuille_shear_rate(y, W, U_max);
    
    double d_bottom = y - vesicle_r0;
    double d_top = (W - y) - vesicle_r0;
    double d_nearest = fmin(d_bottom, d_top);

    double d_min = 0.1 * vesicle_r0;
    double d_reg = fmax(d_nearest, d_min);

    double numerator = U * gamma * pow(vesicle_r0, delta + 1.0);
    double denominator = pow(d_reg, delta);
    double v_magnitude = numerator / denominator;
    
    if (d_bottom < d_top) {
        return v_magnitude;
    } else {
        return -v_magnitude;
    }
}

double migration_velocity_simple_shear(double y, double gamma, double vesicle_r0, 
                                       double lambda, double nu) {
    if (y <= 0) return 0.0;
    
    double U = lift_velocity_coefficient(lambda, nu);
    return U * gamma * pow(vesicle_r0, 3.0) / pow(y, 2.0);
}

void run_simulation(const char* mode, const char* output_file) {
    if (!validate_parameters(CHANNEL_WIDTH, VESICLE_RADIUS, NU, MAX_FLOW_VELOCITY, DT)) {
        fprintf(stderr, "Parameter validation failed. Aborting simulation.\n");
        return;
    }
    
    int steps = (int)(TOTAL_TIME / DT);
    
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", output_file);
        return;
    }
    fprintf(fp, "time,position_um,velocity_um_s,shear_rate,y_normalized\n");
    
    double y = CHANNEL_WIDTH * 0.2;
    
    printf("\n=== Running %s Simulation ===\n", mode);
    double vesicle_r0 = VESICLE_RADIUS;
    
    printf("Channel width: %.1f μm\n", CHANNEL_WIDTH * 1e6);
    printf("Vesicle radius: %.1f μm\n", vesicle_r0 * 1e6);
    printf("Initial position: %.1f μm\n", y * 1e6);
    printf("Reduced volume: %.2f\n", NU);
    printf("Viscosity ratio: %.1f\n", LAMBDA);
    printf("Time step: %.4f s\n", DT);
    printf("Using RK4 integration\n");
    printf("\n");
    
    double (*compute_velocity)(double, double, double, double, double, double, double);
    double shear_param;
    
    if (strcmp(mode, "poiseuille") == 0) {
        compute_velocity = migration_velocity_poiseuille;
        shear_param = MAX_FLOW_VELOCITY;
    }
    
    for (int i = 0; i < steps; i++) {
        double t = i * DT;
        double current_vel, current_shear;
        
        if (strcmp(mode, "poiseuille") == 0) {
            current_vel = migration_velocity_poiseuille(y, CHANNEL_WIDTH, 
                                                        MAX_FLOW_VELOCITY, vesicle_r0, 
                                                        LAMBDA, NU, DELTA);
            current_shear = poiseuille_shear_rate(y, CHANNEL_WIDTH, MAX_FLOW_VELOCITY);
        } else {
            double const_shear = 10.0;
            current_vel = migration_velocity_simple_shear(y, const_shear, vesicle_r0, LAMBDA, NU);
            current_shear = const_shear;
        }
        
        if (i % OUTPUT_INTERVAL == 0) {
            double y_normalized = y / CHANNEL_WIDTH;
            fprintf(fp, "%.3f,%.6f,%.6e,%.6f,%.6f\n", 
                    t, y * 1e6, current_vel * 1e6, current_shear, y_normalized);
        }
        
        double k1, k2, k3, k4;
        if (strcmp(mode, "poiseuille") == 0) {
            k1 = migration_velocity_poiseuille(y, CHANNEL_WIDTH, MAX_FLOW_VELOCITY, 
                                               vesicle_r0, LAMBDA, NU, DELTA);
            k2 = migration_velocity_poiseuille(y + 0.5*DT*k1, CHANNEL_WIDTH, MAX_FLOW_VELOCITY, 
                                               vesicle_r0, LAMBDA, NU, DELTA);
            k3 = migration_velocity_poiseuille(y + 0.5*DT*k2, CHANNEL_WIDTH, MAX_FLOW_VELOCITY, 
                                               vesicle_r0, LAMBDA, NU, DELTA);
            k4 = migration_velocity_poiseuille(y + DT*k3, CHANNEL_WIDTH, MAX_FLOW_VELOCITY, 
                                               vesicle_r0, LAMBDA, NU, DELTA);
        } else {
            double const_shear = 10.0;
            k1 = migration_velocity_simple_shear(y, const_shear, vesicle_r0, LAMBDA, NU);
            k2 = migration_velocity_simple_shear(y + 0.5*DT*k1, const_shear, vesicle_r0, LAMBDA, NU);
            k3 = migration_velocity_simple_shear(y + 0.5*DT*k2, const_shear, vesicle_r0, LAMBDA, NU);
            k4 = migration_velocity_simple_shear(y + DT*k3, const_shear, vesicle_r0, LAMBDA, NU);
        }
        
        double dy = (DT / 6.0) * (k1 + 2*k2 + 2*k3 + k4);
        
        double max_dy = 0.1 * vesicle_r0;
        if (fabs(dy) > max_dy) {
            fprintf(stderr, "Warning: Large displacement %.3e m detected at t=%.2f s. "
                    "Consider reducing DT. Clamping to %.3e m.\n", dy, t, max_dy);
            dy = (dy > 0) ? max_dy : -max_dy;
        }
        
        y += dy;
        
        if (y < vesicle_r0) y = vesicle_r0;
        if (y > CHANNEL_WIDTH - vesicle_r0) y = CHANNEL_WIDTH - vesicle_r0;
    }
    
    fclose(fp);
    
    printf("Results saved to: %s\n", output_file);
    printf("Final position: %.1f μm (normalized: %.3f)\n", 
           y * 1e6, y / CHANNEL_WIDTH);
    printf("Target: %.1f μm (centerline)\n\n", CHANNEL_WIDTH * 1e6 / 2.0);
}

int validate_parameters(double W, double r0, double nu, double U_max, double dt) {
    int valid = 1;
    
    printf("\n=== Parameter Validation ===\n");
    
    if (W <= 2.0 * r0) {
        fprintf(stderr, "ERROR: Channel width (%.2e m) must be > 2*radius (%.2e m)\n", W, 2.0*r0);
        valid = 0;
    } else {
        printf("✓ Channel width: %.2e m > 2*radius\n", W);
    }
    
    if (nu <= 0.0 || nu > 1.0) {
        fprintf(stderr, "ERROR: Reduced volume nu=%.3f must be in (0,1]\n", nu);
        valid = 0;
    } else {
        printf("✓ Reduced volume: %.3f ∈ (0,1]\n", nu);
    }
    
    if (U_max <= 0.0) {
        fprintf(stderr, "ERROR: Maximum velocity U_max=%.2e must be > 0\n", U_max);
        valid = 0;
    } else {
        printf("✓ Maximum velocity: %.2e m/s > 0\n", U_max);
    }
    
    if (dt <= 0.0) {
        fprintf(stderr, "ERROR: Time step dt=%.2e must be > 0\n", dt);
        valid = 0;
    } else {
        printf("✓ Time step: %.2e s > 0\n", dt);
        
        double max_velocity_estimate = U_max * 0.5;
        double CFL = max_velocity_estimate * dt / r0;
        if (CFL > 0.5) {
            fprintf(stderr, "WARNING: CFL number %.3f > 0.5. Consider reducing DT for stability.\n", CFL);
        }
    }
    
    printf("\n");
    return valid;
}
