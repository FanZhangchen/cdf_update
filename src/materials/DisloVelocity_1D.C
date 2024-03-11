// Zhangchen Fan
// HITSZ - CMMC
// May 2023

#include "DisloVelocity_1D.h"
#include "petscblaslapack.h"
#include "libmesh/utility.h"

#include <fstream>
#include <cmath>

registerMooseObject("cdf_updateApp", DisloVelocity_1D);

InputParameters
DisloVelocity_1D::validParams()
{
  InputParameters params = Material::validParams();

  params.addClassDescription("Dislocation velocity for Dislocation transport equation. ");

  params.addRequiredParam<int>("nss", "Number of slip systems");

  params.addRequiredCoupledVar("rhoep", "positive edge dislocation density");

  params.addRequiredCoupledVar("rhoen", "negative edge dislocation density");

  // params.addRequiredCoupledVar("rhosp", "positive screw dislocation density");

  // params.addRequiredCoupledVar("rhosn", "negative screw dislocation density");

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

  params.addParam<Real>("taualpha", 2.63, "The resolved shear stress");

  // params.addParam<std::vector<Real>>("rho_edge", 16000, "The total edge dislocation density");

  return params;
}

DisloVelocity_1D::DisloVelocity_1D(const InputParameters & parameters)
  : DerivativeMaterialInterface<Material>(parameters),

    _nss(getParam<int>("nss")),

    _gssT(_nss),

    // _slip_rate(getParam<Real>("slip_rate")),

    _dislo_velocity(declareProperty<std::vector<Real>>(
        "dislo_velocity")), // Dislocation velocity at current time step t

    _velocity_old(
        getMaterialPropertyOld<std::vector<Real>>("dislo_velocity")), // Dislocation velocity at t-1

    _rhoep(coupledValue("rhoep")), // Coupled rhoep

    _grad_rhoep(coupledGradient("rhoep")), // Coupled rhoep gradient

    _rhoen(coupledValue("rhoen")), // Coupled rhoen

    _grad_rhoen(coupledGradient("rhoen")), // Coupled rhoen gradient

    // _rhosp(coupledValue("rhosp")), // Coupled rhosp

    // _grad_rhosp(coupledGradient("rhosp")), // Coupled rhosp gradient

    // _rhosn(coupledValue("rhosn")), // Coupled rhosn

    // _grad_rhosn(coupledGradient("rhosn")), // Coupled rhosn gradient

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

    _rho_edge(declareProperty<Real>("rho_edge")),

    _rho_screw(declareProperty<Real>("rho_screw")),

    _rhot(declareProperty<Real>("rhot")),

    _tau_backstress(declareProperty<Real>("tau_backstress")),

    _slip_rate(declareProperty<Real>("slip_rate"))

{
}

void
DisloVelocity_1D::computeQpProperties()
{

  _dislo_velocity[_qp].resize(_nss);

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] = 0.0;
  }

  // initialize the edge dislocation density

  _rho_edge[_qp] = _rhoep[_qp] + _rhoen[_qp];

  // _rho_screw[_qp] = _rhosp[_qp] + _rhosn[_qp];

  _rhot[_qp] = _rho_edge[_qp];

  _tau_backstress[_qp] =
      _burgersvector * _mu * (_grad_rhoep[_qp](0) - _grad_rhoen[_qp](0)) / _rhot[_qp];

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
    _dislo_velocity[_qp][i] = _slip_rate[_qp] / _burgersvector / (_rho_edge[_qp]);
  }
}

void
DisloVelocity_1D::initQpStatefulProperties()
{

  _dislo_velocity[_qp].resize(_nss);

  for (unsigned int i = 0; i < _nss; ++i)
  {
    _dislo_velocity[_qp][i] = 0.0;
  }

  // initialize the edge dislocation density

  _rho_edge[_qp] = _rhoep[_qp] + _rhoen[_qp];

  // _rho_screw[_qp] = _rhosp[_qp] + _rhosn[_qp];

  _rhot[_qp] = _rho_edge[_qp];

  _tau_backstress[_qp] =
      _burgersvector * _mu * (_grad_rhoep[_qp](0) - _grad_rhoen[_qp](0)) / _rhot[_qp];

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
    _dislo_velocity[_qp][i] = _slip_rate[_qp] / _burgersvector / (_rho_edge[_qp]);
  }
}