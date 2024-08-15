// Zhangchen Fan
// HITSZ - CMMC
// May 2023

#include "DisloVelocity_DbSlip.h"
#include "petscblaslapack.h"
#include "libmesh/utility.h"

#include <fstream>
#include <cmath>

registerMooseObject("cdf_updateApp", DisloVelocity_DbSlip);

InputParameters
DisloVelocity_DbSlip::validParams()
{
  InputParameters params = Material::validParams();

  params.addClassDescription("Dislocation velocity for Dislocation transport equation. ");

  params.addRequiredParam<int>("nss", "Number of slip systems");

  params.addParam<Real>("boltzmann", 1.38065e-23, "The Boltzmann Constant");

  params.addParam<Real>("abstemp", 298, "The absolute temperature");

  params.addParam<Real>("p", 0.2, "The flow rule parameter p");

  params.addParam<Real>("q", 1.2, "The flow rule parameter q");

  params.addParam<Real>(
      "tau0hat", 20, "Obtained by extrapolating the lattice friction stress at 0K");

  params.addParam<Real>("gdot0", 1.e6, "The flow rule parameter gamma0");

  params.addParam<Real>("f0", 2.77e-19, "Helmholtz free energy of activation");

  params.addParam<Real>("dlamb",
                        0.3,
                        "A statistical coefficient which accounts for the deviation from regular "
                        "spatial arrangements of the dislocation");

  params.addParam<Real>("shear_modulus", 45.e3, "Shear moduli");

  params.addParam<Real>("burgers", 0.257e-6, "The Burgers Vector");

  params.addParam<Real>("taualpha", 2.63, "The resolved shear stress");

  params.addParam<Real>("w1", 0.0, "cross-hardening constants");

  params.addParam<Real>("w2", 0.0, "cross-hardening constants");

  params.addRequiredCoupledVar("edge_dislo_den_pos_1",
                               "positive edge dislocation density on Slip 1");

  params.addRequiredCoupledVar("edge_dislo_den_neg_1",
                               "negative edge dislocation density on Slip 1");

  params.addRequiredCoupledVar("edge_dislo_den_pos_2",
                               "positive screw dislocation density on Slip 2");

  params.addRequiredCoupledVar("edge_dislo_den_neg_2",
                               "negative screw dislocation density on Slip 2");

  params.addParam<Real>("scale", 0.5, "The scaling value of screw dislocation velocity");

  // params.addParam<std::vector<Real>>("rho_edge", 16000, "The total edge dislocation density");

  return params;
}

DisloVelocity_DbSlip::DisloVelocity_DbSlip(const InputParameters & parameters)
  : DerivativeMaterialInterface<Material>(parameters),

    _nss(getParam<int>("nss")),

    _gssT(_nss),

    // _slip_rate(getParam<Real>("slip_rate")),

    _abstemp(getParam<Real>("abstemp")),

    _boltzmann(getParam<Real>("boltzmann")),

    _burgers(getParam<Real>("burgers")),

    _f0(getParam<Real>("f0")),

    _gdot0(getParam<Real>("gdot0")),

    _dlamb(getParam<Real>("dlamb")),

    _shear_modulus(getParam<Real>("shear_modulus")),

    _p(getParam<Real>("p")),

    _q(getParam<Real>("q")),

    _tau0hat(getParam<Real>("tau0hat")),

    _taualpha(getParam<Real>("taualpha")),

    _w1(getParam<Real>("w1")),
    _w2(getParam<Real>("w2")),

    _dislo_velocity(declareProperty<std::vector<Real>>(
        "dislo_velocity")), // Dislocation velocity at current time step t

    _velocity_old(
        getMaterialPropertyOld<std::vector<Real>>("dislo_velocity")), // Dislocation velocity at t-1

    _slip_increment(
        declareProperty<std::vector<Real>>("slip_increment")), // Dislocation slip increment/rate

    _edge_dislo_den_pos_1(coupledValue("edge_dislo_den_pos_1")), // Coupled rhoep

    _grad_edge_dislo_den_pos_1(coupledGradient("edge_dislo_den_pos_1")), // Coupled rhoep gradient

    _edge_dislo_den_neg_1(coupledValue("edge_dislo_den_neg_1")), // Coupled rhoen

    _grad_edge_dislo_den_neg_1(coupledGradient("edge_dislo_den_neg_1")), // Coupled rhoen gradient

    _edge_dislo_den_pos_2(coupledValue("edge_dislo_den_pos_2")), // Coupled rhosp

    _grad_edge_dislo_den_pos_2(coupledGradient("edge_dislo_den_pos_2")), // Coupled rhosp gradient

    _edge_dislo_den_neg_2(coupledValue("edge_dislo_den_neg_2")), // Coupled rhosn

    _grad_edge_dislo_den_neg_2(coupledGradient("edge_dislo_den_neg_2")), // Coupled rhosn gradient

    _slip_rate(declareProperty<Real>("slip_rate")),

    _slip_resistance(declareProperty<std::vector<Real>>("slip_resistance")),

    _total_dislo_den(declareProperty<std::vector<Real>>("total_dislo_den")),

    _rhot(declareProperty<Real>("rhot")),

    _tau_backstress(declareProperty<Real>("tau_backstress")),

    _tau_diffusion(declareProperty<Real>("tau_diffusion")),

    _scale(getParam<Real>("scale"))

{
}

void
DisloVelocity_DbSlip::computeQpProperties()
{

  std::vector<Real> rho_edge_pos(_nss);
  std::vector<Real> rho_edge_neg(_nss);

  _slip_resistance[_qp].resize(_nss);

  rho_edge_pos[0] = _edge_dislo_den_pos_1[_qp];
  rho_edge_pos[1] = _edge_dislo_den_pos_2[_qp];
  // rho_edge_pos[2] = _edge_dislo_den_pos_3[_qp];
  // rho_edge_pos[3] = _edge_dislo_den_pos_4[_qp];
  // rho_edge_pos[4] = _edge_dislo_den_pos_5[_qp];
  // rho_edge_pos[5] = _edge_dislo_den_pos_6[_qp];
  // rho_edge_pos[6] = _edge_dislo_den_pos_7[_qp];
  // rho_edge_pos[7] = _edge_dislo_den_pos_8[_qp];
  // rho_edge_pos[8] = _edge_dislo_den_pos_9[_qp];
  // rho_edge_pos[9] = _edge_dislo_den_pos_10[_qp];
  // rho_edge_pos[10] = _edge_dislo_den_pos_11[_qp];
  // rho_edge_pos[11] = _edge_dislo_den_pos_12[_qp];

  rho_edge_neg[0] = _edge_dislo_den_neg_1[_qp];
  rho_edge_neg[1] = _edge_dislo_den_neg_2[_qp];
  // rho_edge_neg[2] = _edge_dislo_den_neg_3[_qp];
  // rho_edge_neg[3] = _edge_dislo_den_neg_4[_qp];
  // rho_edge_neg[4] = _edge_dislo_den_neg_5[_qp];
  // rho_edge_neg[5] = _edge_dislo_den_neg_6[_qp];
  // rho_edge_neg[6] = _edge_dislo_den_neg_7[_qp];
  // rho_edge_neg[7] = _edge_dislo_den_neg_8[_qp];
  // rho_edge_neg[8] = _edge_dislo_den_neg_9[_qp];
  // rho_edge_neg[9] = _edge_dislo_den_neg_10[_qp];
  // rho_edge_neg[10] = _edge_dislo_den_neg_11[_qp];
  // rho_edge_neg[11] = _edge_dislo_den_neg_12[_qp];

  // Set initial slip resistance
  for (const auto i : make_range(_nss))
  {
    Real initial_hardening_total_dislocation_density = 0.0;
    for (const auto j : make_range(_nss))
    {
      // mooseWarning("x0-",i,j);
      if (i == j) // self vs. latent hardening
        initial_hardening_total_dislocation_density +=
            (_w1 + 1.0 - _w2) * (rho_edge_pos[j] + rho_edge_neg[j]);
      // (_w1 + 1.0 - _w2) * (_edge_dislocation_density[_qp][j] +
      // _screw_dislocation_density[_qp][j]); // self hardening
      else
        initial_hardening_total_dislocation_density += _w1 * (rho_edge_pos[j] + rho_edge_neg[j]);
      // _w1   * (_edge_dislocation_density[_qp][j] + _screw_dislocation_density[_qp][j]); // latent
      // hardening
    }
    _slip_resistance[_qp][i] =
        _dlamb * _shear_modulus * _burgers * std::sqrt(initial_hardening_total_dislocation_density);
  }

  _dislo_velocity[_qp].resize(_nss);
  _slip_increment[_qp].resize(_nss);

  for (const auto i : make_range(_nss))
  {
    _dislo_velocity[_qp][i] = 0.0;
  }

  // initialize the edge dislocation density

  _total_dislo_den[_qp].resize(_nss);

  for (const auto i : make_range(_nss))
  {
    _total_dislo_den[_qp][i] = rho_edge_pos[i] + rho_edge_neg[i];

    if (i == 0)
    {
      _tau_backstress[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_1[_qp](0) - _grad_edge_dislo_den_neg_1[_qp](0)) /
          _total_dislo_den[_qp][i];
      _tau_diffusion[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_1[_qp](0) + _grad_edge_dislo_den_neg_1[_qp](0)) /
          _total_dislo_den[_qp][i];
    }
    else if (i == 1)
    {
      _tau_backstress[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_2[_qp](1) - _grad_edge_dislo_den_neg_2[_qp](1)) /
          _total_dislo_den[_qp][i];
      _tau_diffusion[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_2[_qp](1) + _grad_edge_dislo_den_neg_2[_qp](1)) /
          _total_dislo_den[_qp][i];
    }

    Real driving_force = std::abs(_taualpha - _tau_backstress[_qp]) - _slip_resistance[_qp][i];

    if (driving_force < 0.0)
    {
      _slip_rate[_qp] = 0.0;
    }
    else
    {
      _slip_rate[_qp] =
        _gdot0 *
        std::exp(-_f0 / _boltzmann / _abstemp *
                 std::pow((1 - std::pow(((std::abs(_taualpha - _tau_backstress[_qp]) -
                                          _tau_diffusion[_qp] - _slip_resistance[_qp][i]) /
                                         _tau0hat),
                                        _p)),
                          _q)) *
        std::copysign(1.0, (_taualpha - _tau_backstress[_qp]));
    }

    _slip_increment[_qp][i] = _slip_rate[_qp];
  }

  for (const auto i : make_range(_nss))
  {
    _dislo_velocity[_qp][i] = _slip_increment[_qp][i] / _burgers / _total_dislo_den[_qp][i];
  }
}

void
DisloVelocity_DbSlip::initQpStatefulProperties()
{

  std::vector<Real> rho_edge_pos(_nss);
  std::vector<Real> rho_edge_neg(_nss);

  _slip_resistance[_qp].resize(_nss);

  rho_edge_pos[0] = _edge_dislo_den_pos_1[_qp];
  rho_edge_pos[1] = _edge_dislo_den_pos_2[_qp];
  // rho_edge_pos[2] = _edge_dislo_den_pos_3[_qp];
  // rho_edge_pos[3] = _edge_dislo_den_pos_4[_qp];
  // rho_edge_pos[4] = _edge_dislo_den_pos_5[_qp];
  // rho_edge_pos[5] = _edge_dislo_den_pos_6[_qp];
  // rho_edge_pos[6] = _edge_dislo_den_pos_7[_qp];
  // rho_edge_pos[7] = _edge_dislo_den_pos_8[_qp];
  // rho_edge_pos[8] = _edge_dislo_den_pos_9[_qp];
  // rho_edge_pos[9] = _edge_dislo_den_pos_10[_qp];
  // rho_edge_pos[10] = _edge_dislo_den_pos_11[_qp];
  // rho_edge_pos[11] = _edge_dislo_den_pos_12[_qp];

  rho_edge_neg[0] = _edge_dislo_den_neg_1[_qp];
  rho_edge_neg[1] = _edge_dislo_den_neg_2[_qp];
  // rho_edge_neg[2] = _edge_dislo_den_neg_3[_qp];
  // rho_edge_neg[3] = _edge_dislo_den_neg_4[_qp];
  // rho_edge_neg[4] = _edge_dislo_den_neg_5[_qp];
  // rho_edge_neg[5] = _edge_dislo_den_neg_6[_qp];
  // rho_edge_neg[6] = _edge_dislo_den_neg_7[_qp];
  // rho_edge_neg[7] = _edge_dislo_den_neg_8[_qp];
  // rho_edge_neg[8] = _edge_dislo_den_neg_9[_qp];
  // rho_edge_neg[9] = _edge_dislo_den_neg_10[_qp];
  // rho_edge_neg[10] = _edge_dislo_den_neg_11[_qp];
  // rho_edge_neg[11] = _edge_dislo_den_neg_12[_qp];

  // Set initial slip resistance
  for (const auto i : make_range(_nss))
  {
    Real initial_hardening_total_dislocation_density = 0.0;
    for (const auto j : make_range(_nss))
    {
      // mooseWarning("x0-",i,j);
      if (i == j) // self vs. latent hardening
        initial_hardening_total_dislocation_density +=
            (_w1 + 1.0 - _w2) * (rho_edge_pos[j] + rho_edge_neg[j]);
      // (_w1 + 1.0 - _w2) * (_edge_dislocation_density[_qp][j] +
      // _screw_dislocation_density[_qp][j]); // self hardening
      else
        initial_hardening_total_dislocation_density += _w1 * (rho_edge_pos[j] + rho_edge_neg[j]);
      // _w1   * (_edge_dislocation_density[_qp][j] + _screw_dislocation_density[_qp][j]); // latent
      // hardening
    }
    _slip_resistance[_qp][i] =
        _dlamb * _shear_modulus * _burgers * std::sqrt(initial_hardening_total_dislocation_density);
  }

  _dislo_velocity[_qp].resize(_nss);
  _slip_increment[_qp].resize(_nss);

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] = 0.0;
  }

  // initialize the edge dislocation density

  _total_dislo_den[_qp].resize(_nss);

  for (const auto i : make_range(_nss))
  {
    _total_dislo_den[_qp][i] = rho_edge_pos[i] + rho_edge_neg[i];

    if (i == 0)
    {
      _tau_backstress[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_1[_qp](0) - _grad_edge_dislo_den_neg_1[_qp](0)) /
          _total_dislo_den[_qp][i];
      _tau_diffusion[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_1[_qp](0) + _grad_edge_dislo_den_neg_1[_qp](0)) /
          _total_dislo_den[_qp][i];
    }
    else if (i == 1)
    {
      _tau_backstress[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_2[_qp](1) - _grad_edge_dislo_den_neg_2[_qp](1)) /
          _total_dislo_den[_qp][i];
      _tau_diffusion[_qp] =
          _shear_modulus * _burgers *
          (_grad_edge_dislo_den_pos_2[_qp](1) + _grad_edge_dislo_den_neg_2[_qp](1)) /
          _total_dislo_den[_qp][i];
    }

    Real driving_force = std::abs(_taualpha - _tau_backstress[_qp]) - _slip_resistance[_qp][i];

    if (driving_force < 0.0)
    {
      _slip_rate[_qp] = 0.0;
    }
    else
    {
      _slip_rate[_qp] =
        _gdot0 *
        std::exp(-_f0 / _boltzmann / _abstemp *
                 std::pow((1 - std::pow(((std::abs(_taualpha - _tau_backstress[_qp]) -
                                          _tau_diffusion[_qp] - _slip_resistance[_qp][i]) /
                                         _tau0hat),
                                        _p)),
                          _q)) *
        std::copysign(1.0, (_taualpha - _tau_backstress[_qp]));
    }

    _slip_increment[_qp][i] = _slip_rate[_qp];
  }

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] = _slip_increment[_qp][i] / _burgers / _total_dislo_den[_qp][i];
  }
}