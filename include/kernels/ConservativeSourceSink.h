#pragma once

#include "Kernel.h"

/**
 * Advection of the variable by the velocity provided by the user.
 * Options for numerical stabilization are: none; full upwinding
 * Advection velocity direction \vec{v} and magnitude/sign are taken as material property.
 * Based on dislocation velocity model dependent on the resolved shear stress.
 * Signed edge and screw dislocations are considered
 */

class ConservativeSourceSink : public Kernel
{
public:
  static InputParameters validParams();

  ConservativeSourceSink(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;
  virtual void computeResidual() override;
  virtual void computeJacobian() override;
  virtual void computeOffDiagJacobian(unsigned int jvar) override;

  /// enum to make the code clearer
  enum class SlipSystem
  {
    UNSUPPORTED,
    SLIP_SYSTEM_1,
    SLIP_SYSTEM_2
    // ... add remaining FCC systems (up to FCC_SYSTEM_12)
  };

  const Real _burgers;
  const Real _ke_b;
  const Real _ks_b;
  const Real _de;
  const Real _ds;
  const Real _Ce;
  const Real _Cs;

  const VariableValue _edge_dislo_1_Q1;
  const VariableValue _edge_dislo_1_Q3;
  const VariableValue _edge_dislo_2_Q1;
  const VariableValue _edge_dislo_2_Q3;

  unsigned int _edge_dislo_1_Q1_var_number;
  unsigned int _edge_dislo_1_Q3_var_number;
  unsigned int _edge_dislo_2_Q1_var_number;
  unsigned int _edge_dislo_2_Q3_var_number;

  // Slip system index to determine slip direction
  const unsigned int _slip_sys_index;

  // Slip system number of the total system
  const unsigned int _slip_sys_number;

  // The number of the activated quadrant used in the model
  const unsigned int _activated_quadrant;

  // Sign of dislocations
  const enum class DisloSign { positive, negative } _dislo_sign;

  // Character of dislocations (edge or screw)
  const enum class DisloCharacter { edge, screw } _dislo_character;

  // Check that dislocation density is positive
  // if it went below zero, it should not be further decreased
  bool _check_rho_positive;  


  // Slip increment of the slip systems
  const MaterialProperty<std::vector<Real>> & _slip_increment;

  /// Returns the source and sink terms for residual
  Real sourcesink();

  /// Returns the source and sink terms for Jacobian
  Real sourcesink_jacobian();

};
