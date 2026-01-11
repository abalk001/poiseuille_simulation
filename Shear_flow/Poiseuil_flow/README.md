# Poiseuille Flow Vesicle Migration Simulation

Numerical simulation of vesicle lateral migration in Poiseuille flow, based on 
the theoretical framework from Coupier et al. (2009).

## Physics

The governing equation for Poiseuille flow migration is:

```
dy/dt = U(λ, ν) × γ(y) × R₀^(δ+1) / (y - R₀)^δ
```

Where:
- `y` = distance from wall
- `U(λ, ν)` = lift coefficient (fitted from experimental data)
- `γ(y) = 4×U_max/W × |2y/W - 1|` = position-dependent shear rate
- `R₀` = vesicle radius
- `δ ≈ 1` = Poiseuille exponent

### Key Differences from Simple Shear

| Property | Simple Shear | Poiseuille |
|----------|--------------|------------|
| Shear rate | Constant γ | γ(y) varies with position |
| Scaling | 1/y² | 1/(y-R₀)^δ with δ≈1 |
| Wall effects | Single wall | Both walls contribute |

## Building

```bash
make        # Build the simulation
make clean  # Remove object files
make fclean # Remove all generated files
make re     # Rebuild from scratch
```

## Running

```bash
./poiseuille                    # Run with default parameters
./poiseuille configs/default.conf   # Run with config file
```

## Output Files

- `poiseuille_trajectory.csv` - Time series of vesicle position
- `poiseuille_validation.csv` - Position vs velocity data for scaling law check

## Comparison with Simple Shear

The simulation automatically compares results with simple shear flow data if available:

```bash
cd ../simple_shear_flow && make run  # Generate shear data first
cd ../Poiseuil_flow && make run      # Then run Poiseuille
```

## Configuration Parameters

| Parameter | Default | Unit | Description |
|-----------|---------|------|-------------|
| CHANNEL_WIDTH | 140 | µm | Channel width |
| VESICLE_RADIUS | 10 | µm | Vesicle radius |
| U_MAX | 100 | µm/s | Maximum flow velocity |
| LAMBDA | 1.0 | - | Viscosity ratio |
| NU | 0.95 | - | Reduced volume |
| DELTA | 1.0 | - | Poiseuille exponent |
| DT | 0.01 | s | Time step |
| TOTAL_TIME | 30 | s | Simulation duration |

## References

1. Coupier, G., et al. (2009). "Noninertial lateral migration of vesicles in 
   bounded Poiseuille flow." Physics of Fluids.
2. Olla, P. (1997). "The lift on a tank-treading ellipsoidal cell in a shear 
   flow." J. Phys. II France.
