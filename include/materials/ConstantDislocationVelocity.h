//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* MMS convergence test support
//* Provides constant dislocation velocity and slip direction material properties
//* for DGAdvectionCoupled kernel testing.

#pragma once

#include "Material.h"

/**
 * ConstantDislocationVelocity provides constant-valued material properties
 * required by DGAdvectionCoupled.  Used in MMS (Method of Manufactured Solutions)
 * convergence tests where a simple uniform velocity field is needed.
 */
class ConstantDislocationVelocity : public Material
{
public:
  static InputParameters validParams();

  ConstantDislocationVelocity(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  /// Number of slip systems
  const unsigned int _nss;

  /// Constant velocity magnitude on each slip system
  const Real _velocity_value;

  /// X-component of the edge dislocation slip direction
  const Real _edge_direction_x;

  ///@{ Material properties required by DGAdvectionCoupled
  MaterialProperty<std::vector<Real>> & _dislo_velocity;
  MaterialProperty<std::vector<Real>> & _edge_slip_direction;
  MaterialProperty<std::vector<Real>> & _screw_slip_direction;
  ///@}
};
