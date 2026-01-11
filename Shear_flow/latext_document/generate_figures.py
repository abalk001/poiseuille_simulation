#!/usr/bin/env python3
"""
Generate all figures for the vesicle migration paper.
Compares simple shear and Poiseuille flow simulations.
"""

import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import numpy as np

# Physical parameters
W = 140.0e-6       # Channel width (m)
R0 = 10.0e-6       # Vesicle radius (m)
LAMBDA = 1.0       # Viscosity ratio
NU = 0.95          # Reduced volume
GAMMA_SHEAR = 10.0 # Constant shear rate for simple shear (s^-1)
U_MAX = 100.0e-6   # Maximum velocity for Poiseuille (m/s)
DELTA = 1.0        # Poiseuille exponent
DT = 0.01          # Time step (s)
TOTAL_TIME = 30.0  # Total simulation time (s)

# ==============================================================================
# LIFT COEFFICIENT MODELS
# ==============================================================================

def lift_coefficient_shear(lam, nu):
    """Simple shear lift coefficient: power law model.
    
    CORRECTED: Uses direct amplitude A=0.554, not cross-calibrated.
    Satisfies U(nu=1) = 0 mathematically via power law.
    """
    if nu >= 1.0:
        return 0.0
    if nu <= 0.6:
        return 0.20
    A = 0.554
    B = 0.707
    deflation = 1.0 - nu
    u_base = A * (deflation ** B)
    # Simple lambda correction for shear (not from Poiseuille data)
    lambda_factor = 1.0 / (1.0 + 0.5 * (lam - 1.0))
    if lambda_factor < 0.5:
        lambda_factor = 0.5
    return u_base * lambda_factor

def lift_coefficient_poiseuille(lam, nu):
    """Poiseuille lift coefficient: linear deflation model.
    
    CORRECTED: No base term - ensures U(nu=1) = 0 mathematically,
    not just as a boundary condition.
    U = A * (1-nu) * g(lambda)
    """
    if nu >= 1.0:
        return 0.0
    if nu <= 0.5:
        nu = 0.5
    A = 1.68
    deflation = 1.0 - nu
    lambda_factor = 1.0 / (1.0 + 0.5 * lam)
    return A * deflation * lambda_factor

# ==============================================================================
# PHYSICS FUNCTIONS
# ==============================================================================

def shear_rate_poiseuille(y, W, U_max):
    """Position-dependent shear rate in Poiseuille flow."""
    normalized = 2.0 * y / W - 1.0
    return 4.0 * U_max / W * abs(normalized)

def migration_velocity_shear(y, gamma, R0, lam, nu):
    """Migration velocity in simple shear flow."""
    if y <= R0:
        return 0.0
    U = lift_coefficient_shear(lam, nu)
    return U * gamma * (R0**3) / (y**2)

def migration_velocity_poiseuille(y, W, U_max, R0, lam, nu, delta):
    """Migration velocity in Poiseuille flow."""
    if y <= R0 or y >= W - R0:
        return 0.0
    
    U = lift_coefficient_poiseuille(lam, nu)
    gamma = shear_rate_poiseuille(y, W, U_max)
    
    d_bottom = y - R0
    d_top = W - y - R0
    d_nearest = min(d_bottom, d_top)
    d_reg = max(d_nearest, 0.1 * R0)
    
    numerator = U * gamma * (R0 ** (delta + 1.0))
    denominator = d_reg ** delta
    v_magnitude = numerator / denominator
    
    return v_magnitude if d_bottom < d_top else -v_magnitude

# ==============================================================================
# SIMULATION (RK4)
# ==============================================================================

def simulate_shear(y0, dt, total_time, gamma, R0, lam, nu):
    """Simulate simple shear flow migration using RK4."""
    steps = int(total_time / dt)
    times = np.zeros(steps)
    positions = np.zeros(steps)
    velocities = np.zeros(steps)
    
    y = y0
    for i in range(steps):
        times[i] = i * dt
        positions[i] = y
        velocities[i] = migration_velocity_shear(y, gamma, R0, lam, nu)
        
        # RK4
        k1 = migration_velocity_shear(y, gamma, R0, lam, nu)
        k2 = migration_velocity_shear(y + 0.5*dt*k1, gamma, R0, lam, nu)
        k3 = migration_velocity_shear(y + 0.5*dt*k2, gamma, R0, lam, nu)
        k4 = migration_velocity_shear(y + dt*k3, gamma, R0, lam, nu)
        
        y += (dt / 6.0) * (k1 + 2*k2 + 2*k3 + k4)
        y = max(R0, min(y, W - R0))
    
    return times, positions, velocities

def simulate_poiseuille(y0, dt, total_time, W, U_max, R0, lam, nu, delta):
    """Simulate Poiseuille flow migration using RK4."""
    steps = int(total_time / dt)
    times = np.zeros(steps)
    positions = np.zeros(steps)
    velocities = np.zeros(steps)
    shear_rates = np.zeros(steps)
    
    y = y0
    for i in range(steps):
        times[i] = i * dt
        positions[i] = y
        velocities[i] = migration_velocity_poiseuille(y, W, U_max, R0, lam, nu, delta)
        shear_rates[i] = shear_rate_poiseuille(y, W, U_max)
        
        # RK4
        k1 = migration_velocity_poiseuille(y, W, U_max, R0, lam, nu, delta)
        k2 = migration_velocity_poiseuille(y + 0.5*dt*k1, W, U_max, R0, lam, nu, delta)
        k3 = migration_velocity_poiseuille(y + 0.5*dt*k2, W, U_max, R0, lam, nu, delta)
        k4 = migration_velocity_poiseuille(y + dt*k3, W, U_max, R0, lam, nu, delta)
        
        y += (dt / 6.0) * (k1 + 2*k2 + 2*k3 + k4)
        y = max(R0, min(y, W - R0))
    
    return times, positions, velocities, shear_rates

# ==============================================================================
# FIGURE 1: MIGRATION TRAJECTORIES
# ==============================================================================

def generate_migration_trajectories():
    """Generate figure comparing migration trajectories."""
    print("Generating migration_trajectories.png...")
    
    y0 = W * 0.2  # Initial position = 28 um
    
    # Run simulations
    t_s, y_s, v_s = simulate_shear(y0, DT, TOTAL_TIME, GAMMA_SHEAR, R0, LAMBDA, NU)
    t_p, y_p, v_p, gamma_p = simulate_poiseuille(y0, DT, TOTAL_TIME, W, U_MAX, R0, LAMBDA, NU, DELTA)
    
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # (a) Position vs time
    ax = axes[0, 0]
    ax.plot(t_s, y_s * 1e6, 'b-', linewidth=2, label='Simple Shear')
    ax.plot(t_p, y_p * 1e6, 'r--', linewidth=2, label='Poiseuille')
    ax.axhline(y=W*1e6/2, color='k', linestyle=':', alpha=0.5, label='Centerline')
    ax.set_xlabel('Time (s)', fontsize=12)
    ax.set_ylabel('Position y (μm)', fontsize=12)
    ax.set_title('(a) Migration Trajectory', fontsize=12)
    ax.legend(loc='lower right')
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, TOTAL_TIME)
    ax.set_ylim(0, W*1e6)
    
    # (b) Migration velocity vs position
    ax = axes[0, 1]
    # Sample positions
    positions = np.linspace(1.5*R0, W/2, 100)
    v_shear = [migration_velocity_shear(y, GAMMA_SHEAR, R0, LAMBDA, NU) * 1e6 for y in positions]
    v_pois = [abs(migration_velocity_poiseuille(y, W, U_MAX, R0, LAMBDA, NU, DELTA)) * 1e6 for y in positions]
    ax.plot(positions * 1e6, v_shear, 'b-', linewidth=2, label='Simple Shear')
    ax.plot(positions * 1e6, v_pois, 'r--', linewidth=2, label='Poiseuille')
    ax.set_xlabel('Position y (μm)', fontsize=12)
    ax.set_ylabel('Migration Velocity (μm/s)', fontsize=12)
    ax.set_title('(b) Velocity vs Position', fontsize=12)
    ax.legend(loc='upper right')
    ax.grid(True, alpha=0.3)
    
    # (c) Normalized position vs time
    ax = axes[1, 0]
    ax.plot(t_s, y_s / W, 'b-', linewidth=2, label='Simple Shear')
    ax.plot(t_p, y_p / W, 'r--', linewidth=2, label='Poiseuille')
    ax.axhline(y=0.5, color='k', linestyle=':', alpha=0.5)
    ax.set_xlabel('Time (s)', fontsize=12)
    ax.set_ylabel('Normalized Position y/W', fontsize=12)
    ax.set_title('(c) Normalized Trajectory', fontsize=12)
    ax.legend(loc='lower right')
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, TOTAL_TIME)
    ax.set_ylim(0, 0.6)
    
    # (d) Shear rate profiles
    ax = axes[1, 1]
    y_range = np.linspace(0, W, 200)
    gamma_profile = [shear_rate_poiseuille(y, W, U_MAX) for y in y_range]
    ax.plot(y_range * 1e6, gamma_profile, 'r-', linewidth=2, label='Poiseuille γ(y)')
    ax.axhline(y=GAMMA_SHEAR, color='b', linestyle='--', linewidth=2, label=f'Simple Shear γ={GAMMA_SHEAR} s⁻¹')
    ax.set_xlabel('Position y (μm)', fontsize=12)
    ax.set_ylabel('Shear Rate (s⁻¹)', fontsize=12)
    ax.set_title('(d) Shear Rate Profile', fontsize=12)
    ax.legend(loc='upper right')
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, W*1e6)
    
    plt.tight_layout()
    plt.savefig('migration_trajectories.png', dpi=150, bbox_inches='tight')
    plt.close()
    print("  Done!")

# ==============================================================================
# FIGURE 2: SCALING LAW VALIDATION
# ==============================================================================

def generate_scaling_law_validation():
    """Generate figure validating scaling laws for both flows."""
    print("Generating scaling_law_validation.png...")
    
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    
    # Simple Shear: 1/y^2 scaling
    ax = axes[0]
    y_range = np.linspace(1.5*R0, W/3, 50)
    v_shear = np.array([migration_velocity_shear(y, GAMMA_SHEAR, R0, LAMBDA, NU) for y in y_range])
    
    log_y = np.log(y_range * 1e6)
    log_v = np.log(v_shear * 1e9)
    
    # Linear fit
    coeffs = np.polyfit(log_y, log_v, 1)
    slope = coeffs[0]
    fit_line = np.poly1d(coeffs)(log_y)
    
    ax.scatter(log_y, log_v, c='blue', s=30, alpha=0.7, label='Simulation')
    ax.plot(log_y, fit_line, 'r--', linewidth=2, label=f'Fit: slope = {slope:.4f}')
    ax.set_xlabel('log(y) [μm]', fontsize=12)
    ax.set_ylabel('log(v) [nm/s]', fontsize=12)
    ax.set_title(f'Simple Shear: Expected δ=2, Measured={-slope:.4f}', fontsize=12)
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # Poiseuille: 1/d scaling (normalized by gamma)
    ax = axes[1]
    y_range = np.linspace(2*R0, W/3, 50)
    v_pois = []
    d_values = []
    for y in y_range:
        v = abs(migration_velocity_poiseuille(y, W, U_MAX, R0, LAMBDA, NU, DELTA))
        gamma = shear_rate_poiseuille(y, W, U_MAX)
        if v > 0 and gamma > 0:
            v_pois.append(v / gamma)
            d_values.append(y - R0)
    
    v_pois = np.array(v_pois)
    d_values = np.array(d_values)
    
    log_d = np.log(d_values * 1e6)
    log_v_norm = np.log(v_pois * 1e6)
    
    # Linear fit
    coeffs = np.polyfit(log_d, log_v_norm, 1)
    slope = coeffs[0]
    fit_line = np.poly1d(coeffs)(log_d)
    
    ax.scatter(log_d, log_v_norm, c='red', s=30, alpha=0.7, label='Simulation (v/γ)')
    ax.plot(log_d, fit_line, 'b--', linewidth=2, label=f'Fit: slope = {slope:.4f}')
    ax.set_xlabel('log(d) [μm]', fontsize=12)
    ax.set_ylabel('log(v/γ) [μm]', fontsize=12)
    ax.set_title(f'Poiseuille: Expected δ=1, Measured={-slope:.4f}', fontsize=12)
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('scaling_law_validation.png', dpi=150, bbox_inches='tight')
    plt.close()
    print("  Done!")

# ==============================================================================
# FIGURE 3: DEFLATION EFFECT
# ==============================================================================

def generate_deflation_effect():
    """Generate figure showing effect of deflation on migration."""
    print("Generating deflation_effect.png...")
    
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    
    nu_values = [0.95, 0.90, 0.85, 0.80]
    colors = ['blue', 'green', 'orange', 'red']
    
    # (a) Lift coefficient vs deflation
    ax = axes[0]
    nu_range = np.linspace(0.70, 0.99, 100)
    U_shear = [lift_coefficient_shear(LAMBDA, nu) for nu in nu_range]
    U_pois = [lift_coefficient_poiseuille(LAMBDA, nu) for nu in nu_range]
    
    ax.plot(nu_range, U_shear, 'b-', linewidth=2, label='Simple Shear')
    ax.plot(nu_range, U_pois, 'r--', linewidth=2, label='Poiseuille')
    ax.set_xlabel('Reduced Volume ν', fontsize=12)
    ax.set_ylabel('Lift Coefficient U', fontsize=12)
    ax.set_title('(a) Lift Coefficient vs Deflation', fontsize=12)
    ax.legend()
    ax.grid(True, alpha=0.3)
    ax.invert_xaxis()
    
    # (b) Trajectories for different deflations (Poiseuille)
    ax = axes[1]
    y0 = W * 0.2
    
    for i, nu in enumerate(nu_values):
        t, y, v, _ = simulate_poiseuille(y0, DT, TOTAL_TIME, W, U_MAX, R0, LAMBDA, nu, DELTA)
        ax.plot(t, y * 1e6, color=colors[i], linewidth=2, label=f'ν = {nu}')
    
    ax.axhline(y=W*1e6/2, color='k', linestyle=':', alpha=0.5, label='Centerline')
    ax.set_xlabel('Time (s)', fontsize=12)
    ax.set_ylabel('Position y (μm)', fontsize=12)
    ax.set_title('(b) Migration for Different Deflations (Poiseuille)', fontsize=12)
    ax.legend(loc='lower right')
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, TOTAL_TIME)
    ax.set_ylim(20, 80)
    
    plt.tight_layout()
    plt.savefig('deflation_effect.png', dpi=150, bbox_inches='tight')
    plt.close()
    print("  Done!")

# ==============================================================================
# FIGURE 4: LIFT COEFFICIENT VERIFICATION
# ==============================================================================

def generate_lift_coefficient_verification():
    """Generate figure verifying lift coefficient models against data."""
    print("Generating lift_coefficient_verification.png...")
    
    # Experimental data from paper
    exp_data = [
        (0.990, 0.028),
        (0.980, 0.064),
        (0.961, 0.150),
        (0.943, 0.221),
        (0.926, 0.271),
        (0.909, 0.294),
        (0.889, 0.341),
        (0.870, 0.350),
        (0.833, 0.381),
        (0.800, 0.401),
        (0.769, 0.450),
    ]
    exp_nu = np.array([d[0] for d in exp_data])
    exp_scaled = np.array([d[1] for d in exp_data])  # 3U * a2/a1
    
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    
    # (a) Lift coefficient U vs nu
    ax = axes[0]
    nu_range = np.linspace(0.75, 1.0, 100)
    U_shear = np.array([lift_coefficient_shear(LAMBDA, nu) for nu in nu_range])
    U_pois = np.array([lift_coefficient_poiseuille(LAMBDA, nu) for nu in nu_range])
    U_exp = exp_scaled / (3 * exp_nu)
    
    ax.scatter(exp_nu, U_exp, c='black', s=60, marker='s', label='Experimental', zorder=5)
    ax.plot(nu_range, U_shear, 'b-', linewidth=2, label='Shear Model')
    ax.plot(nu_range, U_pois, 'r--', linewidth=2, label='Poiseuille Model')
    ax.set_xlabel('Reduced Volume ν (≈ a₂/a₁)', fontsize=12)
    ax.set_ylabel('Lift Coefficient U', fontsize=12)
    ax.set_title('(a) Lift Coefficient Models', fontsize=12)
    ax.legend()
    ax.grid(True, alpha=0.3)
    ax.invert_xaxis()
    
    # (b) Scaled velocity 3U*nu vs nu
    ax = axes[1]
    scaled_shear = 3 * U_shear * nu_range
    scaled_pois = 3 * U_pois * nu_range
    
    ax.scatter(exp_nu, exp_scaled, c='black', s=60, marker='s', label='Experimental', zorder=5)
    ax.plot(nu_range, scaled_shear, 'b-', linewidth=2, label='Shear Model')
    ax.plot(nu_range, scaled_pois, 'r--', linewidth=2, label='Poiseuille Model')
    ax.set_xlabel('Reduced Volume ν (≈ a₂/a₁)', fontsize=12)
    ax.set_ylabel('3U · a₂/a₁', fontsize=12)
    ax.set_title('(b) Scaled Lift Velocity', fontsize=12)
    ax.legend()
    ax.grid(True, alpha=0.3)
    ax.invert_xaxis()
    
    plt.tight_layout()
    plt.savefig('lift_coefficient_verification.png', dpi=150, bbox_inches='tight')
    plt.close()
    print("  Done!")

# ==============================================================================
# FIGURE 5: MODEL COMPARISON
# ==============================================================================

def generate_model_comparison():
    """Generate figure comparing old vs new models."""
    print("Generating model_comparison.png...")
    
    # Experimental data
    exp_data = [
        (0.990, 0.028),
        (0.980, 0.064),
        (0.961, 0.150),
        (0.943, 0.221),
        (0.926, 0.271),
        (0.909, 0.294),
        (0.889, 0.341),
        (0.870, 0.350),
        (0.833, 0.381),
        (0.800, 0.401),
        (0.769, 0.450),
    ]
    exp_nu = np.array([d[0] for d in exp_data])
    exp_scaled = np.array([d[1] for d in exp_data])
    
    # Old model: U = (1-nu)^0.8
    def old_model(nu):
        if nu >= 1.0:
            return 0.0
        return (1.0 - nu) ** 0.8
    
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    
    # (a) Direct comparison
    ax = axes[0]
    nu_range = np.linspace(0.75, 1.0, 100)
    
    old_scaled = np.array([3 * old_model(nu) * nu for nu in nu_range])
    new_scaled = np.array([3 * lift_coefficient_shear(LAMBDA, nu) * nu for nu in nu_range])
    
    ax.scatter(exp_nu, exp_scaled, c='black', s=60, marker='s', label='Experimental', zorder=5)
    ax.plot(nu_range, old_scaled, 'b--', linewidth=2, label='Old: $(1-ν)^{0.8}$', alpha=0.7)
    ax.plot(nu_range, new_scaled, 'r-', linewidth=2, label='New: Optimized')
    ax.set_xlabel('Reduced Volume ν', fontsize=12)
    ax.set_ylabel('3U · a₂/a₁', fontsize=12)
    ax.set_title('(a) Model Comparison', fontsize=12)
    ax.legend()
    ax.grid(True, alpha=0.3)
    ax.invert_xaxis()
    
    # (b) Relative errors
    ax = axes[1]
    old_pred = np.array([3 * old_model(nu) * nu for nu in exp_nu])
    new_pred = np.array([3 * lift_coefficient_shear(LAMBDA, nu) * nu for nu in exp_nu])
    
    old_err = np.abs(old_pred - exp_scaled) / exp_scaled * 100
    new_err = np.abs(new_pred - exp_scaled) / exp_scaled * 100
    
    x = np.arange(len(exp_nu))
    width = 0.35
    
    ax.bar(x - width/2, old_err, width, label='Old Model', color='blue', alpha=0.7)
    ax.bar(x + width/2, new_err, width, label='New Model', color='red', alpha=0.7)
    ax.set_xlabel('Data Point Index', fontsize=12)
    ax.set_ylabel('Relative Error (%)', fontsize=12)
    ax.set_title('(b) Error Comparison', fontsize=12)
    ax.set_xticks(x)
    ax.set_xticklabels([f'{nu:.2f}' for nu in exp_nu], rotation=45)
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')
    
    # Add RMSE annotations
    rmse_old = np.sqrt(np.mean((old_pred - exp_scaled)**2))
    rmse_new = np.sqrt(np.mean((new_pred - exp_scaled)**2))
    ax.text(0.95, 0.95, f'RMSE Old: {rmse_old:.4f}\nRMSE New: {rmse_new:.4f}',
            transform=ax.transAxes, fontsize=10, verticalalignment='top',
            horizontalalignment='right', bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    plt.tight_layout()
    plt.savefig('model_comparison.png', dpi=150, bbox_inches='tight')
    plt.close()
    print("  Done!")

# ==============================================================================
# MAIN
# ==============================================================================

if __name__ == '__main__':
    print("=" * 60)
    print("Generating figures for vesicle migration paper")
    print("=" * 60)
    
    generate_migration_trajectories()
    generate_scaling_law_validation()
    generate_deflation_effect()
    generate_lift_coefficient_verification()
    generate_model_comparison()
    
    print("=" * 60)
    print("All figures generated successfully!")
    print("=" * 60)
