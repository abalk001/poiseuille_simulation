#include "header.h"

int main(int argc, char *argv[]) {
    
    // Run regression checks first
    run_regression_checks();
    
    // Run Poiseuille flow simulation
    run_simulation("poiseuille", "poiseuille_trajectory.csv");
    
    // Run simple shear for comparison
    run_simulation("simple_shear", "simple_shear_trajectory.csv");
    
    // Generate validation data
    generate_validation_data("validation_input.csv");
    
    // Compare different deflations
    compare_deflations();
    printf("DONE\n");
    return 0;
}
