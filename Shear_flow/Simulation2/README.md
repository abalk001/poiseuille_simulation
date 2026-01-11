# Vesicle Migration in Poiseuille Flow Simulation

## Overview

This simulation models the lateral migration of deformable vesicles (membrane-enclosed particles) in microfluidic channel flows. The code implements a physics-based model of hydrodynamic lift forces that cause vesicles to migrate across streamlines in shear flows, particularly relevant to blood flow, microfluidics, and lab-on-a-chip applications.

## Physical Principle

### Background

Vesicles are closed lipid bilayer membranes enclosing an internal fluid. When suspended in flow, they experience:

1. **Advection** by the background flow field
2. **Lateral migration** perpendicular to the flow direction due to hydrodynamic lift forces

Unlike rigid particles, vesicles are deformable and develop asymmetric shapes in shear flow, creating lift forces that drive cross-streamline migration.

### Key Physics

#### Hydrodynamic Lift Force

In a shear flow, vesicles experience a **lift force** directed away from nearby walls. This force arises from:

- **Shape asymmetry**: Shear deforms the vesicle into a non-spherical shape
- **Viscosity contrast**: Different viscosities inside (η_in) and outside (η_out) the vesicle
- **Wall interactions**: Hydrodynamic interactions with channel walls create repulsive effects
- **Deflation**: Under-inflated vesicles (ν < 1) deform more, enhancing lift

The **reduced volume** ν = V/(4πR³/3) characterizes vesicle deflation:
- ν = 1: Spherical vesicle (no excess membrane)
- ν < 1: Deflated vesicle (excess membrane allows deformation)

The **viscosity ratio** λ = η_in/η_out affects deformability:
- λ = 1: Matched viscosities (typical for lipid vesicles)
- λ > 1: More viscous interior (stiffer response)
- λ < 1: Less viscous interior (more deformable)

### Poiseuille Flow

Poiseuille flow is the parabolic velocity profile in a channel:

```
u(y) = U_max [1 - (2y/W - 1)²]
```

where:
- y: Distance from bottom wall
- W: Channel width
- U_max: Maximum velocity at centerline (y = W/2)

This creates a **shear rate**:

```
γ(y) = |du/dy| = (4U_max/W)|2y/W - 1|
```

The shear rate is:
- Zero at the centerline (y = W/2)
- Maximum at the walls (y = 0, y = W)

## Mathematical Model

### Migration Velocity

The lateral migration velocity perpendicular to the flow is given by:

```
v_migration(y) = U · γ(y) · r₀^(δ+1) / d^δ
```

where:
- **U**: Lift velocity coefficient (depends on λ and ν)
- **γ(y)**: Local shear rate
- **r₀**: Vesicle effective radius
- **d**: Distance from nearest wall
- **δ**: Scaling exponent (≈ 1.0 from experiments)

#### Distance Calculation

The distance to the nearest wall is:

```
d = min(y - r₀, (W - y) - r₀)
```

This ensures symmetric treatment: vesicles experience lift from whichever wall is closer.

#### Direction Rule

Migration direction is determined by the **nearest wall principle**:

```
if (d_bottom < d_top):
    v_migration > 0  (move away from bottom wall, upward)
else:
    v_migration < 0  (move away from top wall, downward)
```

This creates a natural focusing effect toward the channel centerline.

#### Lift Coefficient

The lift coefficient U depends on both λ and ν:

```
U(λ, ν) = U_base(λ) + U_deflation(λ, ν)
```

where:
- U_base ≈ 0.15 for λ = 1 (from experimental fits)
- U_deflation increases with deflation: (1 - ν)/(1 - ν_min)
- Both terms scale with λ

Empirical form:
```
U(λ, ν) = 0.15 · f(λ) · [1 + (1-ν)/(1-0.6)]
```

where f(λ) accounts for viscosity ratio effects.

### Simple Shear Flow (Reference Case)

For uniform shear γ (constant across channel), the model simplifies to:

```
v_migration(y) = U · γ · r₀³ / y²
```

This exhibits the classic **1/y² scaling** that serves as a validation benchmark.

### Regularization

To avoid numerical singularities when d → 0:

```
d_regularized = max(d, 0.1 · r₀)
```

This provides a smooth "soft wall" effect rather than hard cutoffs.

## Algorithm

### Numerical Integration Scheme

The simulation uses **4th-order Runge-Kutta (RK4)** integration for solving:

```
dy/dt = v_migration(y)
```

#### RK4 Algorithm

```
k₁ = v_migration(y)
k₂ = v_migration(y + Δt·k₁/2)
k₃ = v_migration(y + Δt·k₂/2)
k₄ = v_migration(y + Δt·k₃)

y_new = y_old + (Δt/6)·(k₁ + 2k₂ + 2k₃ + k₄)
```

RK4 provides:
- **4th-order accuracy**: Error ~ O(Δt⁵)
- **Stability**: Better than Euler method for stiff problems
- **Smoothness**: Accurate trajectory evolution

### Stability Safeguards

1. **Displacement Limiting**:
   ```
   if |Δy| > 0.1·r₀:
       Δy = sign(Δy) · 0.1·r₀
       warn_user()
   ```

2. **CFL Condition**:
   ```
   CFL = v_max · Δt / r₀
   if CFL > 0.5: warn_user()
   ```

3. **Boundary Enforcement**:
   ```
   if y < r₀: y = r₀
   if y > W - r₀: y = W - r₀
   ```

### Parameter Validation

Before simulation, the code validates:

| Parameter | Condition | Reason |
|-----------|-----------|--------|
| W | W > 2r₀ | Vesicle must fit in channel |
| ν | 0 < ν ≤ 1 | Physical deflation range |
| U_max | U_max > 0 | Positive flow velocity |
| Δt | Δt > 0 | Positive time step |

### Memory Optimization

Instead of storing full trajectory arrays, the code:

1. **Streams output** every OUTPUT_INTERVAL steps directly to CSV
2. **Allocates** only current state variables (y, v, t)
3. **Reduces memory** from O(N_steps) to O(1)

This enables long simulations without memory overflow.

### Regression Testing

The code includes automated validation checks:

#### Test 1: Simple Shear Scaling
Verifies v ∝ 1/y² in uniform shear:
```
v(y₁)/v(y₂) ≈ (y₂/y₁)²
```

#### Test 2: Poiseuille Symmetry
Checks mirror symmetry about centerline:
```
|v(y₀)| ≈ |v(W - y₀)|
sign(v(y₀)) = -sign(v(W - y₀))
```

#### Test 3: Migration Direction
Ensures vesicles move away from nearest wall:
```
if y < W/2: v > 0
if y > W/2: v < 0
```

## Code Structure

### Files

- **header.h**: Constants, parameters, function declarations
- **poiseuille_simulation.c**: Main entry point
- **tools.c**: Core physics functions (velocities, integration)
- **sim.c**: Validation and comparison utilities
- **Makefile**: Build system

### Key Functions

#### Physics Functions

```c
double migration_velocity_poiseuille(double y, double W, double U_max, 
                                     double r0, double lambda, double nu, double delta)
```
Computes lateral migration velocity in Poiseuille flow using nearest-wall distance and symmetric lift direction.

```c
double lift_velocity_coefficient(double lambda, double nu)
```
Returns the lift coefficient U(λ, ν) based on empirical fits to experimental data.

```c
double poiseuille_velocity(double y, double W, double U_max)
```
Returns streamwise velocity u(y) for parabolic Poiseuille profile.

```c
double poiseuille_shear_rate(double y, double W, double U_max)
```
Returns local shear rate γ(y) = |du/dy|.

#### Simulation Functions

```c
void run_simulation(const char* mode, const char* output_file)
```
Main simulation loop with RK4 integration, stability checks, and streaming output.

```c
int validate_parameters(double W, double r0, double nu, double U_max, double dt)
```
Validates all physical and numerical parameters before simulation.

```c
void run_regression_checks()
```
Executes automated tests to verify model correctness.

## Usage

### Compilation

```bash
make
```

This compiles all source files and creates the `simulation` executable.

### Running the Simulation

```bash
./simulation
```

### Output Files

1. **poiseuille_trajectory.csv**: Vesicle trajectory in Poiseuille flow
   - Columns: time, position_um, velocity_um_s, shear_rate, y_normalized

2. **simple_shear_trajectory.csv**: Trajectory in uniform shear (validation)
   - Same format as above

3. **validation_input.csv**: Data for 1/y² scaling verification
   - Columns: distance_from_wall_um, velocity_um_s, shear_rate

4. **deflation_comparison.csv**: Migration velocity vs. position for different ν values
   - Shows effect of deflation on lift (reproduces experimental results)

### Parameter Adjustment

Edit [header.h](header.h) to modify:

```c
// Physical parameters
#define CHANNEL_WIDTH 140.0e-6    // Channel width (m)
#define VESICLE_RADIUS 10.0e-6    // Vesicle radius (m)
#define LAMBDA 1.0                // Viscosity ratio η_in/η_out
#define NU 0.95                   // Reduced volume
#define MAX_FLOW_VELOCITY 100.0e-6 // Peak velocity (m/s)
#define DELTA 1.0                 // Scaling exponent

// Numerical parameters
#define DT 0.01                   // Time step (s)
#define TOTAL_TIME 30.0           // Simulation duration (s)
#define OUTPUT_INTERVAL 10        // Save every N steps
```

## Physical Insights

### Migration Behavior

1. **Initial Phase**: Vesicle starts near wall (y = 0.2W)
   - High shear rate γ
   - Strong lift force
   - Rapid migration toward centerline

2. **Approach to Centerline**: As y → W/2
   - Shear rate γ → 0
   - Migration velocity v → 0
   - Vesicle approaches equilibrium

3. **Focusing Effect**: 
   - All vesicles converge to centerline
   - Final position ≈ W/2 (independent of initial position)
   - Time to reach center depends on deflation ν

### Deflation Effect

Lower ν (more deflated vesicles):
- **Stronger deformation** in shear
- **Larger lift forces**
- **Faster migration**
- **Better focusing**

This is exploited in microfluidic devices for particle separation.

### Wall Effects

Near walls (d → r₀):
- Lubrication forces dominate
- Regularization prevents singularities
- Soft repulsion (rather than hard wall)
- Physical vesicle deformation limits approach

## Validation Against Experiments

The model reproduces key experimental observations:

1. **Migration toward centerline** in Poiseuille flow ✓
2. **1/y² scaling** in simple shear ✓
3. **Deflation dependence** U ∝ (1-ν) ✓
4. **Symmetry** about channel centerline ✓
5. **Time scales** comparable to experiments (~10-30s for 100 μm channels) ✓

## References

This implementation is based on:

- Coupier, G., et al. "Noninertial lateral migration of vesicles in bounded Poiseuille flow." Physics of Fluids (2008).
- Abkarian, M., et al. "Dynamics of vesicles in a wall-bounded shear flow." Biophysical Journal (2005).
- Vitkova, V., et al. "Hydrodynamics of vesicles: The effect of membrane viscosity." Physical Review E (2007).

## Future Extensions

Possible improvements:
- 3D channel geometry
- Multiple interacting vesicles
- Time-dependent flows (pulsatile)
- Non-parabolic velocity profiles
- Temperature effects
- Membrane mechanics (bending rigidity)


## Authors


---

**Last updated**: January 2026
