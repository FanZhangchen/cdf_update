//* This file is for continuum dislocation density field-based theory
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation
//* 6 Jan 2024

#include "CrystalPlasticityBussoUpdate.h"
#include "libmesh/int_range.h"

registerMooseObject("TensorMechanicsApp", CrystalPlasticityBussoUpdate);

InputParameters
CrystalPlasticityBussoUpdate::validParams()
{
  InputParameters params = CrystalPlasticityDislocationUpdateBase::validParams();
  params.addClassDescription("Busso's version of crystal plasticity.");

  params.addParam<Real>("r", 8.314, "Latent hardening coefficient");
  params.addParam<Real>("temperature", 22.0, "temperature in Celsius");
  params.addParam<Real>("p", 0.3, "exponent for slip rate in busso's flow rule");
  params.addParam<Real>("q", 1.3, "exponent for slip rate in busso's flow rule");
  params.addParam<Real>("f0", 286000.0, "thermal activation energy");
  params.addParam<Real>("gdot0", 1.0e6, "initial slip rate");
  params.addParam<Real>("tau_0", 120.0, "initial lattice friction strength of the material");
  params.addParam<Real>("burgers", 2.54e-7, "magnitude of burgers vector");
  params.addParam<Real>("shear_modulus", 45000.0, "shear_modulus");
  params.addParam<Real>("boltzmann", 1.38065e-23, "The Boltzmann Constant");

  params.addParam<Real>("dlamb", 0.3, "initial slip rate");
  params.addParam<Real>("w1", 1.5, "cross-hardening constants, adopted from Cheong2004");
  params.addParam<Real>("w2", 1.2, "cross-hardening constants, adopted from Cheong2004");

  params.addCoupledVar("rho_edge_pos_1", 0.0, "Positive edge dislocation density: slip system 1");
  params.addCoupledVar("rho_edge_neg_1", 0.0, "Negative edge dislocation density: slip system 1");
  params.addCoupledVar("rho_edge_pos_2", 0.0, "Positive edge dislocation density: slip system 2");
  params.addCoupledVar("rho_edge_neg_2", 0.0, "Negative edge dislocation density: slip system 2");

  return params;
}

CrystalPlasticityBussoUpdate::CrystalPlasticityBussoUpdate(
    const InputParameters & parameters)
  : CrystalPlasticityDislocationUpdateBase(parameters),
    // Constitutive values
    _r(getParam<Real>("r")),
    _temperature(getParam<Real>("temperature")),
    _p(getParam<Real>("p")),
    _q(getParam<Real>("q")),
    _f0(getParam<Real>("f0")),
    _gdot0(getParam<Real>("gdot0")),
    _tau_0(getParam<Real>("tau_0")),
    _burgers(getParam<Real>("burgers")),
    _shear_modulus(getParam<Real>("shear_modulus")),
    _boltzmann(getParam<Real>("boltzmann")),

    _dlamb(getParam<Real>("dlamb")),
    _w1(getParam<Real>("w1")),
    _w2(getParam<Real>("w2")),

    _backstress(_number_slip_systems),

    _rho_edge_pos_1(coupledValue("rho_edge_pos_1")),

    _grad_rhoep1(coupledGradient("rho_edge_pos_1")), // Coupled rhoep gradient

    _rho_edge_neg_1(coupledValue("rho_edge_neg_1")),

    _grad_rhoen1(coupledGradient("rho_edge_neg_1")), // Coupled rhoen gradient

    _rho_edge_pos_2(coupledValue("rho_edge_pos_2")),

    _grad_rhoep2(coupledGradient("rho_edge_pos_2")), // Coupled rhoep gradient

    _rho_edge_neg_2(coupledValue("rho_edge_neg_2")),

    _grad_rhoen2(coupledGradient("rho_edge_neg_2")), // Coupled rhoen gradient

    _edge_slip_direction(
        declareProperty<std::vector<Real>>("edge_slip_direction")), // Edge slip directions
    _screw_slip_direction(
        declareProperty<std::vector<Real>>("screw_slip_direction")), // Screw slip direction

    _deformation_gradient(
      getMaterialProperty<RankTwoTensor>(_base_name + "deformation_gradient")),
    _plastic_deformation_gradient(
      getMaterialProperty<RankTwoTensor>("plastic_deformation_gradient")),

    _dislo_velocity(declareProperty<std::vector<Real>>("dislo_velocity")), // Dislocation velocity

    _accumulated_equivalent_plastic_strain(
      declareProperty<Real>(_base_name + "accumulated_equivalent_plastic_strain")),
    _accumulated_equivalent_plastic_strain_old(
      getMaterialPropertyOld<Real>(_base_name + "accumulated_equivalent_plastic_strain"))
    
{
}

void
CrystalPlasticityBussoUpdate::initQpStatefulProperties()
{
  CrystalPlasticityDislocationUpdateBase::initQpStatefulProperties();

  std::vector<Real> rho_edge_pos(_number_slip_systems);
  std::vector<Real> rho_edge_neg(_number_slip_systems);

  rho_edge_pos[0] = _rho_edge_pos_1[_qp];
  rho_edge_pos[1] = _rho_edge_pos_2[_qp];
  // rho_edge_pos[2] = _rho_edge_pos_3[_qp];
  // rho_edge_pos[3] = _rho_edge_pos_4[_qp];
  // rho_edge_pos[4] = _rho_edge_pos_5[_qp];
  // rho_edge_pos[5] = _rho_edge_pos_6[_qp];
  // rho_edge_pos[6] = _rho_edge_pos_7[_qp];
  // rho_edge_pos[7] = _rho_edge_pos_8[_qp];
  // rho_edge_pos[8] = _rho_edge_pos_9[_qp];
  // rho_edge_pos[9] = _rho_edge_pos_10[_qp];
  // rho_edge_pos[10] = _rho_edge_pos_11[_qp];
  // rho_edge_pos[11] = _rho_edge_pos_12[_qp];

  rho_edge_neg[0] = _rho_edge_neg_1[_qp];
  rho_edge_neg[1] = _rho_edge_neg_2[_qp];
  // rho_edge_neg[2] = _rho_edge_neg_3[_qp];
  // rho_edge_neg[3] = _rho_edge_neg_4[_qp];
  // rho_edge_neg[4] = _rho_edge_neg_5[_qp];
  // rho_edge_neg[5] = _rho_edge_neg_6[_qp];
  // rho_edge_neg[6] = _rho_edge_neg_7[_qp];
  // rho_edge_neg[7] = _rho_edge_neg_8[_qp];
  // rho_edge_neg[8] = _rho_edge_neg_9[_qp];
  // rho_edge_neg[9] = _rho_edge_neg_10[_qp];
  // rho_edge_neg[10] = _rho_edge_neg_11[_qp];
  // rho_edge_neg[11] = _rho_edge_neg_12[_qp];

  // Set initial slip resistance
  for (const auto i : make_range(_number_slip_systems))
  {
    Real initial_hardening_total_dislocation_density = 0.0;
    for (const auto j : make_range(_number_slip_systems))
    {
      //mooseWarning("x0-",i,j);
      if (i == j) // self vs. latent hardening
        initial_hardening_total_dislocation_density +=
           (_w1 + 1.0 - _w2) * (rho_edge_pos[j] + rho_edge_neg[j]);
           // (_w1 + 1.0 - _w2) * (_edge_dislocation_density[_qp][j] + _screw_dislocation_density[_qp][j]); // self hardening
      else
        initial_hardening_total_dislocation_density +=
             _w1 * (rho_edge_pos[j] + rho_edge_neg[j]);
              // _w1   * (_edge_dislocation_density[_qp][j] + _screw_dislocation_density[_qp][j]); // latent hardening
    }
    _slip_resistance[_qp][i] = _dlamb * _shear_modulus * _burgers * 
                            std::sqrt(initial_hardening_total_dislocation_density);
  }

  _edge_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
  _screw_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
}

void
CrystalPlasticityBussoUpdate::calculateSchmidTensor(
    const unsigned int & number_slip_systems,
    const std::vector<RealVectorValue> & plane_normal_vector,
    const std::vector<RealVectorValue> & direction_vector,
    std::vector<RankTwoTensor> & schmid_tensor,
    const RankTwoTensor & crysrot)
{
  std::vector<RealVectorValue> local_direction_vector, local_plane_normal;
  local_direction_vector.resize(number_slip_systems);
  local_plane_normal.resize(number_slip_systems);

  // Temporary directions and normals to calculate
  // screw dislocation slip direction
  RealVectorValue temp_mo;
  RealVectorValue temp_no;
  RealVectorValue temp_screw_mo;

  // Update slip direction with crystal orientation
  for (const auto i : make_range(_number_slip_systems))
  {
    local_direction_vector[i].zero();
    local_plane_normal[i].zero();

    for (const auto j : make_range(LIBMESH_DIM))
      for (const auto k : make_range(LIBMESH_DIM))
      {
        local_direction_vector[i](j) =
            local_direction_vector[i](j) + crysrot(j, k) * direction_vector[i](k);

        local_plane_normal[i](j) =
            local_plane_normal[i](j) + crysrot(j, k) * plane_normal_vector[i](k);
      }

    // Calculate Schmid tensor
    for (const auto j : make_range(LIBMESH_DIM))
      for (const auto k : make_range(LIBMESH_DIM))
      {
        schmid_tensor[i](j, k) = local_direction_vector[i](j) * local_plane_normal[i](k);
      }
  }

  _edge_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
  _screw_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);

  // Store slip direction (already normalized)
  // for edge and screw dislocations
  // to couple with dislocation transport
  for (const auto i : make_range(_number_slip_systems))
  {
    for (const auto j : make_range(LIBMESH_DIM))
    {
      temp_mo(j) = local_direction_vector[i](j);
      temp_no(j) = local_plane_normal[i](j);
    }

    temp_screw_mo = temp_mo.cross(temp_no);

    for (const auto j : make_range(LIBMESH_DIM))
    {
      _edge_slip_direction[_qp][i * LIBMESH_DIM + j] = local_direction_vector[i](j);
      _screw_slip_direction[_qp][i * LIBMESH_DIM + j] = temp_screw_mo(j);
    }
  } 
}

void
CrystalPlasticityBussoUpdate::setInitialConstitutiveVariableValues()
{
  // No need for this subroutine
}

void
CrystalPlasticityBussoUpdate::setSubstepConstitutiveVariableValues()
{
  // No need for this subroutine
}

bool
CrystalPlasticityBussoUpdate::calculateSlipRate()
{

  calculateSlipResistance();

  std::vector<Real> rho_edge_pos(_number_slip_systems);
  std::vector<Real> rho_edge_neg(_number_slip_systems);
  // std::vector<Real> rho_screw_pos(_nss);
  // std::vector<Real> rho_screw_neg(_nss);

  std::vector<Real> rho_edge_pos_grad_x(_number_slip_systems);
  std::vector<Real> rho_edge_neg_grad_x(_number_slip_systems);
  std::vector<Real> rho_edge_pos_grad_y(_number_slip_systems);
  std::vector<Real> rho_edge_neg_grad_y(_number_slip_systems);
  std::vector<Real> rho_edge_pos_grad_z(_number_slip_systems);
  std::vector<Real> rho_edge_neg_grad_z(_number_slip_systems);

  // Assign dislocation density vectors
  rho_edge_pos[0] = _rho_edge_pos_1[_qp];
  rho_edge_pos[1] = _rho_edge_pos_2[_qp];
  // rho_edge_pos[2] = _rho_edge_pos_3[_qp];
  // rho_edge_pos[3] = _rho_edge_pos_4[_qp];
  // rho_edge_pos[4] = _rho_edge_pos_5[_qp];
  // rho_edge_pos[5] = _rho_edge_pos_6[_qp];
  // rho_edge_pos[6] = _rho_edge_pos_7[_qp];
  // rho_edge_pos[7] = _rho_edge_pos_8[_qp];
  // rho_edge_pos[8] = _rho_edge_pos_9[_qp];
  // rho_edge_pos[9] = _rho_edge_pos_10[_qp];
  // rho_edge_pos[10] = _rho_edge_pos_11[_qp];
  // rho_edge_pos[11] = _rho_edge_pos_12[_qp];

  rho_edge_neg[0] = _rho_edge_neg_1[_qp];
  rho_edge_neg[1] = _rho_edge_neg_2[_qp];
  // rho_edge_neg[2] = _rho_edge_neg_3[_qp];
  // rho_edge_neg[3] = _rho_edge_neg_4[_qp];
  // rho_edge_neg[4] = _rho_edge_neg_5[_qp];
  // rho_edge_neg[5] = _rho_edge_neg_6[_qp];
  // rho_edge_neg[6] = _rho_edge_neg_7[_qp];
  // rho_edge_neg[7] = _rho_edge_neg_8[_qp];
  // rho_edge_neg[8] = _rho_edge_neg_9[_qp];
  // rho_edge_neg[9] = _rho_edge_neg_10[_qp];
  // rho_edge_neg[10] = _rho_edge_neg_11[_qp];
  // rho_edge_neg[11] = _rho_edge_neg_12[_qp];

  // Assigin dislocation density gradient vectors
  rho_edge_pos_grad_x[0] = _grad_rhoep1[_qp](0);
  rho_edge_pos_grad_x[1] = _grad_rhoep2[_qp](0);

  rho_edge_neg_grad_x[0] = _grad_rhoen1[_qp](0);
  rho_edge_neg_grad_x[1] = _grad_rhoen2[_qp](0);

  rho_edge_pos_grad_y[0] = _grad_rhoep1[_qp](1);
  rho_edge_pos_grad_y[1] = _grad_rhoep2[_qp](1);

  rho_edge_neg_grad_y[0] = _grad_rhoen1[_qp](1);
  rho_edge_neg_grad_y[1] = _grad_rhoen2[_qp](1);

  rho_edge_pos_grad_z[0] = _grad_rhoep1[_qp](2);
  rho_edge_pos_grad_z[1] = _grad_rhoep2[_qp](2);

  rho_edge_neg_grad_z[0] = _grad_rhoen1[_qp](2);
  rho_edge_neg_grad_z[1] = _grad_rhoen2[_qp](2);

  Real theta = _temperature + 273.15;
  Real RhoTotSlip;
  for (const auto i : make_range(_number_slip_systems))
  {

    RhoTotSlip = rho_edge_pos[i] + rho_edge_neg[i];

    _backstress(i) =
        _burgers * _shear_modulus * (rho_edge_pos_grad_x[i] / std::cos(60.0*3.1415926/180) - rho_edge_neg_grad_x[i] / std::cos(60.0*3.1415926/180) 
          + rho_edge_pos_grad_y[i] / std::sin(60.0*3.1415926/180) - rho_edge_neg_grad_y[i] / std::sin(60.0*3.1415926/180)) / RhoTotSlip;

    Real driving_force = std::abs(_tau[_qp][i]-_backstress(i)) - _slip_resistance[_qp][i];

    if (driving_force < _zero_tol)
    {
      _slip_increment[_qp][i] = 0.0;
      //_slip_increment[_qp][i] =
        //    _gdot0 * std::exp( - _f0 / _r / theta ) * std::copysign(1.0, _tau[_qp][i]);
    }
    else
    {
      _slip_increment[_qp][i] =
            _gdot0 * std::exp( - _f0 / _boltzmann / theta * 
                std::pow((1.0 - std::pow((driving_force / _tau_0 ) , _p)) , _q))
                                      * std::copysign(1.0, _tau[_qp][i]-_backstress(i));

      // _slip_increment[_qp][i] =
      //       _gdot0 * std::exp( - _f0 / _r / theta * 
      //             std::pow(1.0 - std::pow((driving_force / _tau_0 ) , _p) , _q))
      //                                 * std::copysign(1.0, _tau[_qp][i]);
    }

    if (std::abs(_slip_increment[_qp][i]) * _substep_dt > _slip_incr_tol)
    {
      if (_print_convergence_message)
        mooseWarning("Maximum allowable slip increment exceeded ",
                     std::abs(_slip_increment[_qp][i]) * _substep_dt);

      return false;
    }
  }
  calculateDislocationVelocity();
  return true;
}

void
CrystalPlasticityBussoUpdate::calculateSlipResistance()
{
  
  std::vector<Real> rho_edge_pos(_number_slip_systems);
  std::vector<Real> rho_edge_neg(_number_slip_systems);

  rho_edge_pos[0] = _rho_edge_pos_1[_qp];
  rho_edge_pos[1] = _rho_edge_pos_2[_qp];
  // rho_edge_pos[2] = _rho_edge_pos_3[_qp];
  // rho_edge_pos[3] = _rho_edge_pos_4[_qp];
  // rho_edge_pos[4] = _rho_edge_pos_5[_qp];
  // rho_edge_pos[5] = _rho_edge_pos_6[_qp];
  // rho_edge_pos[6] = _rho_edge_pos_7[_qp];
  // rho_edge_pos[7] = _rho_edge_pos_8[_qp];
  // rho_edge_pos[8] = _rho_edge_pos_9[_qp];
  // rho_edge_pos[9] = _rho_edge_pos_10[_qp];
  // rho_edge_pos[10] = _rho_edge_pos_11[_qp];
  // rho_edge_pos[11] = _rho_edge_pos_12[_qp];

  rho_edge_neg[0] = _rho_edge_neg_1[_qp];
  rho_edge_neg[1] = _rho_edge_neg_2[_qp];
  // rho_edge_neg[2] = _rho_edge_neg_3[_qp];
  // rho_edge_neg[3] = _rho_edge_neg_4[_qp];
  // rho_edge_neg[4] = _rho_edge_neg_5[_qp];
  // rho_edge_neg[5] = _rho_edge_neg_6[_qp];
  // rho_edge_neg[6] = _rho_edge_neg_7[_qp];
  // rho_edge_neg[7] = _rho_edge_neg_8[_qp];
  // rho_edge_neg[8] = _rho_edge_neg_9[_qp];
  // rho_edge_neg[9] = _rho_edge_neg_10[_qp];
  // rho_edge_neg[10] = _rho_edge_neg_11[_qp];
  // rho_edge_neg[11] = _rho_edge_neg_12[_qp];

  for (const auto i : make_range(_number_slip_systems))
  {
    Real hardening_total_dislocation_density = 0.0;
    for (const auto j : make_range(_number_slip_systems))
    {
      if (i == j) // self vs. latent hardening
        hardening_total_dislocation_density +=
            (_w1 + 1.0 - _w2)* (rho_edge_pos[j] + rho_edge_neg[j]); // self hardening
      else
        hardening_total_dislocation_density +=
              _w1  * (rho_edge_pos[j] + rho_edge_neg[j]); // latent hardening
    }
    _slip_resistance[_qp][i] = _dlamb * _shear_modulus *_burgers * 
                            std::sqrt(hardening_total_dislocation_density); 
  }
}

void
CrystalPlasticityBussoUpdate::calculateDislocationVelocity()
{
  std::vector<Real> rho_edge_pos(_number_slip_systems);
  std::vector<Real> rho_edge_neg(_number_slip_systems);

  _dislo_velocity[_qp].resize(_number_slip_systems);

  Real total_dislocation_density = 0.0; // total dislocation density in the current slip system

  rho_edge_pos[0] = _rho_edge_pos_1[_qp];
  rho_edge_pos[1] = _rho_edge_pos_2[_qp];
  // rho_edge_pos[2] = _rho_edge_pos_3[_qp];
  // rho_edge_pos[3] = _rho_edge_pos_4[_qp];
  // rho_edge_pos[4] = _rho_edge_pos_5[_qp];
  // rho_edge_pos[5] = _rho_edge_pos_6[_qp];
  // rho_edge_pos[6] = _rho_edge_pos_7[_qp];
  // rho_edge_pos[7] = _rho_edge_pos_8[_qp];
  // rho_edge_pos[8] = _rho_edge_pos_9[_qp];
  // rho_edge_pos[9] = _rho_edge_pos_10[_qp];
  // rho_edge_pos[10] = _rho_edge_pos_11[_qp];
  // rho_edge_pos[11] = _rho_edge_pos_12[_qp];

  rho_edge_neg[0] = _rho_edge_neg_1[_qp];
  rho_edge_neg[1] = _rho_edge_neg_2[_qp];
  // rho_edge_neg[2] = _rho_edge_neg_3[_qp];
  // rho_edge_neg[3] = _rho_edge_neg_4[_qp];
  // rho_edge_neg[4] = _rho_edge_neg_5[_qp];
  // rho_edge_neg[5] = _rho_edge_neg_6[_qp];
  // rho_edge_neg[6] = _rho_edge_neg_7[_qp];
  // rho_edge_neg[7] = _rho_edge_neg_8[_qp];
  // rho_edge_neg[8] = _rho_edge_neg_9[_qp];
  // rho_edge_neg[9] = _rho_edge_neg_10[_qp];
  // rho_edge_neg[10] = _rho_edge_neg_11[_qp];
  // rho_edge_neg[11] = _rho_edge_neg_12[_qp];

  for (const auto i : make_range(_number_slip_systems))
  {

    total_dislocation_density = rho_edge_pos[i] + rho_edge_neg[i]; // + rho_screw_pos[i] + rho_screw_neg[i];

    Real driving_force = std::abs(_tau[_qp][i]-_backstress(i)) - _slip_resistance[_qp][i];
    // }

    if (driving_force > _zero_tol)
    { // driving force less than 0, the dislocation could not move
      _dislo_velocity[_qp][i] = _slip_increment[_qp][i] / _burgers / total_dislocation_density;    
    }
    else
    { // Case below critical resolved shear stress
      _dislo_velocity[_qp][i] = 0.0;
    }

  }
}

void
CrystalPlasticityBussoUpdate::calculateEquivalentSlipIncrement(
    RankTwoTensor & equivalent_slip_increment)
{
  CrystalPlasticityDislocationUpdateBase::calculateEquivalentSlipIncrement(equivalent_slip_increment);

  calculateAccumulatedEquivalentPlasticStrain();
}

void
CrystalPlasticityBussoUpdate::calculateAccumulatedEquivalentPlasticStrain()
{
  RankTwoTensor plastic_strain_rate, term1, term2;
  RankTwoTensor elastic_deformation_gradient, inverse_elastic_deformation_gradient, inverse_plastic_deformation_gradient;

  inverse_plastic_deformation_gradient = _plastic_deformation_gradient[_qp].inverse();
  elastic_deformation_gradient = _deformation_gradient[_qp] * inverse_plastic_deformation_gradient;
  inverse_elastic_deformation_gradient = elastic_deformation_gradient.inverse();
  for (const auto i : make_range(_number_slip_systems))
  {
    term1 = _slip_increment[_qp][i] * elastic_deformation_gradient * 
                               _flow_direction[_qp][i] * inverse_elastic_deformation_gradient * _substep_dt;

    term2 = _slip_increment[_qp][i] * inverse_elastic_deformation_gradient.transpose() * 
                               _flow_direction[_qp][i].transpose() * elastic_deformation_gradient.transpose() * _substep_dt;
    plastic_strain_rate += 0.5 * (term1 + term2);
  }
  _accumulated_equivalent_plastic_strain[_qp] = _accumulated_equivalent_plastic_strain_old[_qp] 
             + std::sqrt( 2.0/3.0 * plastic_strain_rate.doubleContraction(plastic_strain_rate));
}

void
CrystalPlasticityBussoUpdate::calculateConstitutiveSlipDerivative(
    std::vector<Real> & dslip_dtau)
{
  Real theta = _temperature + 273.15;
  for (const auto i : make_range(_number_slip_systems))
  {
    Real driving_force = std::abs(_tau[_qp][i]-_backstress(i)) - _slip_resistance[_qp][i];
    Real u = 0.0, uprime = 0.0, vprime = 0.0;
    if (driving_force < _zero_tol)
      dslip_dtau[i] = 0.0;
    else
      {
        u = driving_force / _tau_0;
        uprime = std::pow(u, _p - 1.0) * std::copysign(1.0, (_tau[_qp][i]-_backstress(i)));
        vprime = std::pow((1.0 - std::pow(u, _p)),_q - 1.0);
        dslip_dtau[i] =
            _gdot0 * _p * _q * _f0 / _boltzmann / theta *
            std::exp(-_f0 / _boltzmann / theta *
                     std::pow((1.0 - std::pow(u,_p)),
                              _q)) * uprime * vprime
               * _substep_dt;
      }
      // {
      //   u = driving_force / _tau_0; 
      //   uprime = 1.0 /_tau_0 * std::copysign(1.0, _tau[_qp][i]);
      //   vprime = _f0 / _r / theta * _q * _p * uprime * std::pow(u, _p - 1.0) * std::pow(1.0- std::pow(u,_p), _q-1.0);
      //   dslip_dtau[i] = _gdot0 * vprime  * std::exp( - _f0 / _r / theta * std::pow(1.0- std::pow(u,_p),_q) )  *_substep_dt; 
      // }
    // mooseWarning("tau=",_tau[_qp][i]);
  }
}

bool
CrystalPlasticityBussoUpdate::areConstitutiveStateVariablesConverged()
{
  return true;
}

void
CrystalPlasticityBussoUpdate::updateSubstepConstitutiveVariableValues()
{
  // No need for this subroutine
}

void
CrystalPlasticityBussoUpdate::cacheStateVariablesBeforeUpdate()
{
  // No need for this subroutine
}

void
CrystalPlasticityBussoUpdate::calculateStateVariableEvolutionRateComponent()
{
  // No need for this subroutine
}

bool
CrystalPlasticityBussoUpdate::updateStateVariables()
{
  return true;
}
