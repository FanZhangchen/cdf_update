//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TotalDislocationDensity.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("cdf_updateApp", TotalDislocationDensity);

InputParameters
TotalDislocationDensity::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  // Add a "coupling paramater" to get a variable from the input file.
  params.addCoupledVar("rhoe1", 0.0, "The edge dislocation density 1.");

  params.addCoupledVar("rhoe2", 0.0, "The edge dislocation density 2.");

  params.addCoupledVar("rhoe3", 0.0, "The edge dislocation density 3.");

  params.addCoupledVar("rhoe4", 0.0, "The edge dislocation density 4.");

  params.addCoupledVar("rhos1", 0.0, "The screw dislocation density 1.");

  params.addCoupledVar("rhos2", 0.0, "The screw dislocation density 2.");

  params.addCoupledVar("rhos3", 0.0, "The screw dislocation density 3.");

  params.addCoupledVar("rhos4", 0.0, "The screw dislocation density 4.");

  params.addParam<Real>("initial_total_dislo_den", 0.0, "Scale parameters");

  return params;
}

TotalDislocationDensity::TotalDislocationDensity(const InputParameters & parameters)
  : VectorAuxKernel(parameters),

    // Get the value of the variables
    _rhoe1(coupledValue("rhoe1")),

    _rhoe2(coupledValue("rhoe2")),

    _rhoe3(coupledValue("rhoe3")),

    _rhoe4(coupledValue("rhoe4")),

    _rhos1(coupledValue("rhos1")),

    _rhos2(coupledValue("rhos2")),

    _rhos3(coupledValue("rhos3")),

    _rhos4(coupledValue("rhos4")),

    _initial_total_dislo_den(getParam<Real>("initial_total_dislo_den"))

{
}

RealVectorValue
TotalDislocationDensity::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  Real rhot;

  rhot = (_rhoe1[_qp] + _rhoe2[_qp] + _rhoe3[_qp] + _rhoe4[_qp] 
    + _rhos1[_qp] + _rhos2[_qp] + _rhos3[_qp] + _rhos4[_qp]) / _initial_total_dislo_den;

  return rhot;
}
