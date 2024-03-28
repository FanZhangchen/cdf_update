// Zhangchen Fan
// HITSZ - CMMC
// May 2023

#include "DisloVelocity_2D8.h"
#include "petscblaslapack.h"
#include "libmesh/utility.h"

#include <fstream>
#include <cmath>

registerMooseObject("cdf_updateApp", DisloVelocity_2D8);

InputParameters
DisloVelocity_2D8::validParams()
{
  InputParameters params = Material::validParams();

  params.addClassDescription("Dislocation velocity for Dislocation transport equation. ");

  params.addRequiredParam<int>("nss", "Number of slip systems");

  params.addRequiredCoupledVar("edge_dislo_den_1", "edge dislocation density in Q1");

  params.addRequiredCoupledVar("edge_dislo_den_2", "edge dislocation density in Q2");

  params.addRequiredCoupledVar("edge_dislo_den_3", "edge dislocation density in Q3");

  params.addRequiredCoupledVar("edge_dislo_den_4", "edge dislocation density in Q4");

  params.addRequiredCoupledVar("screw_dislo_den_1", "positive screw dislocation density");

  params.addRequiredCoupledVar("screw_dislo_den_2", "negative screw dislocation density");

  params.addRequiredCoupledVar("screw_dislo_den_3", "positive screw dislocation density");

  params.addRequiredCoupledVar("screw_dislo_den_4", "negative screw dislocation density");

  params.addParam<Real>("boltzmann", 1.38065e-23, "The Boltzmann Constant");

  params.addParam<Real>("abstemp", 298, "The absolute temperature");

  params.addParam<Real>("p", 0.2, "The flow rule parameter p");

  params.addParam<Real>("q", 1.2, "The flow rule parameter q");

  params.addParam<Real>(
      "tau0hat", 20, "Obtained by extrapolating the lattice friction stress at 0K");

  params.addParam<Real>("gamma0dot", 1.e6, "The flow rule parameter gamma0");

  params.addParam<Real>("F0", 2.77e-19, "Helmholtz free energy of activation");

  params.addParam<Real>("lambda",
                        0.3,
                        "A statistical coefficient which accounts for the deviation from regular "
                        "spatial arrangements of the dislocation");

  params.addParam<Real>("mu", 45.e3, "Shear moduli");

  params.addParam<Real>("burgersvector", 0.257e-6, "The Burgers Vector");

  params.addParam<Real>("taualpha", 2.36, "The resolved shear stress");

  params.addParam<Real>("scale", 0.5, "The ratio of screw velocity");

  // params.addParam<std::vector<Real>>("rho_edge", 16000, "The total edge dislocation density");

  return params;
}

DisloVelocity_2D8::DisloVelocity_2D8(const InputParameters & parameters)
  : DerivativeMaterialInterface<Material>(parameters),

    _nss(getParam<int>("nss")),

    _gssT(_nss),

    _boltzmann(getParam<Real>("boltzmann")),

    _abstemp(getParam<Real>("abstemp")),

    _p(getParam<Real>("p")),

    _q(getParam<Real>("q")),

    _tau0hat(getParam<Real>("tau0hat")),

    _gamma0dot(getParam<Real>("gamma0dot")),

    _F0(getParam<Real>("F0")),

    _lambda(getParam<Real>("lambda")),

    _mu(getParam<Real>("mu")),

    _taualpha(getParam<Real>("taualpha")),

    _burgersvector(getParam<Real>("burgersvector")),

    _scale(getParam<Real>("scale")),

    _dislo_velocity(declareProperty<std::vector<Real>>(
        "dislo_velocity")), // Dislocation velocity at current time step t

    _velocity_old(
        getMaterialPropertyOld<std::vector<Real>>("dislo_velocity")), // Dislocation velocity at t-1

    _edge_dislo_den_1(coupledValue("edge_dislo_den_1")), // Coupled edge_dislo_den_p

    _grad_edge_dislo_den_1(
        coupledGradient("edge_dislo_den_1")), // Coupled edge_dislo_den_p gradient

    _edge_dislo_den_2(coupledValue("edge_dislo_den_2")), // Coupled edge_dislo_den_n

    _grad_edge_dislo_den_2(
        coupledGradient("edge_dislo_den_2")), // Coupled edge_dislo_den_n gradient

    _edge_dislo_den_3(coupledValue("edge_dislo_den_3")), // Coupled edge_dislo_den_p

    _grad_edge_dislo_den_3(
        coupledGradient("edge_dislo_den_3")), // Coupled edge_dislo_den_p gradient

    _edge_dislo_den_4(coupledValue("edge_dislo_den_4")), // Coupled edge_dislo_den_n

    _grad_edge_dislo_den_4(
        coupledGradient("edge_dislo_den_4")), // Coupled edge_dislo_den_n gradient

    _screw_dislo_den_1(coupledValue("screw_dislo_den_1")), // Coupled screw_dislo_den_p

    _grad_screw_dislo_den_1(
        coupledGradient("screw_dislo_den_1")), // Coupled screw_dislo_den_p gradient

    _screw_dislo_den_2(coupledValue("screw_dislo_den_2")), // Coupled screw_dislo_den_n

    _grad_screw_dislo_den_2(
        coupledGradient("screw_dislo_den_2")), // Coupled screw_dislo_den_n gradient

    _screw_dislo_den_3(coupledValue("screw_dislo_den_3")), // Coupled screw_dislo_den_p

    _grad_screw_dislo_den_3(
        coupledGradient("screw_dislo_den_3")), // Coupled screw_dislo_den_p gradient

    _screw_dislo_den_4(coupledValue("screw_dislo_den_4")), // Coupled screw_dislo_den_n

    _grad_screw_dislo_den_4(
        coupledGradient("screw_dislo_den_4")), // Coupled screw_dislo_den_n gradient

    _slip_rate(declareProperty<Real>("slip_rate")),

    _rho_edge(declareProperty<Real>("rho_edge")),

    _rho_screw(declareProperty<Real>("rho_screw")),

    _rhot(declareProperty<Real>("rhot")),

    _tau_backstress(declareProperty<Real>("tau_backstress"))
{
}

void
DisloVelocity_2D8::computeQpProperties()
{

  _dislo_velocity[_qp].resize(_nss);

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] = 0.0;
  }

  // initialize the edge dislocation density

  _rho_edge[_qp] = _edge_dislo_den_1[_qp] + _edge_dislo_den_2[_qp] + _edge_dislo_den_3[_qp] +
                   _edge_dislo_den_4[_qp];

  _rho_screw[_qp] = _screw_dislo_den_1[_qp] + _screw_dislo_den_2[_qp] + _screw_dislo_den_3[_qp] +
                    _screw_dislo_den_4[_qp];

  _rhot[_qp] = _rho_edge[_qp] + _rho_screw[_qp];

  _tau_backstress[_qp] = _burgersvector * _mu *
                         (_grad_edge_dislo_den_1[_qp](0) + _grad_edge_dislo_den_2[_qp](0) -
                          _grad_edge_dislo_den_3[_qp](0) - _grad_edge_dislo_den_4[_qp](0) -
                          _grad_screw_dislo_den_1[_qp](1) + _grad_screw_dislo_den_2[_qp](1) +
                          _grad_screw_dislo_den_3[_qp](1) - _grad_screw_dislo_den_4[_qp](1)) /
                         _rhot[_qp];

  _slip_rate[_qp] =
      _gamma0dot *
      std::exp(-_F0 / _boltzmann / _abstemp *
               std::pow((1 - std::pow(((std::abs(_taualpha - _tau_backstress[_qp]) -
                                        _lambda * _mu * _burgersvector * std::sqrt(_rhot[_qp])) /
                                       _tau0hat),
                                      _p)),
                        _q)) *
      std::copysign(1.0, (_taualpha - _tau_backstress[_qp]));

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] =
        _slip_rate[_qp] / _burgersvector / (_rho_edge[_qp] + _scale * _rho_screw[_qp]);
  }
}

void
DisloVelocity_2D8::initQpStatefulProperties()
{

  _dislo_velocity[_qp].resize(_nss);

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] = 0.0;
  }

  // initialize the edge dislocation density

  _rho_edge[_qp] = _edge_dislo_den_1[_qp] + _edge_dislo_den_2[_qp] + _edge_dislo_den_3[_qp] +
                   _edge_dislo_den_4[_qp];

  _rho_screw[_qp] = _screw_dislo_den_1[_qp] + _screw_dislo_den_2[_qp] + _screw_dislo_den_3[_qp] +
                    _screw_dislo_den_4[_qp];

  _rhot[_qp] = _rho_edge[_qp] + _rho_screw[_qp];

  _tau_backstress[_qp] = _burgersvector * _mu *
                         (_grad_edge_dislo_den_1[_qp](0) + _grad_edge_dislo_den_2[_qp](0) -
                          _grad_edge_dislo_den_3[_qp](0) - _grad_edge_dislo_den_4[_qp](0) +
                          _grad_screw_dislo_den_1[_qp](1) - _grad_screw_dislo_den_2[_qp](1) -
                          _grad_screw_dislo_den_3[_qp](1) + _grad_screw_dislo_den_4[_qp](1)) /
                         _rhot[_qp];

  _slip_rate[_qp] =
      _gamma0dot *
      std::exp(-_F0 / _boltzmann / _abstemp *
               std::pow((1 - std::pow(((std::abs(_taualpha - _tau_backstress[_qp]) -
                                        _lambda * _mu * _burgersvector * std::sqrt(_rhot[_qp])) /
                                       _tau0hat),
                                      _p)),
                        _q)) *
      std::copysign(1.0, (_taualpha - _tau_backstress[_qp]));

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] =
        _slip_rate[_qp] / _burgersvector / (_rho_edge[_qp] + _scale * _rho_screw[_qp]);
  }
}