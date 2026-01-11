#include "shear.h"

int main(void)
{
    t_vesicle vesicle;
    t_params params;

    printf("The Poiseuille simulation start simulation\n");
    init_params(&params);
    inti_vesicle(&vesicle);
    print_summary(&vesicle, &params);
    if(run_simulation(&vesicle, &params, "poiseuille_trajectory.csv") < 0)
        return (-1);
    printf("The Poiseuille simulation end simulation\n");
    return (0);
}