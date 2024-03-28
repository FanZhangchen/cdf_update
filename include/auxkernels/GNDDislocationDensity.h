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
class GNDDislocationDensity : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  GNDDislocationDensity(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// The value of the coupled variables
  const VariableValue & _rhoe1;

  const VariableValue & _rhoe2;

  const VariableValue & _rhoe3;

  const VariableValue & _rhoe4;

  const VariableValue & _rhos1;

  const VariableValue & _rhos2;

  const VariableValue & _rhos3;

  const VariableValue & _rhos4;

  const Real _initial_total_dislo_den;

};
