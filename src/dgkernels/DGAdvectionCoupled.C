//* This dgkernels is to solve the numerical problems caused by
//* the dislocation density transport equations
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation
//* 25 Jan 2024

#include "DGAdvectionCoupled.h"

registerMooseObject("cdf_updateApp", DGAdvectionCoupled);

InputParameters
DGAdvectionCoupled::validParams()
{
  InputParameters params = DGKernel::validParams();
  params.addClassDescription("DG upwinding for the advection of a coupled variable. "
                             "Upwind condition is calculated both on edge/screw dislocations "
                             "in this element and on the neighbouring element.");
  params.addRequiredParam<int>("slip_sys_index",
                               "Slip system index to determine slip direction "
                               "for instance from 0 to 11 for FCC.");
  MooseEnum dislo_sign("positive negative", "positive");
  params.addRequiredParam<MooseEnum>("dislo_sign", dislo_sign, "Sign of dislocations.");
  MooseEnum dislo_character("edge screw", "edge");
  params.addRequiredParam<MooseEnum>(
      "dislo_character", dislo_character, "Character of dislocations: edge or screw.");
  return params;
}

DGAdvectionCoupled::DGAdvectionCoupled(const InputParameters & parameters)
  : DGKernel(parameters),
    _edge_slip_direction(
        getMaterialProperty<std::vector<Real>>("edge_slip_direction")), // Edge velocity direction
    _screw_slip_direction(
        getMaterialProperty<std::vector<Real>>("screw_slip_direction")), // Screw velocity direction
    _dislo_velocity(
        getMaterialProperty<std::vector<Real>>("dislo_velocity")), // Velocity value (signed)
    _slip_sys_index(getParam<int>("slip_sys_index")),
    _dislo_sign(getParam<MooseEnum>("dislo_sign").getEnum<DisloSign>()),
    _dislo_character(getParam<MooseEnum>("dislo_character").getEnum<DisloCharacter>())
{
}

// read dislocation velocity from material object
// and store in _velocity
void
DGAdvectionCoupled::getDislocationVelocity()
{

  Real edge_sign;

  switch (_dislo_sign)
  {
    case DisloSign::positive:
      edge_sign = 1.0;
      break;
    case DisloSign::negative:
      edge_sign = -1.0;
      break;
  }

  // Find dislocation velocity based on slip systems index and dislocation character
  switch (_dislo_character)
  {
    case DisloCharacter::edge:
      for (const auto j : make_range(LIBMESH_DIM))
      {
        _velocity(j) =
            _edge_slip_direction[_qp][_slip_sys_index * LIBMESH_DIM + j]; // edge direction
        _velocity(j) *= edge_sign;
      }
      break;
    case DisloCharacter::screw:
      for (const auto j : make_range(LIBMESH_DIM))
      {
        _velocity(j) =
            _screw_slip_direction[_qp][_slip_sys_index * LIBMESH_DIM + j]; // screw direction
        _velocity(j) *= edge_sign;
      }
      break;
  }

  for (const auto j : make_range(LIBMESH_DIM))
  {
    _velocity(j) *= _dislo_velocity[_qp][_slip_sys_index];
  }
}

Real
DGAdvectionCoupled::computeQpResidual(Moose::DGResidualType type)
{
  Real r = 0;

  getDislocationVelocity();

  Real vdotn = _velocity * _normals[_qp];
  Real u_vdotn = vdotn * _u[_qp];
  Real neigh_u_vdotn = vdotn * _u_neighbor[_qp];

  switch (type)
  {
    case Moose::Element:

      switch (_dislo_sign)
      {
        case DisloSign::positive:

          if (u_vdotn >= 0)
            r += u_vdotn * _test[_i][_qp];

          if (neigh_u_vdotn < 0)
            r += neigh_u_vdotn * _test[_i][_qp];

          break;

        case DisloSign::negative:

          if (u_vdotn <= 0)
            r += u_vdotn * _test[_i][_qp];

          if (neigh_u_vdotn > 0)
            r += neigh_u_vdotn * _test[_i][_qp];

          break;
      }

      break;

    case Moose::Neighbor:

      switch (_dislo_sign)
      {
        case DisloSign::positive:

          if (u_vdotn >= 0)
            r -= u_vdotn * _test_neighbor[_i][_qp];

          if (neigh_u_vdotn < 0)
            r -= neigh_u_vdotn * _test_neighbor[_i][_qp];

          break;

        case DisloSign::negative:

          if (u_vdotn <= 0)
            r -= u_vdotn * _test_neighbor[_i][_qp];

          if (neigh_u_vdotn > 0)
            r -= neigh_u_vdotn * _test_neighbor[_i][_qp];

          break;
      }

      break;
  }

  return r;
}

Real
DGAdvectionCoupled::computeQpJacobian(Moose::DGJacobianType type)
{

  Real r = 0;
  Real vdotn;
  Real u_vdotn;
  // Real neigh_u_vdotn;

  getDislocationVelocity();

  vdotn = _velocity * _normals[_qp];
  u_vdotn = vdotn * _u[_qp];
  // neigh_u_vdotn = vdotn * _u_neighbor[_qp];

  switch (type)
  {
    case Moose::ElementElement:

      switch (_dislo_sign)
      {
        case DisloSign::positive:

          if (u_vdotn >= 0)
            r += vdotn * _phi[_j][_qp] * _test[_i][_qp];

          break;

        case DisloSign::negative:

          if (u_vdotn <= 0)
            r += vdotn * _phi[_j][_qp] * _test[_i][_qp];

          break;
      }

      break;

    case Moose::ElementNeighbor:

      switch (_dislo_sign)
      {
        case DisloSign::positive:

          if (u_vdotn < 0)
            r += vdotn * _phi_neighbor[_j][_qp] * _test[_i][_qp];

          break;

        case DisloSign::negative:

          if (u_vdotn > 0)
            r += vdotn * _phi_neighbor[_j][_qp] * _test[_i][_qp];

          break;
      }

      break;

    case Moose::NeighborElement:

      switch (_dislo_sign)
      {
        case DisloSign::positive:

          if (u_vdotn >= 0)
            r -= vdotn * _phi[_j][_qp] * _test_neighbor[_i][_qp];

          break;

        case DisloSign::negative:

          if (u_vdotn <= 0)
            r -= vdotn * _phi[_j][_qp] * _test_neighbor[_i][_qp];

          break;
      }

      break;

    case Moose::NeighborNeighbor:

      switch (_dislo_sign)
      {
        case DisloSign::positive:

          if (u_vdotn < 0)
            r -= vdotn * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];

          break;

        case DisloSign::negative:

          if (u_vdotn > 0)
            r -= vdotn * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];

          break;
      }

      break;
  }

  return r;
}

// Real
// DGAdvectionCoupled::computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar)
// {
//   Real r = 0;
//   Real vdotn;
//   Real u_vdotn;

//   getDislocationVelocity();

//   vdotn = _velocity * _normals[_qp];
//   u_vdotn = vdotn * _u[_qp];
// }