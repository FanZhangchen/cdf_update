//* This dgkernels is to solve the numerical problems caused by
//* the dislocation density transport equations
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation
//* 25 Jan 2024

#include "DGAdvectionCoupled_NoMech_DS.h"

registerMooseObject("cdf_updateApp", DGAdvectionCoupled_NoMech_DS);

InputParameters
DGAdvectionCoupled_NoMech_DS::validParams()
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
  MooseEnum dislo_character("edge_00 edge_90", "edge_00");
  params.addRequiredParam<MooseEnum>(
      "dislo_character", dislo_character, "Character of dislocations: edge or screw.");
  return params;
}

DGAdvectionCoupled_NoMech_DS::DGAdvectionCoupled_NoMech_DS(const InputParameters & parameters)
  : DGKernel(parameters),
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
DGAdvectionCoupled_NoMech_DS::getDislocationVelocity()
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
    case DisloCharacter::edge_00:
      _velocity(0) = _dislo_velocity[_qp][0] * edge_sign; // velocity value
      _velocity(1) = 0.0;                                 // positive or negative dislocation
      _velocity(2) = 0.0;
      break;
    case DisloCharacter::edge_90:
      _velocity(0) = 0.0;                                 // velocity value
      _velocity(1) = _dislo_velocity[_qp][1] * edge_sign; // positive or negative dislocation
      _velocity(2) = 0.0;
      break;
  }

}

Real
DGAdvectionCoupled_NoMech_DS::computeQpResidual(Moose::DGResidualType type)
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
DGAdvectionCoupled_NoMech_DS::computeQpJacobian(Moose::DGJacobianType type)
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