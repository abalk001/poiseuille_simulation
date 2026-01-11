#include "poiseuille.h"

/*
** Computes the dimensionless lift velocity coefficient U(lambda, nu).
** This is the POISEUILLE FLOW model, different from simple shear flow.
**
** PHYSICAL REQUIREMENT: U(nu=1) = 0 (spheres have zero lift due to
** fore-aft symmetry - see ProceedingMRS reference).
**
** For Poiseuille flow (delta ≈ 1), we use a linear deflation model:
**   U(lambda, nu) = A * (1 - nu) * g(lambda)
** where:
**   A = 1.68 (amplitude fitted to match experimental data)
**   g(lambda) = correction for viscosity ratio
**
** For lambda = 1: g(1) = 0.5, giving U = 0.84 * (1-nu)
** This ensures U(nu=1) = 0 mathematically, not just as a boundary condition.
**
** Note: For high lambda (>5), experimental data shows non-monotonic
** behavior with deflation that this simple model does not capture.
**
** @param lambda: viscosity ratio (inner/outer fluid)
** @param nu: reduced volume (1 = sphere, <1 = deflated)
** @return: dimensionless lift coefficient for Poiseuille flow
*/
double	lift_coefficient(double lambda, double nu)
{
	double	deflation;
	double	lambda_factor;
	double	a_coeff;

	if (nu >= 1.0)
		return (0.0);
	if (nu <= 0.5)
		nu = 0.5;
	a_coeff = 1.68;
	deflation = 1.0 - nu;
	lambda_factor = 1.0 / (1.0 + 0.5 * lambda);
	return (a_coeff * deflation * lambda_factor);
}

/*
** Computes the local shear rate in Poiseuille flow.
** gamma(y) = |du/dy| = 4 * U_max / W * |2y/W - 1|
**
** From the parabolic velocity profile u(y) = U_max * [1 - (2y/W - 1)^2]
**
** @param y: distance from bottom wall (m)
** @param p: simulation parameters
** @return: local shear rate (s^-1)
*/
double	shear_rate(double y, t_params *p)
{
	double	w;
	double	normalized;

	w = p->channel_width;
	normalized = 2.0 * y / w - 1.0;
	return (4.0 * p->u_max / w * fabs(normalized));
}

/*
** Computes the Poiseuille velocity at position y.
** u(y) = U_max * [1 - (2y/W - 1)^2]
**
** @param y: distance from bottom wall (m)
** @param p: simulation parameters
** @return: flow velocity (m/s)
*/
double	poiseuille_velocity(double y, t_params *p)
{
	double	w;
	double	normalized;

	w = p->channel_width;
	normalized = 2.0 * y / w - 1.0;
	return (p->u_max * (1.0 - normalized * normalized));
}

/*
** Computes migration velocity for Poiseuille flow.
** Implements Eq. (2) from Coupier et al. (2009):
**   dy/dt = U(lambda, nu) * gamma(y) * R0^(delta+1) / (y - R0)^delta
**
** where delta ≈ 1 for Poiseuille flow (experimentally determined).
**
** Accounts for both walls by computing distance to nearest wall.
**
** @param y: distance from bottom wall (m)
** @param v: pointer to vesicle structure
** @param p: pointer to simulation parameters
** @return: migration velocity (m/s), positive = toward center
*/
double	migration_velocity(double y, t_vesicle *v, t_params *p)
{
	double	u_coeff;
	double	gamma;
	double	d_bottom;
	double	d_top;
	double	d_nearest;
	double	r_factor;
	double	velocity;
	int		direction;

	if (y <= v->radius || y >= p->channel_width - v->radius)
		return (0.0);
	u_coeff = lift_coefficient(v->lambda, v->nu);
	gamma = shear_rate(y, p);
	d_bottom = y - v->radius;
	d_top = p->channel_width - y - v->radius;
	if (d_bottom < d_top)
	{
		d_nearest = fmax(d_bottom, MIN_DISTANCE * v->radius);
		direction = 1;
	}
	else
	{
		d_nearest = fmax(d_top, MIN_DISTANCE * v->radius);
		direction = -1;
	}
	r_factor = pow(v->radius, p->delta + 1.0);
	velocity = u_coeff * gamma * r_factor / pow(d_nearest, p->delta);
	return (velocity * direction);
}
