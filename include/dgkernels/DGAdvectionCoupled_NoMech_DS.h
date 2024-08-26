//* This dgkernels is to solve the numerical problems caused by
//* the dislocation density transport equations
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation
//* 25 Jan 2024

#pragma once

#include "DGKernel.h"

class DGAdvectionCoupled_NoMech_DS : public DGKernel
{
public:
  static InputParameters validParams();

  DGAdvectionCoupled_NoMech_DS(const InputParameters & parameters);

protected:
  virtual void getDislocationVelocity();
  virtual Real computeQpResidual(Moose::DGResidualType type) override;
  virtual Real computeQpJacobian(Moose::DGJacobianType type) override;

  /// advection velocity
  RealVectorValue _velocity;

  // Dislocation velocity value (signed) on all slip systems
  const MaterialProperty<std::vector<Real>> & _dislo_velocity;

  // Slip system index to determine slip direction
  const unsigned int _slip_sys_index;

  // Sign of dislocations
  const enum class DisloSign { positive, negative } _dislo_sign;

  // Character of dislocations (edge or screw)
  const enum class DisloCharacter { edge_00, edge_90 } _dislo_character;
};
