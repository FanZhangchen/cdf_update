//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Material.h"
#include "LinearInterpolation.h"
#include "DerivativeMaterialInterface.h"

class DisloVelocity_2D4 : public DerivativeMaterialInterface<Material>
{
public:
  DisloVelocity_2D4(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpProperties() override;

  virtual void initQpStatefulProperties();

  const unsigned int _nss;

  std::vector<Real> _gssT;

  const Real _abstemp;

  const Real _boltzmann;

  const Real _burgersvector;

  const Real _F0;

  const Real _gamma0dot;

  const Real _lambda;

  const Real _mu;

  const Real _p;

  const Real _q;

  const Real _tau0hat;

  const Real _taualpha;

private:
  /// member variable to hold the computed diffusivity coefficient
  MaterialProperty<std::vector<Real>> & _dislo_velocity;
  /// member variable to hold the computed convection velocity gradient term
  const MaterialProperty<std::vector<Real>> & _velocity_old;

  const VariableValue & _rhoep;

  const VariableGradient & _grad_rhoep;

  const VariableValue & _rhoen;

  const VariableGradient & _grad_rhoen;

  const VariableValue & _rhosp;

  const VariableGradient & _grad_rhosp;

  const VariableValue & _rhosn;

  const VariableGradient & _grad_rhosn;

  // Real _initial_velocity;

  MaterialProperty<Real> & _slip_rate;

  MaterialProperty<Real> & _rho_edge;

  MaterialProperty<Real> & _rho_screw;

  MaterialProperty<Real> & _rhot;

  MaterialProperty<Real> & _tau_backstress;

  const Real _scale;
};
