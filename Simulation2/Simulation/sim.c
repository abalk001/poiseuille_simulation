#include "header.h"

void run_regression_checks() {
    printf("\n=== Running Regression Checks ===\n");
    
    double vesicle_r0 = VESICLE_RADIUS;
    double const_shear = 10.0;
    int passed = 1;
    
    // Test 1: Verify v ~ 1/y^2 scaling in simple shear
    printf("Test 1: Simple shear v ~ 1/y^2 scaling...\n");
    double y1 = 3.0 * vesicle_r0;
    double y2 = 6.0 * vesicle_r0;
    double v1 = migration_velocity_simple_shear(y1, const_shear, vesicle_r0, LAMBDA, NU);
    double v2 = migration_velocity_simple_shear(y2, const_shear, vesicle_r0, LAMBDA, NU);
    
    double expected_ratio = (y2 * y2) / (y1 * y1);  // Should be ~4 for y2=2*y1
    double actual_ratio = v1 / v2;
    double error = fabs(actual_ratio - expected_ratio) / expected_ratio;
    
    if (error < 0.1) {  // 10% tolerance
        printf("  \u2713 PASSED: v1/v2 = %.3f (expected %.3f, error %.1f%%)\n", 
               actual_ratio, expected_ratio, error*100);
    } else {
        printf("  \u2717 FAILED: v1/v2 = %.3f (expected %.3f, error %.1f%%)\n", 
               actual_ratio, expected_ratio, error*100);
        passed = 0;
    }
    
    // Test 2: Verify symmetry about W/2 in Poiseuille flow
    printf("Test 2: Poiseuille flow symmetry about W/2...\n");
    double W = CHANNEL_WIDTH;
    double offset = W * 0.15;  // Test at 15% from each wall
    double y_bottom = offset;
    double y_top = W - offset;
    
    double v_bottom = migration_velocity_poiseuille(y_bottom, W, MAX_FLOW_VELOCITY, 
                                                     vesicle_r0, LAMBDA, NU, DELTA);
    double v_top = migration_velocity_poiseuille(y_top, W, MAX_FLOW_VELOCITY, 
                                                  vesicle_r0, LAMBDA, NU, DELTA);
    
    // Magnitudes should be similar, but signs opposite
    double magnitude_ratio = fabs(v_bottom) / fabs(v_top);
    double sign_check = (v_bottom * v_top < 0) ? 1 : 0;  // Should have opposite signs
    
    if (sign_check && fabs(magnitude_ratio - 1.0) < 0.2) {  // 20% tolerance
        printf("  \u2713 PASSED: Velocities symmetric (|v_bot|/|v_top| = %.3f, opposite signs)\n", 
               magnitude_ratio);
    } else {
        printf("  \u2717 FAILED: Symmetry violated (|v_bot|/|v_top| = %.3f, sign check: %d)\n", 
               magnitude_ratio, sign_check);
        passed = 0;
    }
    
    // Test 3: Verify migration direction (away from nearest wall)
    printf("Test 3: Migration direction (away from nearest wall)...\n");
    double y_near_bottom = W * 0.25;  // Closer to bottom
    double y_near_top = W * 0.75;     // Closer to top
    
    double v_near_bottom = migration_velocity_poiseuille(y_near_bottom, W, MAX_FLOW_VELOCITY,
                                                         vesicle_r0, LAMBDA, NU, DELTA);
    double v_near_top = migration_velocity_poiseuille(y_near_top, W, MAX_FLOW_VELOCITY,
                                                      vesicle_r0, LAMBDA, NU, DELTA);
    
    int dir_check_bottom = (v_near_bottom > 0) ? 1 : 0;  // Should move up (away from bottom)
    int dir_check_top = (v_near_top < 0) ? 1 : 0;        // Should move down (away from top)
    
    if (dir_check_bottom && dir_check_top) {
        printf("  \u2713 PASSED: Migration direction correct (away from nearest wall)\n");
    } else {
        printf("  \u2717 FAILED: Migration direction incorrect\n");
        passed = 0;
    }
    
    if (passed) {
        printf("\n\u2713 All regression checks PASSED\n\n");
    } else {
        printf("\n\u2717 Some regression checks FAILED\n\n");
    }
}

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