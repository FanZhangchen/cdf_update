#pragma once

#include "Material.h"

// A helper class from MOOSE that linear interpolates x,y data
#include "DerivativeMaterialInterface.h"

/**
 * Material objects inherit from Material and override computeQpProperties.
 *
 * Their job is to declare properties for use by other objects in the
 * calculation such as Kernels and BoundaryConditions.
 */
class SSDUpdate : public DerivativeMaterialInterface<Material>
{
public:
  static InputParameters validParams();

  SSDUpdate(const InputParameters & parameters);

protected:
  /// Necessary override. This is where the values of the properties are computed.
  virtual void computeQpProperties();
  virtual void initQpStatefulProperties();

private:

  const Real _burgers;
  const Real _ke_b;
  const Real _ks_b;
  const Real _de;
  const Real _ds;
  const Real _Ce;
  const Real _Cs;

  const unsigned int _nss;

  std::vector<Real> _gssT;

  // for a FCC, there are 12 slips inside
  // thus, here gives a completed variables
  // variable value of Slip 1
  const VariableValue & _edge_dislo_den_1_Q1;
  const VariableValue & _edge_dislo_den_1_Q2;
  const VariableValue & _edge_dislo_den_1_Q3;
  const VariableValue & _edge_dislo_den_1_Q4;
  const VariableValue & _screw_dislo_den_1_Q1;
  const VariableValue & _screw_dislo_den_1_Q2;
  const VariableValue & _screw_dislo_den_1_Q3;
  const VariableValue & _screw_dislo_den_1_Q4;

  // variable value of Slip 2
  const VariableValue & _edge_dislo_den_2_Q1;
  const VariableValue & _edge_dislo_den_2_Q2;
  const VariableValue & _edge_dislo_den_2_Q3;
  const VariableValue & _edge_dislo_den_2_Q4;
  const VariableValue & _screw_dislo_den_2_Q1;
  const VariableValue & _screw_dislo_den_2_Q2;
  const VariableValue & _screw_dislo_den_2_Q3;
  const VariableValue & _screw_dislo_den_2_Q4;

  // variable value of Slip 3
  const VariableValue & _edge_dislo_den_3_Q1;
  const VariableValue & _edge_dislo_den_3_Q2;
  const VariableValue & _edge_dislo_den_3_Q3;
  const VariableValue & _edge_dislo_den_3_Q4;
  const VariableValue & _screw_dislo_den_3_Q1;
  const VariableValue & _screw_dislo_den_3_Q2;
  const VariableValue & _screw_dislo_den_3_Q3;
  const VariableValue & _screw_dislo_den_3_Q4;

  // variable value of Slip 4
  const VariableValue & _edge_dislo_den_4_Q1;
  const VariableValue & _edge_dislo_den_4_Q2;
  const VariableValue & _edge_dislo_den_4_Q3;
  const VariableValue & _edge_dislo_den_4_Q4;
  const VariableValue & _screw_dislo_den_4_Q1;
  const VariableValue & _screw_dislo_den_4_Q2;
  const VariableValue & _screw_dislo_den_4_Q3;
  const VariableValue & _screw_dislo_den_4_Q4;

  // variable value of Slip 5
  const VariableValue & _edge_dislo_den_5_Q1;
  const VariableValue & _edge_dislo_den_5_Q2;
  const VariableValue & _edge_dislo_den_5_Q3;
  const VariableValue & _edge_dislo_den_5_Q4;
  const VariableValue & _screw_dislo_den_5_Q1;
  const VariableValue & _screw_dislo_den_5_Q2;
  const VariableValue & _screw_dislo_den_5_Q3;
  const VariableValue & _screw_dislo_den_5_Q4;

  // variable value of Slip 6
  const VariableValue & _edge_dislo_den_6_Q1;
  const VariableValue & _edge_dislo_den_6_Q2;
  const VariableValue & _edge_dislo_den_6_Q3;
  const VariableValue & _edge_dislo_den_6_Q4;
  const VariableValue & _screw_dislo_den_6_Q1;
  const VariableValue & _screw_dislo_den_6_Q2;
  const VariableValue & _screw_dislo_den_6_Q3;
  const VariableValue & _screw_dislo_den_6_Q4;

  // variable value of Slip 7
  const VariableValue & _edge_dislo_den_7_Q1;
  const VariableValue & _edge_dislo_den_7_Q2;
  const VariableValue & _edge_dislo_den_7_Q3;
  const VariableValue & _edge_dislo_den_7_Q4;
  const VariableValue & _screw_dislo_den_7_Q1;
  const VariableValue & _screw_dislo_den_7_Q2;
  const VariableValue & _screw_dislo_den_7_Q3;
  const VariableValue & _screw_dislo_den_7_Q4;

  // variable value of Slip 8
  const VariableValue & _edge_dislo_den_8_Q1;
  const VariableValue & _edge_dislo_den_8_Q2;
  const VariableValue & _edge_dislo_den_8_Q3;
  const VariableValue & _edge_dislo_den_8_Q4;
  const VariableValue & _screw_dislo_den_8_Q1;
  const VariableValue & _screw_dislo_den_8_Q2;
  const VariableValue & _screw_dislo_den_8_Q3;
  const VariableValue & _screw_dislo_den_8_Q4;

  // variable value of Slip 9
  const VariableValue & _edge_dislo_den_9_Q1;
  const VariableValue & _edge_dislo_den_9_Q2;
  const VariableValue & _edge_dislo_den_9_Q3;
  const VariableValue & _edge_dislo_den_9_Q4;
  const VariableValue & _screw_dislo_den_9_Q1;
  const VariableValue & _screw_dislo_den_9_Q2;
  const VariableValue & _screw_dislo_den_9_Q3;
  const VariableValue & _screw_dislo_den_9_Q4;

  // variable value of Slip 10
  const VariableValue & _edge_dislo_den_10_Q1;
  const VariableValue & _edge_dislo_den_10_Q2;
  const VariableValue & _edge_dislo_den_10_Q3;
  const VariableValue & _edge_dislo_den_10_Q4;
  const VariableValue & _screw_dislo_den_10_Q1;
  const VariableValue & _screw_dislo_den_10_Q2;
  const VariableValue & _screw_dislo_den_10_Q3;
  const VariableValue & _screw_dislo_den_10_Q4;

  // variable value of Slip 11
  const VariableValue & _edge_dislo_den_11_Q1;
  const VariableValue & _edge_dislo_den_11_Q2;
  const VariableValue & _edge_dislo_den_11_Q3;
  const VariableValue & _edge_dislo_den_11_Q4;
  const VariableValue & _screw_dislo_den_11_Q1;
  const VariableValue & _screw_dislo_den_11_Q2;
  const VariableValue & _screw_dislo_den_11_Q3;
  const VariableValue & _screw_dislo_den_11_Q4;

  // variable value of Slip 12
  const VariableValue & _edge_dislo_den_12_Q1;
  const VariableValue & _edge_dislo_den_12_Q2;
  const VariableValue & _edge_dislo_den_12_Q3;
  const VariableValue & _edge_dislo_den_12_Q4;
  const VariableValue & _screw_dislo_den_12_Q1;
  const VariableValue & _screw_dislo_den_12_Q2;
  const VariableValue & _screw_dislo_den_12_Q3;
  const VariableValue & _screw_dislo_den_12_Q4;

  const MaterialProperty<std::vector<Real>> & _slip_increment;

  MaterialProperty<std::vector<Real>> & _edge_dislocation_increment;
  const MaterialProperty<std::vector<Real>> & _edge_dislocation_increment_old;

  MaterialProperty<std::vector<Real>> & _screw_dislocation_increment;
  const MaterialProperty<std::vector<Real>> & _screw_dislocation_increment_old;
};
