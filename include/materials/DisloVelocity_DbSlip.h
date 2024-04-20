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

class DisloVelocity_DbSlip : public DerivativeMaterialInterface<Material>
{
public:
  DisloVelocity_DbSlip(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpProperties() override;

  virtual void initQpStatefulProperties();

  const unsigned int _nss;

  std::vector<Real> _gssT;

  const Real _abstemp;

  const Real _boltzmann;

  const Real _burgers;

  const Real _f0;

  const Real _gdot0;

  const Real _dlamb;

  const Real _shear_modulus;

  const Real _p;

  const Real _q;

  const Real _tau0hat;

  const Real _taualpha;

  const Real _w1;

  const Real _w2;

private:
  /// member variable to hold the computed diffusivity coefficient
  MaterialProperty<std::vector<Real>> & _dislo_velocity;
  /// member variable to hold the computed convection velocity gradient term
  const MaterialProperty<std::vector<Real>> & _velocity_old;

  const VariableValue & _edge_dislo_den_pos_1;

  const VariableGradient & _grad_edge_dislo_den_pos_1;

  const VariableValue & _edge_dislo_den_neg_1;

  const VariableGradient & _grad_edge_dislo_den_neg_1;

  const VariableValue & _edge_dislo_den_pos_2;

  const VariableGradient & _grad_edge_dislo_den_pos_2;

  const VariableValue & _edge_dislo_den_neg_2;

  const VariableGradient & _grad_edge_dislo_den_neg_2;

  // Real _initial_velocity;

  MaterialProperty<Real> & _slip_rate;

  MaterialProperty<std::vector<Real>> & _slip_resistance;

  MaterialProperty<std::vector<Real>> & _total_dislo_den;

  MaterialProperty<Real> & _rhot;

  MaterialProperty<Real> & _tau_backstress;

  const Real _scale;
};
