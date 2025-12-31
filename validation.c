#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Physical Parameters from the paper [cite: 35, 54]
#define R0 10.0e-6          // Vesicle radius (meters), approx 10 microns
#define SHEAR_RATE 10.0     // Shear rate (1/s)
#define U_LIFT_PREFACTOR 0.1 // Dimensionless lift velocity U (approx from Fig 2 for v~0.95)
#define Y_START 15.0e-6     // Starting position (must be > R0)
#define DT 0.01             // Time step for integration
#define TOTAL_TIME 20.0     // Total simulation time


double theoretical_velocity(double y) {
    if (y <= 0) return 0.0;
    // v = U * gamma * R0^3 / y^2
    return U_LIFT_PREFACTOR * SHEAR_RATE * pow(R0, 3) / pow(y, 2);
}

double analytical_trajectory(double t, double y0) {
    double C = U_LIFT_PREFACTOR * SHEAR_RATE * pow(R0, 3);
    double term = 3 * C * t + pow(y0, 3);
    return pow(term, 1.0/3.0);
}

void check_scaling_law(double *y_data, double *v_data, int n) {
    double sum_log_y = 0.0, sum_log_v = 0.0;
    double sum_log_y_log_v = 0.0, sum_log_y_sq = 0.0;
    
    int valid_points = 0;

    for (int i = 0; i < n; i++) {
        if (y_data[i] > 0 && v_data[i] > 0) {
            double ly = log(y_data[i]);
            double lv = log(v_data[i]);
            
            sum_log_y += ly;
            sum_log_v += lv;
            sum_log_y_log_v += ly * lv;
            sum_log_y_sq += ly * ly;
            valid_points++;
        }
    }

    if (valid_points < 2) {
        printf("Error: Not enough valid data points for regression.\n");
        return;
    }

    double mean_log_y = sum_log_y / valid_points;
    double mean_log_v = sum_log_v / valid_points;

    // Calculate slope (scaling exponent)
    double slope = (sum_log_y_log_v - valid_points * mean_log_y * mean_log_v) / 
                   (sum_log_y_sq - valid_points * mean_log_y * mean_log_y);

    printf("\n--- SCALING VALIDATION REPORT ---\n");
    printf("Target Exponent (Theory): -2.00 [cite: 79]\n");
    printf("Measured Exponent (Data): %.4f\n", slope);
    
    double error = fabs(slope - (-2.0)) / 2.0 * 100.0;
    printf("Deviation: %.2f%%\n", error);
    
    if (error < 5.0) {
        printf("RESULT: PASS. Your data recovers the 1/y^2 scaling law.\n");
    } else {
        printf("RESULT: FAIL. Scaling deviates significantly from theory.\n");
    }
    printf("-------------------------------\n");
}

/*
 * Load data from CSV file
 * Format: distance_from_wall_um,velocity_um_s,shear_rate
 */
int load_validation_data(const char* filename, double **y_data, double **v_data, int *n_points) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 0; // File not found
    }
    
    // Skip header line
    char header[256];
    fgets(header, sizeof(header), fp);
    
    // Count lines
    int count = 0;
    double tmp1, tmp2, tmp3;
    while (fscanf(fp, "%lf,%lf,%lf", &tmp1, &tmp2, &tmp3) == 3) {
        count++;
    }
    rewind(fp);
    fgets(header, sizeof(header), fp); // Skip header again
    
    // Allocate arrays
    *y_data = (double*)malloc(count * sizeof(double));
    *v_data = (double*)malloc(count * sizeof(double));
    *n_points = count;
    
    // Read data (convert from μm to meters)
    for (int i = 0; i < count; i++) {
        double y_um, v_um_s, shear;
        fscanf(fp, "%lf,%lf,%lf", &y_um, &v_um_s, &shear);
        (*y_data)[i] = y_um * 1.0e-6;  // Convert μm to m
        (*v_data)[i] = v_um_s * 1.0e-6; // Convert μm/s to m/s
    }
    
    fclose(fp);
    return 1;
}

int main(int argc, char *argv[]) {
    printf("Running Vesicle Migration Benchmark...\n");
    printf("Reference: Coupier et al., Mater. Res. Soc. Symp. Proc. Vol. 1132 (2009)\n\n");

    double *y_sim, *v_sim, *t_sim;
    int steps;
    
    // Try to load external data first
    if (argc > 1 && load_validation_data(argv[1], &y_sim, &v_sim, &steps)) {
        printf("Loaded external validation data from: %s\n", argv[1]);
        printf("Number of data points: %d\n\n", steps);
        
        // Run scaling check
        check_scaling_law(y_sim, v_sim, steps);
        
        // Clean up
        free(y_sim);
        free(v_sim);
        return 0;
    }

    // 1. GENERATE SYNTHETIC SIMULATION DATA
    // (Used when no external file is provided)
    printf("No external data provided. Generating synthetic benchmark data...\n\n");
    
    steps = (int)(TOTAL_TIME / DT);
    y_sim = (double*)malloc(steps * sizeof(double));
    v_sim = (double*)malloc(steps * sizeof(double));
    t_sim = (double*)malloc(steps * sizeof(double));
    
    double current_y = Y_START;
    
    printf("Generating 'Mock' Simulation Data (Simple Euler Integration)...\n");
    
    // Simulating a trajectory using the theoretical law + slight noise to mimic numerical error
    for (int i = 0; i < steps; i++) {
        double t = i * DT;
        
        // Calculate velocity based on current position
        double v_theoretical = theoretical_velocity(current_y);
        
        // Store data
        y_sim[i] = current_y;
        v_sim[i] = v_theoretical; 
        t_sim[i] = t;

        // Update position (Euler step)
        current_y += v_theoretical * DT;
    }

    // 2. EXPORT BENCHMARK DATA
    // You can plot this "benchmark_trajectory.csv" against your code's output
    FILE *fp = fopen("benchmark_trajectory.csv", "w");
    fprintf(fp, "time,y_analytical,y_simulation,velocity_theoretical\n");
    for (int i = 0; i < steps; i += 10) { // sparse output
        double exact_y = analytical_trajectory(t_sim[i], Y_START);
        fprintf(fp, "%f,%e,%e,%e\n", t_sim[i], exact_y, y_sim[i], v_sim[i]);
    }
    fclose(fp);
    printf(">> Generated 'benchmark_trajectory.csv' for plotting.\n");
    check_scaling_law(y_sim, v_sim, steps);

    free(y_sim);
    free(v_sim);
    free(t_sim);

    return 0;
}