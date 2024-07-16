//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the Darcy velocity given
 * several fluid properties and the pressure gradient.
 */
class TotalDislocationDensitySingleSlipSys : public AuxKernel
{
public:
  static InputParameters validParams();

  TotalDislocationDensitySingleSlipSys(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual Real computeValue() override;

  const unsigned int _nss;

  std::vector<Real> _gssT;

  ///@{Nodal degrees of freedom
  const VariableValue & _edge_dislo_den_1_Q1;
  const VariableGradient & _grad_edge_dislo_den_1_Q1;
  const VariableValue & _edge_dislo_den_1_Q2;
  const VariableGradient & _grad_edge_dislo_den_1_Q2;
  const VariableValue & _edge_dislo_den_1_Q3;
  const VariableGradient & _grad_edge_dislo_den_1_Q3;
  const VariableValue & _edge_dislo_den_1_Q4;
  const VariableGradient & _grad_edge_dislo_den_1_Q4;
  const VariableValue & _screw_dislo_den_1_Q1;
  const VariableGradient & _grad_screw_dislo_den_1_Q1;
  const VariableValue & _screw_dislo_den_1_Q2;
  const VariableGradient & _grad_screw_dislo_den_1_Q2;
  const VariableValue & _screw_dislo_den_1_Q3;
  const VariableGradient & _grad_screw_dislo_den_1_Q3;
  const VariableValue & _screw_dislo_den_1_Q4;
  const VariableGradient & _grad_screw_dislo_den_1_Q4;

  ///@}

  const Real _initial_total_dislo_den;
};
