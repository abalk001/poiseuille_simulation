# Configuration Files for Simple Shear Flow Simulation

This directory contains pre-configured parameter sets for reproducing results from Coupier et al. (2009) and testing different physical scenarios.

## Available Configurations

### 1. `default.conf`
**Standard experimental conditions**
- Vesicle radius: 10 μm
- Viscosity ratio λ: 1.0 (equal viscosities)
- Reduced volume ν: 0.95 (nearly spherical)
- Channel width: 140 μm
- Shear rate: 10 s⁻¹

### 2. `high_deflation.conf`
**Tests effect of strong vesicle deflation**
- Based on paper Figure 2
- ν = 0.75 (strongly deflated)
- Larger vesicle (15 μm)
- Wider channel (200 μm)
- Predicts faster migration

### 3. `viscosity_contrast.conf`
**Tests effect of viscosity ratio**
- Based on paper Figure 4
- λ = 5.0 (inner fluid 5× more viscous)
- Higher shear rate (15 s⁻¹)
- Smaller time step for stability

### 4. `low_shear.conf`
**Low flow rate conditions**
- Shear rate: 2 s⁻¹
- Longer simulation time (60s)
- Tests migration in weak flows

### 5. `paper_fig2.conf`
**Reproduces paper Figure 2 conditions**
- R₀ = 15 μm
- W = 200 μm
- λ = 1.0, ν = 0.80
- Direct comparison with published data

## Usage

```bash
# Run with specific configuration
./simple_shear configs/high_deflation.conf

# Test all configurations
make test-configs

# See available configs
make help
```

## Configuration File Format

```ini
# Comment lines start with #
# Format: KEY = VALUE
# All distances in micrometers (μm)

VESICLE_RADIUS = 10.0    # Effective radius (μm)
LAMBDA = 1.0             # Viscosity ratio (inner/outer)
NU = 0.95                # Reduced volume (1=sphere, <1=deflated)
CHANNEL_WIDTH = 140.0    # Channel width (μm)
SHEAR_RATE = 10.0        # Shear rate (s⁻¹)
DT = 0.01                # Time step (s)
TOTAL_TIME = 30.0        # Total simulation time (s)
```

## Creating Custom Configurations

1. Copy an existing `.conf` file
2. Modify parameters as needed
3. Run: `./simple_shear configs/your_config.conf`

## Parameter Ranges from Paper

| Parameter | Symbol | Range | Unit |
|-----------|--------|-------|------|
| Vesicle radius | R₀ | 10-20 | μm |
| Channel width | W | 140-200 | μm |
| Viscosity ratio | λ | 1-10 | - |
| Reduced volume | ν | 0.6-0.95 | - |
| Shear rate | γ | 1-100 | s⁻¹ |

## Expected Results

- **Higher deflation (lower ν)** → Faster migration
- **Higher λ** → Non-monotonic behavior (see paper Fig. 4)
- **Lower shear rate** → Slower migration (linear scaling)
- All should validate **1/y² scaling law**
