//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TotalDislocationDensitySingleSlipSys.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("cdf_updateApp", TotalDislocationDensitySingleSlipSys);

InputParameters
TotalDislocationDensitySingleSlipSys::validParams()
{
  InputParameters params = AuxKernel::validParams();

  params.addRequiredParam<int>("nss", "Number of slip systems");
  // Add a "coupling paramater" to get a variable from the input file.
  params.addCoupledVar("edge_dislo_den_1_Q1", 0.0, "edge dislocation density in Q1: slip system 1");
  params.addCoupledVar("edge_dislo_den_1_Q2", 0.0, "edge dislocation density in Q2: slip system 1");
  params.addCoupledVar("edge_dislo_den_1_Q3", 0.0, "edge dislocation density in Q3: slip system 1");
  params.addCoupledVar("edge_dislo_den_1_Q4", 0.0, "edge dislocation density in Q4: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q1", 0.0, "screw dislocation density in Q1: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q2", 0.0, "screw dislocation density in Q2: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q3", 0.0, "screw dislocation density in Q3: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q4", 0.0, "screw dislocation density in Q4: slip system 1");

  params.addParam<Real>("initial_total_dislo_den", 0.0, "Scale parameters");

  return params;
}

TotalDislocationDensitySingleSlipSys::TotalDislocationDensitySingleSlipSys(const InputParameters & parameters)
  : AuxKernel(parameters),

    _nss(getParam<int>("nss")),

    _gssT(_nss),

    // slip 1
    _edge_dislo_den_1_Q1(coupledValue("edge_dislo_den_1_Q1")),
    _grad_edge_dislo_den_1_Q1(coupledGradient("edge_dislo_den_1_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_1_Q2(coupledValue("edge_dislo_den_1_Q2")),
    _grad_edge_dislo_den_1_Q2(coupledGradient("edge_dislo_den_1_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_1_Q3(coupledValue("edge_dislo_den_1_Q3")),
    _grad_edge_dislo_den_1_Q3(coupledGradient("edge_dislo_den_1_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_1_Q4(coupledValue("edge_dislo_den_1_Q4")),
    _grad_edge_dislo_den_1_Q4(coupledGradient("edge_dislo_den_1_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_1_Q1(coupledValue("screw_dislo_den_1_Q1")),
    _grad_screw_dislo_den_1_Q1(coupledGradient("screw_dislo_den_1_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_1_Q2(coupledValue("screw_dislo_den_1_Q2")),
    _grad_screw_dislo_den_1_Q2(coupledGradient("screw_dislo_den_1_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_1_Q3(coupledValue("screw_dislo_den_1_Q3")),
    _grad_screw_dislo_den_1_Q3(coupledGradient("screw_dislo_den_1_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_1_Q4(coupledValue("screw_dislo_den_1_Q4")),
    _grad_screw_dislo_den_1_Q4(coupledGradient("screw_dislo_den_1_Q4")), // Coupled rhoen gradient

    _initial_total_dislo_den(getParam<Real>("initial_total_dislo_den"))

{
}

Real
TotalDislocationDensitySingleSlipSys::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  Real rhot = 0.0;
  Real normalized_rhot = 0.0;

  std::vector<Real> rho_edge_Q1(_nss);
  std::vector<Real> rho_edge_Q2(_nss);
  std::vector<Real> rho_edge_Q3(_nss);
  std::vector<Real> rho_edge_Q4(_nss);
  std::vector<Real> rho_screw_Q1(_nss);
  std::vector<Real> rho_screw_Q2(_nss);
  std::vector<Real> rho_screw_Q3(_nss);
  std::vector<Real> rho_screw_Q4(_nss);

  rho_edge_Q1[0] = _edge_dislo_den_1_Q1[_qp];

  rho_edge_Q2[0] = _edge_dislo_den_1_Q2[_qp];

  rho_edge_Q3[0] = _edge_dislo_den_1_Q3[_qp];

  rho_edge_Q4[0] = _edge_dislo_den_1_Q4[_qp];

  rho_screw_Q1[0] = _screw_dislo_den_1_Q1[_qp];

  rho_screw_Q2[0] = _screw_dislo_den_1_Q2[_qp];

  rho_screw_Q3[0] = _screw_dislo_den_1_Q3[_qp];

  rho_screw_Q4[0] = _screw_dislo_den_1_Q4[_qp];

  for (const auto i : make_range(_nss))
  {
    rhot += rho_edge_Q1[i] + rho_edge_Q2[i] + rho_edge_Q3[i] + rho_edge_Q4[i] + rho_screw_Q1[i] +
            rho_screw_Q2[i] + rho_screw_Q3[i] + rho_screw_Q4[i];
  }

  normalized_rhot = rhot / _initial_total_dislo_den;

  return normalized_rhot;
}
