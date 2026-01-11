#include "header.h"

int main(void)
{
    run_regression_checks();
    run_simulation("poiseuille", "poiseuille_trajectory.csv");
    run_simulation("simple_shear", "simple_shear_trajectory.csv");
    generate_validation_data("validation_input.csv");
    compare_deflations();
    printf("DONE\n");
    return 0;
}
