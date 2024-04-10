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

  rhot = (_rhoe1[_qp] + _rhoe2[_qp] + _rhoe3[_qp] + _rhoe4[_qp] + _rhos1[_qp] + _rhos2[_qp] +
          _rhos3[_qp] + _rhos4[_qp]) /
         _initial_total_dislo_den;

  // ((edge_dislo_1_Q1+edge_dislo_1_Q2+edge_dislo_1_Q3+edge_dislo_1_Q4+screw_dislo_1_Q1+screw_dislo_1_Q2+screw_dislo_1_Q3+screw_dislo_1_Q4)+(edge_dislo_2_Q1+edge_dislo_2_Q2+edge_dislo_2_Q3+edge_dislo_2_Q4+screw_dislo_2_Q1+screw_dislo_2_Q2+screw_dislo_2_Q3+screw_dislo_2_Q4)+
  // edge_dislo_3_Q1+edge_dislo_3_Q2+edge_dislo_3_Q3+edge_dislo_3_Q4+screw_dislo_3_Q1+screw_dislo_3_Q2+screw_dislo_3_Q3+screw_dislo_3_Q4)+(edge_dislo_4_Q1+edge_dislo_4_Q2+edge_dislo_4_Q3+edge_dislo_4_Q4+screw_dislo_4_Q1+screw_dislo_4_Q2+screw_dislo_4_Q3+screw_dislo_4_Q4)+(edge_dislo_5_Q1+edge_dislo_5_Q2+edge_dislo_5_Q3+edge_dislo_5_Q4+screw_dislo_5_Q1+screw_dislo_5_Q2+screw_dislo_5_Q3+screw_dislo_5_Q4)+(edge_dislo_6_Q1+edge_dislo_6_Q2+edge_dislo_6_Q3+edge_dislo_6_Q4+screw_dislo_6_Q1+screw_dislo_6_Q2+screw_dislo_6_Q3+screw_dislo_6_Q4)+(edge_dislo_7_Q1+edge_dislo_7_Q2+edge_dislo_7_Q3+edge_dislo_7_Q4+screw_dislo_7_Q1+screw_dislo_7_Q2+screw_dislo_7_Q3+screw_dislo_7_Q4)+(edge_dislo_8_Q1+edge_dislo_8_Q2+edge_dislo_8_Q3+edge_dislo_8_Q4+screw_dislo_8_Q1+screw_dislo_8_Q2+screw_dislo_8_Q3+screw_dislo_8_Q4)+(edge_dislo_9_Q1+edge_dislo_9_Q2+edge_dislo_9_Q3+edge_dislo_9_Q4+screw_dislo_9_Q1+screw_dislo_9_Q2+screw_dislo_9_Q3+screw_dislo_9_Q4)+(edge_dislo_10_Q1+edge_dislo_10_Q2+edge_dislo_10_Q3+edge_dislo_10_Q4+screw_dislo_10_Q1+screw_dislo_10_Q2+screw_dislo_10_Q3+screw_dislo_10_Q4)+(edge_dislo_11_Q1+edge_dislo_11_Q2+edge_dislo_11_Q3+edge_dislo_11_Q4+screw_dislo_11_Q1+screw_dislo_11_Q2+screw_dislo_11_Q3+screw_dislo_11_Q4)+(edge_dislo_12_Q1+edge_dislo_12_Q2+edge_dislo_12_Q3+edge_dislo_12_Q4+screw_dislo_12_Q1+screw_dislo_12_Q2+screw_dislo_12_Q3+screw_dislo_12_Q4))/12/16000

  return rhot;
}
