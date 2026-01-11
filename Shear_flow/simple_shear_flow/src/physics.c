/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abalk <abalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 12:00:00 by abalk             #+#    #+#             */
/*   Updated: 2026/01/11 12:00:00 by abalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shear.h"

/*
** Computes the dimensionless lift velocity coefficient U(lambda, nu).
** Based on Figure 2 from Coupier et al. (2009) SIMPLE SHEAR experimental data.
**
** CRITICAL: This model is calibrated from SIMPLE SHEAR experiments only
** (Figure 2 of reference). It must NOT be cross-calibrated with Poiseuille
** flow data (Figure 4), as the two flow types have fundamentally different
** lift mechanisms (delta=2 vs delta=1).
**
** Power law model:
**   U(lambda, nu) = A * (1 - nu)^B * g(lambda)
** where:
**   A = 0.554, B = 0.707 (fitted to Figure 2 at lambda=1)
**   g(lambda) = 1/(1 + 0.5*(lambda-1)) for lambda >= 1
**
** PHYSICAL REQUIREMENTS:
** - U(nu=1) = 0: Spheres have zero lift (fore-aft symmetry preserved)
** - The power law naturally satisfies this (no artificial boundary condition)
**
** LIMITATION: For high lambda (>5), experiments show non-monotonic behavior
** with deflation that this monotonic model does not capture.
**
** @param lambda: viscosity ratio (inner/outer fluid)
** @param nu: reduced volume (1 = sphere, <1 = deflated)
** @return: dimensionless lift coefficient for simple shear
*/
double	lift_coefficient(double lambda, double nu)
{
	double	deflation;
	double	u_base;
	double	lambda_factor;
	double	a_coeff;
	double	b_exp;

	if (nu >= 1.0)
		return (0.0);
	if (nu <= 0.6)
		return (0.20);
	a_coeff = 0.554;
	b_exp = 0.707;
	deflation = 1.0 - nu;
	u_base = a_coeff * pow(deflation, b_exp);
	lambda_factor = 1.0 / (1.0 + 0.5 * (lambda - 1.0));
	if (lambda_factor < 0.5)
		lambda_factor = 0.5;
	return (u_base * lambda_factor);
}

/*
** Computes the migration velocity for simple shear flow.
** Implements the 1/y^2 scaling law: dy/dt = U * gamma * R0^3 / y^2
**
** @param y: distance from wall (m)
** @param gamma: shear rate (s^-1)
** @param v: pointer to vesicle structure
** @return: migration velocity (m/s)
*/
double	migration_velocity(double y, double gamma, t_vesicle *v)
{
	double	u_coeff;
	double	r_cubed;

	if (y <= 0.0)
		return (0.0);
	u_coeff = lift_coefficient(v->lambda, v->nu);
	r_cubed = pow(v->radius, 3.0);
	return (u_coeff * gamma * r_cubed / pow(y, 2.0));
}
