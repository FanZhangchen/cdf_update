//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MatCoupledDisloDensityALL.h"

#include "MooseVariable.h"

registerMooseObject("cdf_updateApp", MatCoupledDisloDensityALL);

InputParameters
MatCoupledDisloDensityALL::validParams()
{
  InputParameters params = Kernel::validParams();

  params.addClassDescription(
      "Implements a forcing term RHS of the form PDE = RHS, where RHS = Sum_j c_j * m_j * v_j. "
      "c_j, m_j, and v_j are provided as real coefficients, material properties, and coupled "
      "variables, respectively.");
  params.addRequiredCoupledVar(
      "v", "The coupled variables which provide the dislocation density as source and sink terms");
  params.addParam<std::vector<Real>>(
      "coef", "Coefficents ($\\sigma$) multiplier for the coupled force term.");
  params.addParam<std::vector<MaterialPropertyName>>("material_properties",
                                                     "The coupled material properties.");
  params.addRequiredParam<int>("slip_sys_index",
                               "Slip system index to determine slip direction "
                               "for instance from 0 to 11 for FCC.");
  MooseEnum dislo_character("edge screw", "edge");
  params.addRequiredParam<MooseEnum>(
      "dislo_character", dislo_character, "Character of dislocations: edge or screw.");
  /// The parameters that need to be used in the calculations of source and sink terms
  params.addParam<Real>("burgers", 2.57e-7, "magnitude of burgers vector");
  params.addParam<Real>("ke_b", 55000.0, "edge hardening constants");
  params.addParam<Real>("ks_b", 110000.0, "screw hardening constants");
  params.addParam<Real>("de", 1.0e-6, "critical edge annihilation distance");
  params.addParam<Real>("ds", 5.0e-6, "critical screw annihilation distance");
  params.addParam<Real>("Ce", 0.5, "edge proportional constant");
  params.addParam<Real>("Cs", 0.5, "screw proportional constant");

  params.addParam<bool>("check_rho_positive", false, "Check positive dislocation density.");
  return params;
}

MatCoupledDisloDensityALL::MatCoupledDisloDensityALL(const InputParameters & parameters)
  : Kernel(parameters),
    _n_coupled(coupledComponents("v")),
    _coupled_props(isParamValid("material_properties")),
    _v_var(coupledIndices("v")),
    _v(coupledValues("v")),
    _coef(isParamValid("coef") ? getParam<std::vector<Real>>("coef")
                               : std::vector<Real>(_n_coupled, 1)),

    // Get the parameters of the initial condition of SSD density from the input file
    _burgers(getParam<Real>("burgers")),
    _ke_b(getParam<Real>("ke_b")),
    _ks_b(getParam<Real>("ks_b")),
    _de(getParam<Real>("de")),
    _ds(getParam<Real>("ds")),
    _Ce(getParam<Real>("Ce")),
    _Cs(getParam<Real>("Cs")),

    _slip_sys_index(getParam<int>("slip_sys_index")),
    _dislo_character(getParam<MooseEnum>("dislo_character").getEnum<DisloCharacter>()),
    _check_rho_positive(getParam<bool>("check_rho_positive")),
    _slip_increment(getMaterialProperty<std::vector<Real>>("slip_increment"))
{
  for (MooseIndex(_n_coupled) j = 0; j < _n_coupled; ++j)
  {
    _v_var_to_index[_v_var[j]] = j;

    if (_var.number() == _v_var[j])
      paramError(
          "v",
          "Coupled variable 'v' needs to be different from 'variable' with MatCoupledDisloDensityALL, "
          "consider using Reaction or somethig similar");
  }

  if (isParamValid("coef") && _coef.size() != _n_coupled)
    paramError("coef", "Size of coef must be equal to size of v");

  if (_coupled_props)
  {
    _mat_props.resize(_n_coupled);
    std::vector<MaterialPropertyName> names =
        getParam<std::vector<MaterialPropertyName>>("material_properties");
    if (names.size() != _n_coupled)
      paramError("material_properties", "Size must be equal to number of coupled variables");
    for (unsigned int j = 0; j < _n_coupled; ++j)
      _mat_props[j] = &getMaterialPropertyByName<Real>(names[j]);
  }
}

Real
MatCoupledDisloDensityALL::computeQpResidual()
{
  Real r = 0;
  Real total_dislocation_density = 0;
  Real temp_type_dislocation_density = 0;
  Real source = 0;
  Real sink = 0;

  const Real abs_slip_increment = std::abs(_slip_increment[_qp][_slip_sys_index]);

  // Calculate the total dislocation density for all slip systems
  for (unsigned int j = 0; j < _n_coupled; ++j)
  {
    total_dislocation_density += (*_v[j])[_qp];
  }

  for (unsigned int k = 0; k < 3; ++k)
  {
    temp_type_dislocation_density += (*_v[k])[_qp];
  }

  switch (_dislo_character)
  {
    case DisloCharacter::edge:
      source = _Ce * _ke_b * std::sqrt(total_dislocation_density + _u[_qp]) * abs_slip_increment;
      sink = -_Ce / _burgers * 2.0 * _de * (temp_type_dislocation_density + _u[_qp]) *
             abs_slip_increment;
      break;
    case DisloCharacter::screw:
      source = _Cs * _ks_b * std::sqrt(total_dislocation_density + _u[_qp]) * abs_slip_increment;
      sink = -_Cs / _burgers *
             (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                  std::sqrt(total_dislocation_density + _u[_qp]) +
              2.0 * _ds) *
             (temp_type_dislocation_density + _u[_qp]) * abs_slip_increment;
      break;
  }

  r = source + sink;

  // if (_coupled_props)
  //   for (unsigned int j = 0; j < _n_coupled; ++j)
  //     r += -_coef[j] * (*_mat_props[j])[_qp] * (*_v[j])[_qp];
  // else
  //   for (unsigned int j = 0; j < _n_coupled; ++j)
  //     r += -_coef[j] * (*_v[j])[_qp];

  if (_check_rho_positive && _u[_qp] <= 0.0)
  {
    return 0.0;
  }
  else
  {
    return -r * _test[_i][_qp];
  }
}

Real
MatCoupledDisloDensityALL::computeQpJacobian()
{
  Real r = 0;
  Real total_dislocation_density = 0;
  Real source = 0;
  Real sink = 0;

  const Real abs_slip_increment = std::abs(_slip_increment[_qp][_slip_sys_index]);

  // Calculate the total dislocation density for all slip systems
  for (unsigned int j = 0; j < _n_coupled; ++j)
  {
    total_dislocation_density += (*_v[j])[_qp];
  }

  switch (_dislo_character)
  {
    case DisloCharacter::edge:
      source =
          _Ce * _ke_b * 0.5 / std::sqrt(total_dislocation_density + _u[_qp]) * abs_slip_increment;
      sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
      break;
    case DisloCharacter::screw:
      source =
          _Cs * _ks_b * 0.5 / std::sqrt(total_dislocation_density + _u[_qp]) * abs_slip_increment;
      sink = -_Cs / _burgers *
             (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers * 0.5 /
                  std::sqrt(total_dislocation_density + _u[_qp]) +
              2.0 * _ds) *
             abs_slip_increment;
      break;
  }

  r = source + sink;

  if (_check_rho_positive && _u[_qp] <= 0.0)
  {
    return 0.0;
  }
  else
  {
    return -r * _test[_i][_qp] * _phi[_j][_qp];
  }
}

Real
MatCoupledDisloDensityALL::computeQpOffDiagJacobian(unsigned int jvar)
{
  auto p = _v_var_to_index.find(jvar);
  if (p == _v_var_to_index.end())
    return 0;

  Real r = 0;
  Real total_dislocation_density = 0;
  Real source = 0;
  Real sink = 0;

  switch (_dislo_character)
  {
    case DisloCharacter::edge:
      if (jvar == 0 || jvar == 1 || jvar == 2)
      {
        return 0;
      } 
      else 
      {
        // Handle unexpected jvar values (optional)
        return 0; // Or throw an error
      }
      break;
    case DisloCharacter::screw:
      if (jvar == 0 || jvar == 1 || jvar == 2)
      {
        return 0;
      } 
      else 
      {
        // Handle unexpected jvar values (optional)
        return 0; // Or throw an error
      }
      break;
  }

  if (_coupled_props)
    return -_phi[_j][_qp] * _test[_i][_qp];
  return -_phi[_j][_qp] * _test[_i][_qp];
}
