//* This file is for continuum dislocation density field-based theory
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation
//* 2 Apr 2024

#include "CrystalPlasticityBussoUpdateSingleSlipSys.h"
#include "libmesh/int_range.h"

registerMooseObject("SolidMechanicsApp", CrystalPlasticityBussoUpdateSingleSlipSys);

InputParameters
CrystalPlasticityBussoUpdateSingleSlipSys::validParams()
{
  InputParameters params = CrystalPlasticityDislocationUpdateBase::validParams();
  params.addClassDescription("Busso's version of crystal plasticity.");

  params.addParam<Real>("r", 8.314, "Latent hardening coefficient");
  params.addParam<Real>("temperature", 22.0, "temperature in Celsius");
  params.addParam<Real>("p", 0.3, "exponent for slip rate in busso's flow rule");
  params.addParam<Real>("q", 1.3, "exponent for slip rate in busso's flow rule");
  params.addParam<Real>("f0", 2.77e-19, "thermal activation energy");
  params.addParam<Real>("gdot0", 1.0e6, "initial slip rate");
  params.addParam<Real>("tau_0", 120.0, "initial lattice friction strength of the material");
  params.addParam<Real>("burgers", 2.54e-7, "magnitude of burgers vector");
  params.addParam<Real>("shear_modulus", 45000.0, "shear_modulus");
  params.addParam<Real>("boltzmann", 1.38065e-23, "The Boltzmann Constant");
  params.addParam<Real>("scaling_Cb", 1.0, "The scaling parameter for the backstress");

  params.addParam<Real>("dlamb", 0.3, "initial slip rate");
  params.addParam<Real>("w1", 1.5, "cross-hardening constants, adopted from Cheong2004");
  params.addParam<Real>("w2", 1.2, "cross-hardening constants, adopted from Cheong2004");

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

  MooseEnum is_two_slips("yes no", "yes");
  params.addRequiredParam<MooseEnum>("is_two_slips", is_two_slips, "check two slips case.");

  return params;
}

CrystalPlasticityBussoUpdateSingleSlipSys::CrystalPlasticityBussoUpdateSingleSlipSys(const InputParameters & parameters)
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
    _scaling_Cb(getParam<Real>("scaling_Cb")),

    _dlamb(getParam<Real>("dlamb")),
    _w1(getParam<Real>("w1")),
    _w2(getParam<Real>("w2")),

    _backstress(_number_slip_systems),

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

    _edge_slip_direction(
        declareProperty<std::vector<Real>>("edge_slip_direction")), // Edge slip directions
    _screw_slip_direction(
        declareProperty<std::vector<Real>>("screw_slip_direction")), // Screw slip direction

    _deformation_gradient(getMaterialProperty<RankTwoTensor>(_base_name + "deformation_gradient")),
    _plastic_deformation_gradient(
        getMaterialProperty<RankTwoTensor>("plastic_deformation_gradient")),

    _dislo_velocity(declareProperty<std::vector<Real>>("dislo_velocity")), // Dislocation velocity

    _accumulated_equivalent_plastic_strain(
        declareProperty<Real>(_base_name + "accumulated_equivalent_plastic_strain")),
    _accumulated_equivalent_plastic_strain_old(
        getMaterialPropertyOld<Real>(_base_name + "accumulated_equivalent_plastic_strain")),

    _is_two_slips(getParam<MooseEnum>("is_two_slips").getEnum<TwoSlipCheck>())

{
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::initQpStatefulProperties()
{
  CrystalPlasticityDislocationUpdateBase::initQpStatefulProperties();

  std::vector<Real> rho_edge_Q1(_number_slip_systems);
  std::vector<Real> rho_edge_Q2(_number_slip_systems);
  std::vector<Real> rho_edge_Q3(_number_slip_systems);
  std::vector<Real> rho_edge_Q4(_number_slip_systems);
  std::vector<Real> rho_screw_Q1(_number_slip_systems);
  std::vector<Real> rho_screw_Q2(_number_slip_systems);
  std::vector<Real> rho_screw_Q3(_number_slip_systems);
  std::vector<Real> rho_screw_Q4(_number_slip_systems);

  rho_edge_Q1[0] = _edge_dislo_den_1_Q1[_qp];

  rho_edge_Q2[0] = _edge_dislo_den_1_Q2[_qp];

  rho_edge_Q3[0] = _edge_dislo_den_1_Q3[_qp];
  
  rho_edge_Q4[0] = _edge_dislo_den_1_Q4[_qp];
  
  rho_screw_Q1[0] = _screw_dislo_den_1_Q1[_qp];

  rho_screw_Q2[0] = _screw_dislo_den_1_Q2[_qp];

  rho_screw_Q3[0] = _screw_dislo_den_1_Q3[_qp];
  
  rho_screw_Q4[0] = _screw_dislo_den_1_Q4[_qp];

  // Set initial slip resistance
  for (const auto i : make_range(_number_slip_systems))
  {
    Real initial_hardening_total_dislocation_density = 0.0;
    for (const auto j : make_range(_number_slip_systems))
    {
      // mooseWarning("x0-",i,j);
      if (i == j) // self vs. latent hardening
        initial_hardening_total_dislocation_density +=
            (_w1 + 1.0 - _w2) *
            (rho_edge_Q1[j] + rho_edge_Q2[j] + rho_edge_Q3[j] + rho_edge_Q4[j] + rho_screw_Q1[j] +
             rho_screw_Q2[j] + rho_screw_Q3[j] + rho_screw_Q4[j]);
      // (_w1 + 1.0 - _w2) * (_edge_dislocation_density[_qp][j] +
      // _screw_dislocation_density[_qp][j]); // self hardening
      else
        initial_hardening_total_dislocation_density +=
            _w1 * (rho_edge_Q1[j] + rho_edge_Q2[j] + rho_edge_Q3[j] + rho_edge_Q4[j] +
                   rho_screw_Q1[j] + rho_screw_Q2[j] + rho_screw_Q3[j] + rho_screw_Q4[j]);
      // _w1   * (_edge_dislocation_density[_qp][j] + _screw_dislocation_density[_qp][j]); // latent
      // hardening
    }
    _slip_resistance[_qp][i] =
        _dlamb * _shear_modulus * _burgers * std::sqrt(initial_hardening_total_dislocation_density);
  }

  _edge_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
  _screw_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::calculateSchmidTensor(
    const unsigned int & number_slip_systems,
    const std::vector<RealVectorValue> & plane_normal_vector,
    const std::vector<RealVectorValue> & direction_vector,
    std::vector<RankTwoTensor> & schmid_tensor,
    const RankTwoTensor & crysrot)
{
  std::vector<RealVectorValue> local_direction_vector, local_plane_normal;
  local_direction_vector.resize(number_slip_systems);
  local_plane_normal.resize(number_slip_systems);
  // mooseWarning("number_slip_systems", number_slip_systems);

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
      // s alpha
      _edge_slip_direction[_qp][i * LIBMESH_DIM + j] = local_direction_vector[i](j);
      // e alpha
      _screw_slip_direction[_qp][i * LIBMESH_DIM + j] = temp_screw_mo(j);
    }
  }
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::setInitialConstitutiveVariableValues()
{
  // No need for this subroutine
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::setSubstepConstitutiveVariableValues()
{
  // No need for this subroutine
}

bool
CrystalPlasticityBussoUpdateSingleSlipSys::calculateSlipRate()
{

  calculateSlipResistance();

  // std::vector<Real> global_x, global_y, global_z;
  // global_x.resize(LIBMESH_DIM);
  // global_y.resize(LIBMESH_DIM);
  // global_z.resize(LIBMESH_DIM);

  // // Unit vector along the x-axis
  // global_x[0] = 1.0;
  // global_x[1] = 0.0;
  // global_x[2] = 0.0;

  // // Unit vector along the y-axis
  // global_y[0] = 0.0;
  // global_y[1] = 1.0;
  // global_y[2] = 0.0;

  // // Unit vector along the z-axis
  // global_z[0] = 0.0;
  // global_z[1] = 0.0;
  // global_z[2] = 1.0;

  std::vector<Real> local_edge_slip_direction, local_screw_slip_direction;
  local_edge_slip_direction.resize(LIBMESH_DIM);
  local_screw_slip_direction.resize(LIBMESH_DIM);

  std::vector<Real> rho_edge_Q1(_number_slip_systems);
  std::vector<Real> rho_edge_Q2(_number_slip_systems);
  std::vector<Real> rho_edge_Q3(_number_slip_systems);
  std::vector<Real> rho_edge_Q4(_number_slip_systems);
  std::vector<Real> rho_screw_Q1(_number_slip_systems);
  std::vector<Real> rho_screw_Q2(_number_slip_systems);
  std::vector<Real> rho_screw_Q3(_number_slip_systems);
  std::vector<Real> rho_screw_Q4(_number_slip_systems);

  std::vector<Real> grad_x_rho_edge_Q1(_number_slip_systems);
  std::vector<Real> grad_y_rho_edge_Q1(_number_slip_systems);
  std::vector<Real> grad_z_rho_edge_Q1(_number_slip_systems);
  std::vector<Real> grad_x_rho_edge_Q2(_number_slip_systems);
  std::vector<Real> grad_y_rho_edge_Q2(_number_slip_systems);
  std::vector<Real> grad_z_rho_edge_Q2(_number_slip_systems);
  std::vector<Real> grad_x_rho_edge_Q3(_number_slip_systems);
  std::vector<Real> grad_y_rho_edge_Q3(_number_slip_systems);
  std::vector<Real> grad_z_rho_edge_Q3(_number_slip_systems);
  std::vector<Real> grad_x_rho_edge_Q4(_number_slip_systems);
  std::vector<Real> grad_y_rho_edge_Q4(_number_slip_systems);
  std::vector<Real> grad_z_rho_edge_Q4(_number_slip_systems);

  std::vector<Real> grad_x_rho_screw_Q1(_number_slip_systems);
  std::vector<Real> grad_y_rho_screw_Q1(_number_slip_systems);
  std::vector<Real> grad_z_rho_screw_Q1(_number_slip_systems);
  std::vector<Real> grad_x_rho_screw_Q2(_number_slip_systems);
  std::vector<Real> grad_y_rho_screw_Q2(_number_slip_systems);
  std::vector<Real> grad_z_rho_screw_Q2(_number_slip_systems);
  std::vector<Real> grad_x_rho_screw_Q3(_number_slip_systems);
  std::vector<Real> grad_y_rho_screw_Q3(_number_slip_systems);
  std::vector<Real> grad_z_rho_screw_Q3(_number_slip_systems);
  std::vector<Real> grad_x_rho_screw_Q4(_number_slip_systems);
  std::vector<Real> grad_y_rho_screw_Q4(_number_slip_systems);
  std::vector<Real> grad_z_rho_screw_Q4(_number_slip_systems);

  // Assign dislocation density vectors
  rho_edge_Q1[0] = _edge_dislo_den_1_Q1[_qp];

  rho_edge_Q2[0] = _edge_dislo_den_1_Q2[_qp];

  rho_edge_Q3[0] = _edge_dislo_den_1_Q3[_qp];
  
  rho_edge_Q4[0] = _edge_dislo_den_1_Q4[_qp];
  
  rho_screw_Q1[0] = _screw_dislo_den_1_Q1[_qp];

  rho_screw_Q2[0] = _screw_dislo_den_1_Q2[_qp];

  rho_screw_Q3[0] = _screw_dislo_den_1_Q3[_qp];
  
  rho_screw_Q4[0] = _screw_dislo_den_1_Q4[_qp];

  // Assigin dislocation density gradient vectors Slip 1
  grad_x_rho_edge_Q1[0] = _grad_edge_dislo_den_1_Q1[_qp](0);
  grad_y_rho_edge_Q1[0] = _grad_edge_dislo_den_1_Q1[_qp](1);
  grad_z_rho_edge_Q1[0] = _grad_edge_dislo_den_1_Q1[_qp](2);

  grad_x_rho_edge_Q2[0] = _grad_edge_dislo_den_1_Q2[_qp](0);
  grad_y_rho_edge_Q2[0] = _grad_edge_dislo_den_1_Q2[_qp](1);
  grad_z_rho_edge_Q2[0] = _grad_edge_dislo_den_1_Q2[_qp](2);

  grad_x_rho_edge_Q3[0] = _grad_edge_dislo_den_1_Q3[_qp](0);
  grad_y_rho_edge_Q3[0] = _grad_edge_dislo_den_1_Q3[_qp](1);
  grad_z_rho_edge_Q3[0] = _grad_edge_dislo_den_1_Q3[_qp](2);

  grad_x_rho_edge_Q4[0] = _grad_edge_dislo_den_1_Q4[_qp](0);
  grad_y_rho_edge_Q4[0] = _grad_edge_dislo_den_1_Q4[_qp](1);
  grad_z_rho_edge_Q4[0] = _grad_edge_dislo_den_1_Q4[_qp](2);

  grad_x_rho_screw_Q1[0] = _grad_screw_dislo_den_1_Q1[_qp](0);
  grad_y_rho_screw_Q1[0] = _grad_screw_dislo_den_1_Q1[_qp](1);
  grad_z_rho_screw_Q1[0] = _grad_screw_dislo_den_1_Q1[_qp](2);

  grad_x_rho_screw_Q2[0] = _grad_screw_dislo_den_1_Q2[_qp](0);
  grad_y_rho_screw_Q2[0] = _grad_screw_dislo_den_1_Q2[_qp](1);
  grad_z_rho_screw_Q2[0] = _grad_screw_dislo_den_1_Q2[_qp](2);

  grad_x_rho_screw_Q3[0] = _grad_screw_dislo_den_1_Q3[_qp](0);
  grad_y_rho_screw_Q3[0] = _grad_screw_dislo_den_1_Q3[_qp](1);
  grad_z_rho_screw_Q3[0] = _grad_screw_dislo_den_1_Q3[_qp](2);

  grad_x_rho_screw_Q4[0] = _grad_screw_dislo_den_1_Q4[_qp](0);
  grad_y_rho_screw_Q4[0] = _grad_screw_dislo_den_1_Q4[_qp](1);
  grad_z_rho_screw_Q4[0] = _grad_screw_dislo_den_1_Q4[_qp](2);

  Real theta = _temperature + 273.15;
  Real RhoTotSlip;
  for (const auto i : make_range(_number_slip_systems))
  {

    RhoTotSlip = rho_edge_Q1[i] + rho_edge_Q2[i] + rho_edge_Q3[i] + rho_edge_Q4[i] +
                 rho_screw_Q1[i] + rho_screw_Q2[i] + rho_screw_Q3[i] + rho_screw_Q4[i];

    // mooseWarning("Edge slip direction vector",
    //                  _edge_slip_direction[_qp][i*LIBMESH_DIM]);

    if (_edge_slip_direction[_qp][i * LIBMESH_DIM] < 1.e-10)
      local_edge_slip_direction[0] = 0.0;
    else
      local_edge_slip_direction[0] = 1.0 / _edge_slip_direction[_qp][i * LIBMESH_DIM];

    if (_edge_slip_direction[_qp][i * LIBMESH_DIM + 1] < 1.e-10)
      local_edge_slip_direction[1] = 0.0;
    else
      local_edge_slip_direction[1] = 1.0 / _edge_slip_direction[_qp][i * LIBMESH_DIM + 1];

    if (_edge_slip_direction[_qp][i * LIBMESH_DIM + 2] < 1.e-10)
      local_edge_slip_direction[2] = 0.0;
    else
      local_edge_slip_direction[2] = 1.0 / _edge_slip_direction[_qp][i * LIBMESH_DIM + 2];

    if (_screw_slip_direction[_qp][i * LIBMESH_DIM] < 1.e-10)
      local_screw_slip_direction[0] = 0.0;
    else
      local_screw_slip_direction[0] = 1.0 / _screw_slip_direction[_qp][i * LIBMESH_DIM];

    if (_screw_slip_direction[_qp][i * LIBMESH_DIM + 1] < 1.e-10)
      local_screw_slip_direction[1] = 0.0;
    else
      local_screw_slip_direction[1] = 1.0 / _screw_slip_direction[_qp][i * LIBMESH_DIM + 1];

    if (_screw_slip_direction[_qp][i * LIBMESH_DIM + 2] < 1.e-10)
      local_screw_slip_direction[2] = 0.0;
    else
      local_screw_slip_direction[2] = 1.0 / _screw_slip_direction[_qp][i * LIBMESH_DIM + 2];

    _backstress(i) = _scaling_Cb * _burgers * _shear_modulus *
                     ((grad_x_rho_edge_Q1[i] * local_edge_slip_direction[0] +
                       grad_x_rho_edge_Q2[i] * local_edge_slip_direction[0] -
                       grad_x_rho_edge_Q3[i] * local_edge_slip_direction[0] -
                       grad_x_rho_edge_Q4[i] * local_edge_slip_direction[0] +
                       grad_y_rho_edge_Q1[i] * local_edge_slip_direction[1] +
                       grad_y_rho_edge_Q2[i] * local_edge_slip_direction[1] -
                       grad_y_rho_edge_Q3[i] * local_edge_slip_direction[1] -
                       grad_y_rho_edge_Q4[i] * local_edge_slip_direction[1] +
                       grad_z_rho_edge_Q1[i] * local_edge_slip_direction[2] +
                       grad_z_rho_edge_Q2[i] * local_edge_slip_direction[2] -
                       grad_z_rho_edge_Q3[i] * local_edge_slip_direction[2] -
                       grad_z_rho_edge_Q4[i] * local_edge_slip_direction[2]) +
                      (grad_x_rho_screw_Q1[i] * local_screw_slip_direction[0] -
                       grad_x_rho_screw_Q2[i] * local_screw_slip_direction[0] -
                       grad_x_rho_screw_Q3[i] * local_screw_slip_direction[0] +
                       grad_x_rho_screw_Q4[i] * local_screw_slip_direction[0] +
                       grad_y_rho_screw_Q1[i] * local_screw_slip_direction[1] -
                       grad_y_rho_screw_Q2[i] * local_screw_slip_direction[1] -
                       grad_y_rho_screw_Q3[i] * local_screw_slip_direction[1] +
                       grad_y_rho_screw_Q4[i] * local_screw_slip_direction[1] +
                       grad_z_rho_screw_Q1[i] * local_screw_slip_direction[2] -
                       grad_z_rho_screw_Q2[i] * local_screw_slip_direction[2] -
                       grad_z_rho_screw_Q3[i] * local_screw_slip_direction[2] +
                       grad_z_rho_screw_Q4[i] * local_screw_slip_direction[2])) /
                     RhoTotSlip;

    Real driving_force = std::abs(_tau[_qp][i] - _backstress(i)) - _slip_resistance[_qp][i];

    if (driving_force < _zero_tol)
    {
      _slip_increment[_qp][i] = 0.0;
    }
    else
    {
      _slip_increment[_qp][i] =
          _gdot0 *
          std::exp(-_f0 / _boltzmann / theta *
                   std::pow((1.0 - std::pow((driving_force / _tau_0), _p)), _q)) *
          std::copysign(1.0, _tau[_qp][i] - _backstress(i));
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
CrystalPlasticityBussoUpdateSingleSlipSys::calculateSlipResistance()
{

  std::vector<Real> rho_edge_Q1(_number_slip_systems);
  std::vector<Real> rho_edge_Q2(_number_slip_systems);
  std::vector<Real> rho_edge_Q3(_number_slip_systems);
  std::vector<Real> rho_edge_Q4(_number_slip_systems);
  std::vector<Real> rho_screw_Q1(_number_slip_systems);
  std::vector<Real> rho_screw_Q2(_number_slip_systems);
  std::vector<Real> rho_screw_Q3(_number_slip_systems);
  std::vector<Real> rho_screw_Q4(_number_slip_systems);

  // Assign dislocation density vectors
  rho_edge_Q1[0] = _edge_dislo_den_1_Q1[_qp];

  rho_edge_Q2[0] = _edge_dislo_den_1_Q2[_qp];

  rho_edge_Q3[0] = _edge_dislo_den_1_Q3[_qp];
  
  rho_edge_Q4[0] = _edge_dislo_den_1_Q4[_qp];
  
  rho_screw_Q1[0] = _screw_dislo_den_1_Q1[_qp];

  rho_screw_Q2[0] = _screw_dislo_den_1_Q2[_qp];

  rho_screw_Q3[0] = _screw_dislo_den_1_Q3[_qp];
  
  rho_screw_Q4[0] = _screw_dislo_den_1_Q4[_qp];

  for (const auto i : make_range(_number_slip_systems))
  {
    Real hardening_total_dislocation_density = 0.0;
    for (const auto j : make_range(_number_slip_systems))
    {
      if (i == j) // self vs. latent hardening
        hardening_total_dislocation_density +=
            (_w1 + 1.0 - _w2) *
            (rho_edge_Q1[j] + rho_edge_Q2[j] + rho_edge_Q3[j] + rho_edge_Q4[j] + rho_screw_Q1[j] +
             rho_screw_Q2[j] + rho_screw_Q3[j] + rho_screw_Q4[j]); // self hardening
      else
        hardening_total_dislocation_density +=
            _w1 *
            (rho_edge_Q1[j] + rho_edge_Q2[j] + rho_edge_Q3[j] + rho_edge_Q4[j] + rho_screw_Q1[j] +
             rho_screw_Q2[j] + rho_screw_Q3[j] + rho_screw_Q4[j]); // latent hardening
    }
    _slip_resistance[_qp][i] =
        _dlamb * _shear_modulus * _burgers * std::sqrt(hardening_total_dislocation_density);
  }
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::calculateDislocationVelocity()
{
  std::vector<Real> rho_edge_Q1(_number_slip_systems);
  std::vector<Real> rho_edge_Q2(_number_slip_systems);
  std::vector<Real> rho_edge_Q3(_number_slip_systems);
  std::vector<Real> rho_edge_Q4(_number_slip_systems);
  std::vector<Real> rho_screw_Q1(_number_slip_systems);
  std::vector<Real> rho_screw_Q2(_number_slip_systems);
  std::vector<Real> rho_screw_Q3(_number_slip_systems);
  std::vector<Real> rho_screw_Q4(_number_slip_systems);

  _dislo_velocity[_qp].resize(_number_slip_systems);

  Real total_dislocation_density = 0.0; // total dislocation density in the current slip system

  // Assign dislocation density vectors
  rho_edge_Q1[0] = _edge_dislo_den_1_Q1[_qp];

  rho_edge_Q2[0] = _edge_dislo_den_1_Q2[_qp];

  rho_edge_Q3[0] = _edge_dislo_den_1_Q3[_qp];
  
  rho_edge_Q4[0] = _edge_dislo_den_1_Q4[_qp];
  
  rho_screw_Q1[0] = _screw_dislo_den_1_Q1[_qp];

  rho_screw_Q2[0] = _screw_dislo_den_1_Q2[_qp];

  rho_screw_Q3[0] = _screw_dislo_den_1_Q3[_qp];
  
  rho_screw_Q4[0] = _screw_dislo_den_1_Q4[_qp];

  for (const auto i : make_range(_number_slip_systems))
  {

    total_dislocation_density = rho_edge_Q1[i] + rho_edge_Q2[i] + rho_edge_Q3[i] + rho_edge_Q4[i] +
                                rho_screw_Q1[i] + rho_screw_Q2[i] + rho_screw_Q3[i] +
                                rho_screw_Q4[i];

    Real driving_force = std::abs(_tau[_qp][i] - _backstress(i)) - _slip_resistance[_qp][i];
    // }

    if (driving_force > _zero_tol)
    { // driving force less than 0, the dislocation could not move
      _dislo_velocity[_qp][i] = _slip_increment[_qp][i] / _burgers / total_dislocation_density;
      // mooseWarning("_dislo_velocity", _dislo_velocity[_qp][i]);
    }
    else
    { // Case below critical resolved shear stress
      _dislo_velocity[_qp][i] = 0.0;
    }
  }
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::calculateEquivalentSlipIncrement(
    RankTwoTensor & equivalent_slip_increment)
{
  CrystalPlasticityDislocationUpdateBase::calculateEquivalentSlipIncrement(
      equivalent_slip_increment);

  calculateAccumulatedEquivalentPlasticStrain();
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::calculateAccumulatedEquivalentPlasticStrain()
{
  RankTwoTensor plastic_strain_rate, term1, term2;
  RankTwoTensor elastic_deformation_gradient, inverse_elastic_deformation_gradient,
      inverse_plastic_deformation_gradient;

  inverse_plastic_deformation_gradient = _plastic_deformation_gradient[_qp].inverse();
  elastic_deformation_gradient = _deformation_gradient[_qp] * inverse_plastic_deformation_gradient;
  inverse_elastic_deformation_gradient = elastic_deformation_gradient.inverse();
  for (const auto i : make_range(_number_slip_systems))
  {
    term1 = _slip_increment[_qp][i] * elastic_deformation_gradient * _flow_direction[_qp][i] *
            inverse_elastic_deformation_gradient * _substep_dt;

    term2 = _slip_increment[_qp][i] * inverse_elastic_deformation_gradient.transpose() *
            _flow_direction[_qp][i].transpose() * elastic_deformation_gradient.transpose() *
            _substep_dt;
    plastic_strain_rate += 0.5 * (term1 + term2);
  }
  _accumulated_equivalent_plastic_strain[_qp] =
      _accumulated_equivalent_plastic_strain_old[_qp] +
      std::sqrt(2.0 / 3.0 * plastic_strain_rate.doubleContraction(plastic_strain_rate));
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::calculateConstitutiveSlipDerivative(std::vector<Real> & dslip_dtau)
{
  Real theta = _temperature + 273.15;
  for (const auto i : make_range(_number_slip_systems))
  {
    Real driving_force = std::abs(_tau[_qp][i] - _backstress(i)) - _slip_resistance[_qp][i];
    Real u = 0.0, uprime = 0.0, vprime = 0.0;
    if (driving_force < _zero_tol)
      dslip_dtau[i] = 0.0;
    else
    {
      u = driving_force / _tau_0;
      uprime = std::pow(u, _p - 1.0) * std::copysign(1.0, (_tau[_qp][i] - _backstress(i)));
      vprime = std::pow((1.0 - std::pow(u, _p)), _q - 1.0);
      dslip_dtau[i] = _gdot0 * _p * _q * _f0 / _boltzmann / theta *
                      std::exp(-_f0 / _boltzmann / theta * std::pow((1.0 - std::pow(u, _p)), _q)) *
                      uprime * vprime * _substep_dt;
    }
    // mooseWarning("tau=",_tau[_qp][i]);
  }
}

bool
CrystalPlasticityBussoUpdateSingleSlipSys::areConstitutiveStateVariablesConverged()
{
  return true;
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::updateSubstepConstitutiveVariableValues()
{
  // No need for this subroutine
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::cacheStateVariablesBeforeUpdate()
{
  // No need for this subroutine
}

void
CrystalPlasticityBussoUpdateSingleSlipSys::calculateStateVariableEvolutionRateComponent()
{
  // No need for this subroutine
}

bool
CrystalPlasticityBussoUpdateSingleSlipSys::updateStateVariables()
{
  return true;
}
