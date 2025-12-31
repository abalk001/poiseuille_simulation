#include "header.h"

void generate_validation_data(const char* output_file) {
    printf("\n=== Generating Validation Data ===\n");
    printf("Testing 1/y^2 scaling law for simple shear flow\n\n");
    
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error opening validation file\n");
        return;
    }
    
    fprintf(fp, "distance_from_wall_um,velocity_um_s,shear_rate\n");
    
    double const_shear = 10.0; // 10 s^-1
    double vesicle_r0 = VESICLE_RADIUS;
    
    // Sample at various distances from wall
    for (double y = 2.0 * vesicle_r0; y < CHANNEL_WIDTH / 2.0; y += vesicle_r0 / 2.0) {
        double v = migration_velocity_simple_shear(y, const_shear, vesicle_r0, LAMBDA, NU);
        fprintf(fp, "%.6f,%.6e,%.6f\n", y * 1e6, v * 1e6, const_shear);
    }
    
    fclose(fp);
    printf("Validation data saved to: %s\n", output_file);
    printf("Use this with validation.c to verify 1/y^2 scaling\n\n");
}

void compare_deflations() {
    printf("\n=== Comparing Different Deflations ===\n");
    
    FILE *fp = fopen("deflation_comparison.csv", "w");
    fprintf(fp, "position_um,nu_0.95,nu_0.90,nu_0.85,nu_0.80,nu_0.75\n");
    
    double nu_values[] = {0.95, 0.90, 0.85, 0.80, 0.75};
    int n_nu = 5;
    double vesicle_r0 = VESICLE_RADIUS;
    
    for (double y = 2.0 * vesicle_r0; y < CHANNEL_WIDTH - 2.0 * vesicle_r0; y += vesicle_r0) {
        fprintf(fp, "%.3f", y * 1e6);
        
        for (int i = 0; i < n_nu; i++) {
            double v = migration_velocity_poiseuille(y, CHANNEL_WIDTH, MAX_FLOW_VELOCITY, 
                                                     vesicle_r0, LAMBDA, nu_values[i], DELTA);
            fprintf(fp, ",%.6e", v * 1e6);
        }
        fprintf(fp, "\n");
    }
    
    fclose(fp);
    printf("Deflation comparison saved to: deflation_comparison.csv\n");
    printf("Shows how deflation affects migration velocity (Figure 4 from paper)\n\n");
}