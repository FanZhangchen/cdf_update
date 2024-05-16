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
class TotalDislocationDensity : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  TotalDislocationDensity(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

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

  const VariableValue & _edge_dislo_den_2_Q1;
  const VariableGradient & _grad_edge_dislo_den_2_Q1;
  const VariableValue & _edge_dislo_den_2_Q2;
  const VariableGradient & _grad_edge_dislo_den_2_Q2;
  const VariableValue & _edge_dislo_den_2_Q3;
  const VariableGradient & _grad_edge_dislo_den_2_Q3;
  const VariableValue & _edge_dislo_den_2_Q4;
  const VariableGradient & _grad_edge_dislo_den_2_Q4;
  const VariableValue & _screw_dislo_den_2_Q1;
  const VariableGradient & _grad_screw_dislo_den_2_Q1;
  const VariableValue & _screw_dislo_den_2_Q2;
  const VariableGradient & _grad_screw_dislo_den_2_Q2;
  const VariableValue & _screw_dislo_den_2_Q3;
  const VariableGradient & _grad_screw_dislo_den_2_Q3;
  const VariableValue & _screw_dislo_den_2_Q4;
  const VariableGradient & _grad_screw_dislo_den_2_Q4;

  const VariableValue & _edge_dislo_den_3_Q1;
  const VariableGradient & _grad_edge_dislo_den_3_Q1;
  const VariableValue & _edge_dislo_den_3_Q2;
  const VariableGradient & _grad_edge_dislo_den_3_Q2;
  const VariableValue & _edge_dislo_den_3_Q3;
  const VariableGradient & _grad_edge_dislo_den_3_Q3;
  const VariableValue & _edge_dislo_den_3_Q4;
  const VariableGradient & _grad_edge_dislo_den_3_Q4;
  const VariableValue & _screw_dislo_den_3_Q1;
  const VariableGradient & _grad_screw_dislo_den_3_Q1;
  const VariableValue & _screw_dislo_den_3_Q2;
  const VariableGradient & _grad_screw_dislo_den_3_Q2;
  const VariableValue & _screw_dislo_den_3_Q3;
  const VariableGradient & _grad_screw_dislo_den_3_Q3;
  const VariableValue & _screw_dislo_den_3_Q4;
  const VariableGradient & _grad_screw_dislo_den_3_Q4;

  const VariableValue & _edge_dislo_den_4_Q1;
  const VariableGradient & _grad_edge_dislo_den_4_Q1;
  const VariableValue & _edge_dislo_den_4_Q2;
  const VariableGradient & _grad_edge_dislo_den_4_Q2;
  const VariableValue & _edge_dislo_den_4_Q3;
  const VariableGradient & _grad_edge_dislo_den_4_Q3;
  const VariableValue & _edge_dislo_den_4_Q4;
  const VariableGradient & _grad_edge_dislo_den_4_Q4;
  const VariableValue & _screw_dislo_den_4_Q1;
  const VariableGradient & _grad_screw_dislo_den_4_Q1;
  const VariableValue & _screw_dislo_den_4_Q2;
  const VariableGradient & _grad_screw_dislo_den_4_Q2;
  const VariableValue & _screw_dislo_den_4_Q3;
  const VariableGradient & _grad_screw_dislo_den_4_Q3;
  const VariableValue & _screw_dislo_den_4_Q4;
  const VariableGradient & _grad_screw_dislo_den_4_Q4;

  const VariableValue & _edge_dislo_den_5_Q1;
  const VariableGradient & _grad_edge_dislo_den_5_Q1;
  const VariableValue & _edge_dislo_den_5_Q2;
  const VariableGradient & _grad_edge_dislo_den_5_Q2;
  const VariableValue & _edge_dislo_den_5_Q3;
  const VariableGradient & _grad_edge_dislo_den_5_Q3;
  const VariableValue & _edge_dislo_den_5_Q4;
  const VariableGradient & _grad_edge_dislo_den_5_Q4;
  const VariableValue & _screw_dislo_den_5_Q1;
  const VariableGradient & _grad_screw_dislo_den_5_Q1;
  const VariableValue & _screw_dislo_den_5_Q2;
  const VariableGradient & _grad_screw_dislo_den_5_Q2;
  const VariableValue & _screw_dislo_den_5_Q3;
  const VariableGradient & _grad_screw_dislo_den_5_Q3;
  const VariableValue & _screw_dislo_den_5_Q4;
  const VariableGradient & _grad_screw_dislo_den_5_Q4;

  const VariableValue & _edge_dislo_den_6_Q1;
  const VariableGradient & _grad_edge_dislo_den_6_Q1;
  const VariableValue & _edge_dislo_den_6_Q2;
  const VariableGradient & _grad_edge_dislo_den_6_Q2;
  const VariableValue & _edge_dislo_den_6_Q3;
  const VariableGradient & _grad_edge_dislo_den_6_Q3;
  const VariableValue & _edge_dislo_den_6_Q4;
  const VariableGradient & _grad_edge_dislo_den_6_Q4;
  const VariableValue & _screw_dislo_den_6_Q1;
  const VariableGradient & _grad_screw_dislo_den_6_Q1;
  const VariableValue & _screw_dislo_den_6_Q2;
  const VariableGradient & _grad_screw_dislo_den_6_Q2;
  const VariableValue & _screw_dislo_den_6_Q3;
  const VariableGradient & _grad_screw_dislo_den_6_Q3;
  const VariableValue & _screw_dislo_den_6_Q4;
  const VariableGradient & _grad_screw_dislo_den_6_Q4;

  const VariableValue & _edge_dislo_den_7_Q1;
  const VariableGradient & _grad_edge_dislo_den_7_Q1;
  const VariableValue & _edge_dislo_den_7_Q2;
  const VariableGradient & _grad_edge_dislo_den_7_Q2;
  const VariableValue & _edge_dislo_den_7_Q3;
  const VariableGradient & _grad_edge_dislo_den_7_Q3;
  const VariableValue & _edge_dislo_den_7_Q4;
  const VariableGradient & _grad_edge_dislo_den_7_Q4;
  const VariableValue & _screw_dislo_den_7_Q1;
  const VariableGradient & _grad_screw_dislo_den_7_Q1;
  const VariableValue & _screw_dislo_den_7_Q2;
  const VariableGradient & _grad_screw_dislo_den_7_Q2;
  const VariableValue & _screw_dislo_den_7_Q3;
  const VariableGradient & _grad_screw_dislo_den_7_Q3;
  const VariableValue & _screw_dislo_den_7_Q4;
  const VariableGradient & _grad_screw_dislo_den_7_Q4;

  const VariableValue & _edge_dislo_den_8_Q1;
  const VariableGradient & _grad_edge_dislo_den_8_Q1;
  const VariableValue & _edge_dislo_den_8_Q2;
  const VariableGradient & _grad_edge_dislo_den_8_Q2;
  const VariableValue & _edge_dislo_den_8_Q3;
  const VariableGradient & _grad_edge_dislo_den_8_Q3;
  const VariableValue & _edge_dislo_den_8_Q4;
  const VariableGradient & _grad_edge_dislo_den_8_Q4;
  const VariableValue & _screw_dislo_den_8_Q1;
  const VariableGradient & _grad_screw_dislo_den_8_Q1;
  const VariableValue & _screw_dislo_den_8_Q2;
  const VariableGradient & _grad_screw_dislo_den_8_Q2;
  const VariableValue & _screw_dislo_den_8_Q3;
  const VariableGradient & _grad_screw_dislo_den_8_Q3;
  const VariableValue & _screw_dislo_den_8_Q4;
  const VariableGradient & _grad_screw_dislo_den_8_Q4;

  const VariableValue & _edge_dislo_den_9_Q1;
  const VariableGradient & _grad_edge_dislo_den_9_Q1;
  const VariableValue & _edge_dislo_den_9_Q2;
  const VariableGradient & _grad_edge_dislo_den_9_Q2;
  const VariableValue & _edge_dislo_den_9_Q3;
  const VariableGradient & _grad_edge_dislo_den_9_Q3;
  const VariableValue & _edge_dislo_den_9_Q4;
  const VariableGradient & _grad_edge_dislo_den_9_Q4;
  const VariableValue & _screw_dislo_den_9_Q1;
  const VariableGradient & _grad_screw_dislo_den_9_Q1;
  const VariableValue & _screw_dislo_den_9_Q2;
  const VariableGradient & _grad_screw_dislo_den_9_Q2;
  const VariableValue & _screw_dislo_den_9_Q3;
  const VariableGradient & _grad_screw_dislo_den_9_Q3;
  const VariableValue & _screw_dislo_den_9_Q4;
  const VariableGradient & _grad_screw_dislo_den_9_Q4;

  const VariableValue & _edge_dislo_den_10_Q1;
  const VariableGradient & _grad_edge_dislo_den_10_Q1;
  const VariableValue & _edge_dislo_den_10_Q2;
  const VariableGradient & _grad_edge_dislo_den_10_Q2;
  const VariableValue & _edge_dislo_den_10_Q3;
  const VariableGradient & _grad_edge_dislo_den_10_Q3;
  const VariableValue & _edge_dislo_den_10_Q4;
  const VariableGradient & _grad_edge_dislo_den_10_Q4;
  const VariableValue & _screw_dislo_den_10_Q1;
  const VariableGradient & _grad_screw_dislo_den_10_Q1;
  const VariableValue & _screw_dislo_den_10_Q2;
  const VariableGradient & _grad_screw_dislo_den_10_Q2;
  const VariableValue & _screw_dislo_den_10_Q3;
  const VariableGradient & _grad_screw_dislo_den_10_Q3;
  const VariableValue & _screw_dislo_den_10_Q4;
  const VariableGradient & _grad_screw_dislo_den_10_Q4;

  const VariableValue & _edge_dislo_den_11_Q1;
  const VariableGradient & _grad_edge_dislo_den_11_Q1;
  const VariableValue & _edge_dislo_den_11_Q2;
  const VariableGradient & _grad_edge_dislo_den_11_Q2;
  const VariableValue & _edge_dislo_den_11_Q3;
  const VariableGradient & _grad_edge_dislo_den_11_Q3;
  const VariableValue & _edge_dislo_den_11_Q4;
  const VariableGradient & _grad_edge_dislo_den_11_Q4;
  const VariableValue & _screw_dislo_den_11_Q1;
  const VariableGradient & _grad_screw_dislo_den_11_Q1;
  const VariableValue & _screw_dislo_den_11_Q2;
  const VariableGradient & _grad_screw_dislo_den_11_Q2;
  const VariableValue & _screw_dislo_den_11_Q3;
  const VariableGradient & _grad_screw_dislo_den_11_Q3;
  const VariableValue & _screw_dislo_den_11_Q4;
  const VariableGradient & _grad_screw_dislo_den_11_Q4;

  const VariableValue & _edge_dislo_den_12_Q1;
  const VariableGradient & _grad_edge_dislo_den_12_Q1;
  const VariableValue & _edge_dislo_den_12_Q2;
  const VariableGradient & _grad_edge_dislo_den_12_Q2;
  const VariableValue & _edge_dislo_den_12_Q3;
  const VariableGradient & _grad_edge_dislo_den_12_Q3;
  const VariableValue & _edge_dislo_den_12_Q4;
  const VariableGradient & _grad_edge_dislo_den_12_Q4;
  const VariableValue & _screw_dislo_den_12_Q1;
  const VariableGradient & _grad_screw_dislo_den_12_Q1;
  const VariableValue & _screw_dislo_den_12_Q2;
  const VariableGradient & _grad_screw_dislo_den_12_Q2;
  const VariableValue & _screw_dislo_den_12_Q3;
  const VariableGradient & _grad_screw_dislo_den_12_Q3;
  const VariableValue & _screw_dislo_den_12_Q4;
  const VariableGradient & _grad_screw_dislo_den_12_Q4;

  ///@}

  const Real _initial_total_dislo_den;
};
