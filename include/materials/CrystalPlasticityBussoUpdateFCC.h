//* This file is for continuum dislocation density field-based theory
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation
//* 6 Jan 2024

#pragma once

#include "CrystalPlasticityDislocationUpdateBase.h"

class CrystalPlasticityBussoUpdateFCC;

/**
 * CrystalPlasticityBussoUpdateFCC uses the multiplicative decomposition of the
 * deformation gradient and solves the PK2 stress residual equation at the
 * intermediate configuration to evolve the material state. The internal
 * variables are updated using an interative predictor-corrector algorithm.
 * Backward Euler integration rule is used for the rate equations.
 */

class CrystalPlasticityBussoUpdateFCC : public CrystalPlasticityDislocationUpdateBase
{
public:
  static InputParameters validParams();

  CrystalPlasticityBussoUpdateFCC(const InputParameters & parameters);

protected:
  /**
   * initializes the stateful properties such as
   * stress, plastic deformation gradient, slip system resistances, etc.
   */
  virtual void initQpStatefulProperties() override;

  virtual void calculateSchmidTensor(const unsigned int & number_dislocation_systems,
                                     const std::vector<RealVectorValue> & plane_normal_vector,
                                     const std::vector<RealVectorValue> & direction_vector,
                                     std::vector<RankTwoTensor> & schmid_tensor,
                                     const RankTwoTensor & crysrot);

  /**
   * Sets the value of the current and previous substep iteration slip system
   * resistance to the old value at the start of the PK2 stress convergence
   * while loop.
   */
  virtual void setInitialConstitutiveVariableValues() override;

  /**
   * Sets the current slip system resistance value to the previous substep value.
   * In cases where only one substep is taken (or when the first) substep is taken,
   * this method just sets the current value to the old slip system resistance
   * value again.
   */
  virtual void setSubstepConstitutiveVariableValues() override;

  /**
   * Stores the current value of the slip system resistance into a separate
   * material property in case substepping is needed.
   */
  virtual void updateSubstepConstitutiveVariableValues() override;

  virtual bool calculateSlipRate() override;

  virtual void
  calculateEquivalentSlipIncrement(RankTwoTensor & /*equivalent_slip_increment*/) override;

  virtual void calculateConstitutiveSlipDerivative(std::vector<Real> & dslip_dtau) override;

  // Cache the slip system value before the update for the diff in the convergence check
  virtual void cacheStateVariablesBeforeUpdate() override;

  /**
   * Following the Constitutive model for slip system resistance as given in
   * Cheong, Ke-Shen, and Esteban P. Busso. "Discrete dislocation density modelling
   * of single phase FCC polycrystal aggregates." Acta materialia 52 (2004): 5665-5675.
   * The slip system resistant increment is calculated in exponential function
   * and a convergence check is performed on the slip system resistance increment
   */
  virtual void calculateStateVariableEvolutionRateComponent() override;

  /*
   * Finalizes the values of the state variables and slip system resistance
   * for the current timestep after convergence has been reached.
   */
  virtual bool updateStateVariables() override;

  /**
   * Calculate the velocity of dislocation
   * on each slip system
   */
  virtual void calculateDislocationVelocity();

  /**
   * Calculates the sum of the contribution of the initial slip resistance and the
   * edge and screwdislocations to the slip resistance on each slip system,
   * equation (13) from Cheng and Busso Acta 52 (2004)
   */
  virtual void calculateSlipResistance() override;

  /**
   * Calculates the accumulated plastic strain and stored energy density
   */
  void calculateAccumulatedEquivalentPlasticStrain();

  /*
   * Determines if the state variables, e.g. edge densities, have converged
   * by comparing the change in the values over the iteration period.
   */
  virtual bool areConstitutiveStateVariablesConverged() override;

  ///@{Varibles used in the Busso 1992 slip system resistance constiutive model
  const Real _r;
  const Real _temperature;
  const Real _p;
  const Real _q;
  const Real _f0;
  const Real _gdot0;
  const Real _tau_0;
  const Real _burgers;
  const Real _shear_modulus;
  const Real _boltzmann;
  const Real _scaling_Cb;

  const Real _dlamb;
  const Real _w1;
  const Real _w2;

  DenseVector<Real> _backstress;
  ///@}

  ///@{Nodal degrees of freedom
  const VariableValue & _edge_dislo_den_1_Q1;
  const VariableGradient & _grad_edge_dislo_den_1_Q1;
  const VariableValue & _edge_dislo_den_1_Q2;
  const VariableGradient & _grad_edge_dislo_den_1_Q2;
  const VariableValue & _edge_dislo_den_1_Q3;
  const VariableGradient & _grad_edge_dislo_den_1_Q3;
  const VariableValue & _edge_dislo_den_1_Q4;
  const VariableGradient & _grad_edge_dislo_den_1_Q4;
  const VariableValue & _screw_dislo_den_1_Q1;
  const VariableGradient & _grad_screw_dislo_den_1_Q1;
  const VariableValue & _screw_dislo_den_1_Q2;
  const VariableGradient & _grad_screw_dislo_den_1_Q2;
  const VariableValue & _screw_dislo_den_1_Q3;
  const VariableGradient & _grad_screw_dislo_den_1_Q3;
  const VariableValue & _screw_dislo_den_1_Q4;
  const VariableGradient & _grad_screw_dislo_den_1_Q4;

  const VariableValue & _edge_dislo_den_2_Q1;
  const VariableGradient & _grad_edge_dislo_den_2_Q1;
  const VariableValue & _edge_dislo_den_2_Q2;
  const VariableGradient & _grad_edge_dislo_den_2_Q2;
  const VariableValue & _edge_dislo_den_2_Q3;
  const VariableGradient & _grad_edge_dislo_den_2_Q3;
  const VariableValue & _edge_dislo_den_2_Q4;
  const VariableGradient & _grad_edge_dislo_den_2_Q4;
  const VariableValue & _screw_dislo_den_2_Q1;
  const VariableGradient & _grad_screw_dislo_den_2_Q1;
  const VariableValue & _screw_dislo_den_2_Q2;
  const VariableGradient & _grad_screw_dislo_den_2_Q2;
  const VariableValue & _screw_dislo_den_2_Q3;
  const VariableGradient & _grad_screw_dislo_den_2_Q3;
  const VariableValue & _screw_dislo_den_2_Q4;
  const VariableGradient & _grad_screw_dislo_den_2_Q4;

  const VariableValue & _edge_dislo_den_3_Q1;
  const VariableGradient & _grad_edge_dislo_den_3_Q1;
  const VariableValue & _edge_dislo_den_3_Q2;
  const VariableGradient & _grad_edge_dislo_den_3_Q2;
  const VariableValue & _edge_dislo_den_3_Q3;
  const VariableGradient & _grad_edge_dislo_den_3_Q3;
  const VariableValue & _edge_dislo_den_3_Q4;
  const VariableGradient & _grad_edge_dislo_den_3_Q4;
  const VariableValue & _screw_dislo_den_3_Q1;
  const VariableGradient & _grad_screw_dislo_den_3_Q1;
  const VariableValue & _screw_dislo_den_3_Q2;
  const VariableGradient & _grad_screw_dislo_den_3_Q2;
  const VariableValue & _screw_dislo_den_3_Q3;
  const VariableGradient & _grad_screw_dislo_den_3_Q3;
  const VariableValue & _screw_dislo_den_3_Q4;
  const VariableGradient & _grad_screw_dislo_den_3_Q4;

  const VariableValue & _edge_dislo_den_4_Q1;
  const VariableGradient & _grad_edge_dislo_den_4_Q1;
  const VariableValue & _edge_dislo_den_4_Q2;
  const VariableGradient & _grad_edge_dislo_den_4_Q2;
  const VariableValue & _edge_dislo_den_4_Q3;
  const VariableGradient & _grad_edge_dislo_den_4_Q3;
  const VariableValue & _edge_dislo_den_4_Q4;
  const VariableGradient & _grad_edge_dislo_den_4_Q4;
  const VariableValue & _screw_dislo_den_4_Q1;
  const VariableGradient & _grad_screw_dislo_den_4_Q1;
  const VariableValue & _screw_dislo_den_4_Q2;
  const VariableGradient & _grad_screw_dislo_den_4_Q2;
  const VariableValue & _screw_dislo_den_4_Q3;
  const VariableGradient & _grad_screw_dislo_den_4_Q3;
  const VariableValue & _screw_dislo_den_4_Q4;
  const VariableGradient & _grad_screw_dislo_den_4_Q4;

  const VariableValue & _edge_dislo_den_5_Q1;
  const VariableGradient & _grad_edge_dislo_den_5_Q1;
  const VariableValue & _edge_dislo_den_5_Q2;
  const VariableGradient & _grad_edge_dislo_den_5_Q2;
  const VariableValue & _edge_dislo_den_5_Q3;
  const VariableGradient & _grad_edge_dislo_den_5_Q3;
  const VariableValue & _edge_dislo_den_5_Q4;
  const VariableGradient & _grad_edge_dislo_den_5_Q4;
  const VariableValue & _screw_dislo_den_5_Q1;
  const VariableGradient & _grad_screw_dislo_den_5_Q1;
  const VariableValue & _screw_dislo_den_5_Q2;
  const VariableGradient & _grad_screw_dislo_den_5_Q2;
  const VariableValue & _screw_dislo_den_5_Q3;
  const VariableGradient & _grad_screw_dislo_den_5_Q3;
  const VariableValue & _screw_dislo_den_5_Q4;
  const VariableGradient & _grad_screw_dislo_den_5_Q4;

  const VariableValue & _edge_dislo_den_6_Q1;
  const VariableGradient & _grad_edge_dislo_den_6_Q1;
  const VariableValue & _edge_dislo_den_6_Q2;
  const VariableGradient & _grad_edge_dislo_den_6_Q2;
  const VariableValue & _edge_dislo_den_6_Q3;
  const VariableGradient & _grad_edge_dislo_den_6_Q3;
  const VariableValue & _edge_dislo_den_6_Q4;
  const VariableGradient & _grad_edge_dislo_den_6_Q4;
  const VariableValue & _screw_dislo_den_6_Q1;
  const VariableGradient & _grad_screw_dislo_den_6_Q1;
  const VariableValue & _screw_dislo_den_6_Q2;
  const VariableGradient & _grad_screw_dislo_den_6_Q2;
  const VariableValue & _screw_dislo_den_6_Q3;
  const VariableGradient & _grad_screw_dislo_den_6_Q3;
  const VariableValue & _screw_dislo_den_6_Q4;
  const VariableGradient & _grad_screw_dislo_den_6_Q4;

  const VariableValue & _edge_dislo_den_7_Q1;
  const VariableGradient & _grad_edge_dislo_den_7_Q1;
  const VariableValue & _edge_dislo_den_7_Q2;
  const VariableGradient & _grad_edge_dislo_den_7_Q2;
  const VariableValue & _edge_dislo_den_7_Q3;
  const VariableGradient & _grad_edge_dislo_den_7_Q3;
  const VariableValue & _edge_dislo_den_7_Q4;
  const VariableGradient & _grad_edge_dislo_den_7_Q4;
  const VariableValue & _screw_dislo_den_7_Q1;
  const VariableGradient & _grad_screw_dislo_den_7_Q1;
  const VariableValue & _screw_dislo_den_7_Q2;
  const VariableGradient & _grad_screw_dislo_den_7_Q2;
  const VariableValue & _screw_dislo_den_7_Q3;
  const VariableGradient & _grad_screw_dislo_den_7_Q3;
  const VariableValue & _screw_dislo_den_7_Q4;
  const VariableGradient & _grad_screw_dislo_den_7_Q4;

  const VariableValue & _edge_dislo_den_8_Q1;
  const VariableGradient & _grad_edge_dislo_den_8_Q1;
  const VariableValue & _edge_dislo_den_8_Q2;
  const VariableGradient & _grad_edge_dislo_den_8_Q2;
  const VariableValue & _edge_dislo_den_8_Q3;
  const VariableGradient & _grad_edge_dislo_den_8_Q3;
  const VariableValue & _edge_dislo_den_8_Q4;
  const VariableGradient & _grad_edge_dislo_den_8_Q4;
  const VariableValue & _screw_dislo_den_8_Q1;
  const VariableGradient & _grad_screw_dislo_den_8_Q1;
  const VariableValue & _screw_dislo_den_8_Q2;
  const VariableGradient & _grad_screw_dislo_den_8_Q2;
  const VariableValue & _screw_dislo_den_8_Q3;
  const VariableGradient & _grad_screw_dislo_den_8_Q3;
  const VariableValue & _screw_dislo_den_8_Q4;
  const VariableGradient & _grad_screw_dislo_den_8_Q4;

  const VariableValue & _edge_dislo_den_9_Q1;
  const VariableGradient & _grad_edge_dislo_den_9_Q1;
  const VariableValue & _edge_dislo_den_9_Q2;
  const VariableGradient & _grad_edge_dislo_den_9_Q2;
  const VariableValue & _edge_dislo_den_9_Q3;
  const VariableGradient & _grad_edge_dislo_den_9_Q3;
  const VariableValue & _edge_dislo_den_9_Q4;
  const VariableGradient & _grad_edge_dislo_den_9_Q4;
  const VariableValue & _screw_dislo_den_9_Q1;
  const VariableGradient & _grad_screw_dislo_den_9_Q1;
  const VariableValue & _screw_dislo_den_9_Q2;
  const VariableGradient & _grad_screw_dislo_den_9_Q2;
  const VariableValue & _screw_dislo_den_9_Q3;
  const VariableGradient & _grad_screw_dislo_den_9_Q3;
  const VariableValue & _screw_dislo_den_9_Q4;
  const VariableGradient & _grad_screw_dislo_den_9_Q4;

  const VariableValue & _edge_dislo_den_10_Q1;
  const VariableGradient & _grad_edge_dislo_den_10_Q1;
  const VariableValue & _edge_dislo_den_10_Q2;
  const VariableGradient & _grad_edge_dislo_den_10_Q2;
  const VariableValue & _edge_dislo_den_10_Q3;
  const VariableGradient & _grad_edge_dislo_den_10_Q3;
  const VariableValue & _edge_dislo_den_10_Q4;
  const VariableGradient & _grad_edge_dislo_den_10_Q4;
  const VariableValue & _screw_dislo_den_10_Q1;
  const VariableGradient & _grad_screw_dislo_den_10_Q1;
  const VariableValue & _screw_dislo_den_10_Q2;
  const VariableGradient & _grad_screw_dislo_den_10_Q2;
  const VariableValue & _screw_dislo_den_10_Q3;
  const VariableGradient & _grad_screw_dislo_den_10_Q3;
  const VariableValue & _screw_dislo_den_10_Q4;
  const VariableGradient & _grad_screw_dislo_den_10_Q4;

  const VariableValue & _edge_dislo_den_11_Q1;
  const VariableGradient & _grad_edge_dislo_den_11_Q1;
  const VariableValue & _edge_dislo_den_11_Q2;
  const VariableGradient & _grad_edge_dislo_den_11_Q2;
  const VariableValue & _edge_dislo_den_11_Q3;
  const VariableGradient & _grad_edge_dislo_den_11_Q3;
  const VariableValue & _edge_dislo_den_11_Q4;
  const VariableGradient & _grad_edge_dislo_den_11_Q4;
  const VariableValue & _screw_dislo_den_11_Q1;
  const VariableGradient & _grad_screw_dislo_den_11_Q1;
  const VariableValue & _screw_dislo_den_11_Q2;
  const VariableGradient & _grad_screw_dislo_den_11_Q2;
  const VariableValue & _screw_dislo_den_11_Q3;
  const VariableGradient & _grad_screw_dislo_den_11_Q3;
  const VariableValue & _screw_dislo_den_11_Q4;
  const VariableGradient & _grad_screw_dislo_den_11_Q4;

  const VariableValue & _edge_dislo_den_12_Q1;
  const VariableGradient & _grad_edge_dislo_den_12_Q1;
  const VariableValue & _edge_dislo_den_12_Q2;
  const VariableGradient & _grad_edge_dislo_den_12_Q2;
  const VariableValue & _edge_dislo_den_12_Q3;
  const VariableGradient & _grad_edge_dislo_den_12_Q3;
  const VariableValue & _edge_dislo_den_12_Q4;
  const VariableGradient & _grad_edge_dislo_den_12_Q4;
  const VariableValue & _screw_dislo_den_12_Q1;
  const VariableGradient & _grad_screw_dislo_den_12_Q1;
  const VariableValue & _screw_dislo_den_12_Q2;
  const VariableGradient & _grad_screw_dislo_den_12_Q2;
  const VariableValue & _screw_dislo_den_12_Q3;
  const VariableGradient & _grad_screw_dislo_den_12_Q3;
  const VariableValue & _screw_dislo_den_12_Q4;
  const VariableGradient & _grad_screw_dislo_den_12_Q4;
  
  ///@}

  // Rotated slip direction to couple with dislocation transport
  // to indicate dislocation velocity direction for all slip systems
  // edge dislocations
  MaterialProperty<std::vector<Real>> & _edge_slip_direction;

  // edge dislocation line direction
  // corresponding to direction of motion of screw dislocations
  MaterialProperty<std::vector<Real>> & _screw_slip_direction;

  // Accumulated equivalent plastic strain
  std::vector<Real> _slip_resistance_increment;

  // Increment of increased resistance for each slip system
  const MaterialProperty<RankTwoTensor> & _deformation_gradient;
  const MaterialProperty<RankTwoTensor> & _plastic_deformation_gradient;

  // Dislocation velocity
  MaterialProperty<std::vector<Real>> & _dislo_velocity;

  MaterialProperty<Real> & _accumulated_equivalent_plastic_strain;
  const MaterialProperty<Real> & _accumulated_equivalent_plastic_strain_old;

  const enum class TwoSlipCheck { yes, no } _is_two_slips;
};
