# Simple Shear Flow Vesicle Migration Simulation

## Overview

This C implementation simulates vesicle lateral migration in bounded simple shear flow, based on the theoretical framework of Coupier et al. (2009). The code follows the **42 Norm** coding standard.

## Physics

### Governing Equation

For a vesicle near a wall in simple shear flow, the migration velocity follows:

```
dy/dt = U(λ,ν) · γ · R₀³ / y²
```

Where:
- `y` = distance from wall to vesicle center
- `U(λ,ν)` = dimensionless lift velocity coefficient
- `γ` = shear rate (constant in simple shear)
- `R₀` = vesicle effective radius
- `λ` = viscosity ratio (inner/outer fluid)
- `ν` = reduced volume (deflation, ν=1 for sphere)

### Key Features

- **1/y² scaling law**: Wall-induced lift decays as inverse square of distance
- **Deflation dependence**: More deflated vesicles migrate faster
- **RK4 integration**: 4th-order Runge-Kutta for accurate time stepping

## File Structure

```
simple_shear_flow/
├── Makefile
├── README.md
├── include/
│   └── shear.h          # Header with structs and prototypes
└── src/
    ├── main.c           # Entry point
    ├── physics.c        # Lift coefficient and migration velocity
    ├── simulation.c     # RK4 integration loop
    ├── validation.c     # Scaling law verification
    ├── utils.c          # Initialization and validation
    └── output.c         # CSV file handling
```

## Compilation

```bash
make        # Build the executable
make clean  # Remove object files
make fclean # Remove all generated files
make re     # Clean rebuild
make run    # Build and run
```

## Usage

### Basic Usage

```bash
# Run with default parameters
./simple_shear

# Run with custom configuration
./simple_shear configs/high_deflation.conf

# See available configs
make help
```

### Available Configurations

The simulation includes pre-configured parameter sets from the paper:

- **`configs/default.conf`** - Standard conditions (R₀=10μm, λ=1, ν=0.95)
- **`configs/high_deflation.conf`** - Strongly deflated vesicle (ν=0.75)
- **`configs/viscosity_contrast.conf`** - High viscosity ratio (λ=5.0)
- **`configs/low_shear.conf`** - Low shear rate (γ=2 s⁻¹)
- **`configs/paper_fig2.conf`** - Reproduces paper Figure 2 conditions

See [`configs/README.md`](configs/README.md) for detailed descriptions.

### Output Files

- `simple_shear_trajectory.csv` - Vesicle trajectory over time
- `validation_input.csv` - Position-velocity pairs for scaling validation

### Output Format

```
time,position_um,velocity_um_s,shear_rate,y_normalized
0.000,28.000000,1.234567e-02,10.000000,0.200000
...
```

## Parameters

| Parameter | Symbol | Default Value | Unit |
|-----------|--------|---------------|------|
| Channel width | W | 140 | μm |
| Vesicle radius | R₀ | 10 | μm |
| Shear rate | γ | 10 | s⁻¹ |
| Viscosity ratio | λ | 1.0 | - |
| Reduced volume | ν | 0.95 | - |
| Time step | Δt | 0.01 | s |
| Total time | T | 30 | s |

## 42 Norm Compliance

This code follows the 42 school coding standard:
- ✓ Max 25 lines per function
- ✓ Max 5 functions per file
- ✓ Max 4 parameters per function
- ✓ Only `while` loops (no `for`)
- ✓ Variables declared at function start
- ✓ 80 character line limit
- ✓ Proper header format
- ✓ Tab indentation

## Validation

The program automatically validates the 1/y² scaling law:

```
=== SCALING LAW VALIDATION ===
Target exponent (theory): -2.00
Measured exponent: -2.0000
Result: PASS
```

## References

1. Coupier, G., et al. (2009). "Lateral migration and segregation of vesicles 
   with viscosity contrast in simple shear and Poiseuille flows." 
   Mater. Res. Soc. Symp. Proc. Vol. 1132.

## Author

Ahmed Balk - 42 Student
