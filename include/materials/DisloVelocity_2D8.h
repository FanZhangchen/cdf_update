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

class DisloVelocity_2D8 : public DerivativeMaterialInterface<Material>
{
public:
  DisloVelocity_2D8(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpProperties() override;

  virtual void initQpStatefulProperties();

  const unsigned int _nss;

  std::vector<Real> _gssT;

  const Real _boltzmann;

  const Real _abstemp;

  const Real _p;

  const Real _q;

  const Real _tau0hat;

  const Real _gamma0dot;

  const Real _F0;

  const Real _lambda;

  const Real _mu;

  const Real _taualpha;

  const Real _burgersvector;

  const Real _scale;

private:
  /// member variable to hold the computed diffusivity coefficient
  MaterialProperty<std::vector<Real>> & _dislo_velocity;
  /// member variable to hold the computed convection velocity gradient term
  const MaterialProperty<std::vector<Real>> & _velocity_old;

  const VariableValue & _edge_dislo_den_1;

  const VariableGradient & _grad_edge_dislo_den_1;

  const VariableValue & _edge_dislo_den_2;

  const VariableGradient & _grad_edge_dislo_den_2;

  const VariableValue & _edge_dislo_den_3;

  const VariableGradient & _grad_edge_dislo_den_3;

  const VariableValue & _edge_dislo_den_4;

  const VariableGradient & _grad_edge_dislo_den_4;

  const VariableValue & _screw_dislo_den_1;

  const VariableGradient & _grad_screw_dislo_den_1;

  const VariableValue & _screw_dislo_den_2;

  const VariableGradient & _grad_screw_dislo_den_2;

  const VariableValue & _screw_dislo_den_3;

  const VariableGradient & _grad_screw_dislo_den_3;

  const VariableValue & _screw_dislo_den_4;

  const VariableGradient & _grad_screw_dislo_den_4;

  // Real _initial_velocity;

  MaterialProperty<Real> & _slip_rate;

  MaterialProperty<Real> & _rho_edge;

  MaterialProperty<Real> & _rho_screw;

  MaterialProperty<Real> & _rhot;

  MaterialProperty<Real> & _tau_backstress;
};
