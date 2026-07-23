//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* MMS convergence test support

#include "ConstantDislocationVelocity.h"

registerMooseObject("cdf_updateApp", ConstantDislocationVelocity);

InputParameters
ConstantDislocationVelocity::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription(
      "Provides constant dislocation velocity and slip direction material "
      "properties for DGAdvectionCoupled.  Intended for MMS convergence tests.");
  params.addParam<int>("nss", 1, "Number of slip systems (default: 1)");
  params.addParam<Real>("velocity", 1.0, "Constant dislocation velocity magnitude (default: 1.0)");
  params.addParam<Real>(
      "edge_direction", 1.0, "X-component of edge slip direction (default: 1.0)");
  return params;
}

ConstantDislocationVelocity::ConstantDislocationVelocity(const InputParameters & parameters)
  : Material(parameters),
    _nss(getParam<int>("nss")),
    _velocity_value(getParam<Real>("velocity")),
    _edge_direction_x(getParam<Real>("edge_direction")),
    _dislo_velocity(declareProperty<std::vector<Real>>("dislo_velocity")),
    _edge_slip_direction(declareProperty<std::vector<Real>>("edge_slip_direction")),
    _screw_slip_direction(declareProperty<std::vector<Real>>("screw_slip_direction"))
{
}

void
ConstantDislocationVelocity::computeQpProperties()
{
  const unsigned int dim = LIBMESH_DIM;

  _dislo_velocity[_qp].resize(_nss);
  _edge_slip_direction[_qp].resize(_nss * dim);
  _screw_slip_direction[_qp].resize(_nss * dim);

  for (unsigned int i = 0; i < _nss; ++i)
  {
    // Constant velocity magnitude
    _dislo_velocity[_qp][i] = _velocity_value;

    for (unsigned int j = 0; j < dim; ++j)
    {
      // Edge slip direction: only x-component is non-zero
      _edge_slip_direction[_qp][i * dim + j] = (j == 0) ? _edge_direction_x : 0.0;
      // Screw slip direction: unused in edge-character MMS test, set to zero
      _screw_slip_direction[_qp][i * dim + j] = 0.0;
    }
  }
}
