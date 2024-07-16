#include "ConservativeSourceSink.h"
#include "SystemBase.h"
#include "libmesh/utility.h"

registerMooseObject("cdf_updateApp", ConservativeSourceSink);

InputParameters
ConservativeSourceSink::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("This is a kernel for the source and sink term see Cheong 2004.");
  MooseEnum upwinding_type("none full", "none");
  params.addParam<MooseEnum>("upwinding_type",
                             upwinding_type,
                             "Type of upwinding used.  None: Typically results in overshoots and "
                             "undershoots, but numerical diffusion is minimized.  Full: Overshoots "
                             "and undershoots are avoided, but numerical diffusion is large");
  params.addRequiredParam<int>("slip_sys_index",
                               "Slip system index to determine slip direction "
                               "for instance from 0 to 11 for FCC.");
  params.addRequiredParam<int>("slip_sys_number",
                               "Slip system is determined by number of slip system "
                               "for different type: FCC, BCC, HCP.");
  params.addRequiredParam<int>(
      "activated_quadrant",
      "The number of quadrant that activated in"
      "commonly used 2 or 4, which represents a simple case and general case.");
  MooseEnum dislo_sign("positive negative", "positive");
  params.addRequiredParam<MooseEnum>("dislo_sign", dislo_sign, "Sign of dislocations.");
  MooseEnum dislo_character("edge screw", "edge");
  params.addRequiredParam<MooseEnum>(
      "dislo_character", dislo_character, "Character of dislocations: edge or screw.");

  params.addParam<Real>("burgers", 2.57e-7, "magnitude of burgers vector");
  params.addParam<Real>("ke_b", 55000.0, "edge hardening constants");
  params.addParam<Real>("ks_b", 110000.0, "screw hardening constants");
  params.addParam<Real>("de", 1.0e-6, "critical edge annihilation distance");
  params.addParam<Real>("ds", 5.0e-6, "critical screw annihilation distance");
  params.addParam<Real>("Ce", 0.5, "edge proportional constant");
  params.addParam<Real>("Cs", 0.5, "screw proportional constant");

  params.addCoupledVar(
      "edge_dislo_1_Q1", 0.0, "The edge dislocation density of Quadrant 1 of Slip system 1");
  params.addCoupledVar(
      "edge_dislo_1_Q3", 0.0, "The edge dislocation density of Quadrant 3 of Slip system 1");
  params.addCoupledVar(
      "edge_dislo_2_Q1", 0.0, "The edge dislocation density of Quadrant 1 of Slip system 2");
  params.addCoupledVar(
      "edge_dislo_2_Q3", 0.0, "The edge dislocation density of Quadrant 3 of Slip system 2");

  params.addParam<bool>("check_rho_positive", false, "Check positive dislocation density.");
  return params;
}

ConservativeSourceSink::ConservativeSourceSink(const InputParameters & parameters)
  : Kernel(parameters),
    _burgers(getParam<Real>("burgers")),
    _ke_b(getParam<Real>("ke_b")),
    _ks_b(getParam<Real>("ks_b")),
    _de(getParam<Real>("de")),
    _ds(getParam<Real>("ds")),
    _Ce(getParam<Real>("Ce")),
    _Cs(getParam<Real>("Cs")),

    _edge_dislo_1_Q1(coupledValue("edge_dislo_1_Q1")),
    _edge_dislo_1_Q3(coupledValue("edge_dislo_1_Q3")),
    _edge_dislo_2_Q1(coupledValue("edge_dislo_2_Q1")),
    _edge_dislo_2_Q3(coupledValue("edge_dislo_2_Q3")),

    _edge_dislo_1_Q1_var_number(coupled("edge_dislo_1_Q1")),
    _edge_dislo_1_Q3_var_number(coupled("edge_dislo_1_Q3")),
    _edge_dislo_2_Q1_var_number(coupled("edge_dislo_2_Q1")),
    _edge_dislo_2_Q3_var_number(coupled("edge_dislo_2_Q3")),

    _slip_sys_index(getParam<int>("slip_sys_index")),
    _slip_sys_number(getParam<int>("slip_sys_number")),
    _activated_quadrant(getParam<int>("activated_quadrant")),
    _dislo_sign(getParam<MooseEnum>("dislo_sign").getEnum<DisloSign>()),
    _dislo_character(getParam<MooseEnum>("dislo_character").getEnum<DisloCharacter>()),
    _check_rho_positive(getParam<bool>("check_rho_positive")),
    _slip_increment(getMaterialProperty<std::vector<Real>>("slip_increment"))
{
}

Real
ConservativeSourceSink::sourcesink()
{
  Real TotalDislocationDensity_ALL;
  Real source = 0.0;
  Real sink = 0.0;

  /// declare temporary total dislocation density
  std::vector<std::vector<std::vector<Real>>> temp_dislo_den;
  std::vector<Real> temp_edge_dislo_den;
  std::vector<Real> temp_screw_dislo_den;

  SlipSystem active_system;

  const Real abs_slip_increment = std::abs(_slip_increment[_qp][_slip_sys_index]);

  // Validate index
  if (_slip_sys_index > 12)
  {
    active_system = SlipSystem::UNSUPPORTED;
    std::cerr << "Error: Invalid slip system index. Using unsupported system." << std::endl;
  }
  else
  {
    active_system = static_cast<SlipSystem>(_slip_sys_index + 1);
  }

  int type_dislo = 2; // to initialize the set

  // Set the first dimension (x)
  temp_dislo_den.resize(_slip_sys_number);
  temp_edge_dislo_den.resize(_slip_sys_number);
  temp_screw_dislo_den.resize(_slip_sys_number);

  // Initialize the second and third dimensions (y, z)
  for (unsigned int i = 0; i < _slip_sys_number; ++i)
  {
    temp_dislo_den[i].resize(type_dislo);
    for (int j = 0; j < type_dislo; ++j)
    {
      temp_dislo_den[i][j].resize(_activated_quadrant);
    }
  }

  switch (active_system)
  {
    case SlipSystem::UNSUPPORTED:
      mooseWarning("Unsupported slip system selected. Code logic not implemented.");
      break;
    case SlipSystem::SLIP_SYSTEM_1:
      switch (_dislo_character)
      {
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = 0.0;
              std::cout << "_edge_dislo_1_Q3 value: " << _edge_dislo_1_Q3[_qp] << std::endl;
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = 0.0;
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
      }
      break;
    case SlipSystem::SLIP_SYSTEM_2:
      switch (_dislo_character)
      {
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = 0.0;
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = 0.0;
              break;
          }
          break;
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
      }
      break;
  }
  // temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
  // temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
  // temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
  // temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];

  TotalDislocationDensity_ALL = 0.0;
  for (const auto i : make_range(_slip_sys_number))
    for (const auto j : make_range(type_dislo))
      for (const auto k : make_range(_activated_quadrant))
      {
        TotalDislocationDensity_ALL += temp_dislo_den[i][j][k];
        if (j == 0)
          // The total dislocation density of edge dislocation density
          temp_edge_dislo_den[i] += temp_dislo_den[i][j][k];
        else if (j == 1)
          // The total dislocation density of screw dislocation density
          temp_screw_dislo_den[i] += temp_dislo_den[i][j][k];
        else
          mooseWarning("The type of dislocation density is more than 2.");
      }

  // Perform actions based on active system
  switch (active_system)
  {
    case SlipSystem::UNSUPPORTED:
      mooseWarning("Unsupported slip system selected. Code logic not implemented.");
      break;
    case SlipSystem::SLIP_SYSTEM_1:
      // this part is for the fcc system 1
      switch (_dislo_character)
      {
          // to calculate the source and sink terms of the edge dislocation density
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
              // here is a simple case that only positive and negative edge dislocation density
              // available in it, so only two option is given below
            case DisloSign::positive:
              // calculate the source and sink terms of positive edge dislocation density
              // TotalDislocationDensity_ALL = TotalDislocationDensity_ALL -
              // temp_dislo_den[0][0][0]; temp_edge_dislo_den[0] = temp_edge_dislo_den[0] -
              // temp_dislo_den[0][0][0];
              source = _Ce * _ke_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * (temp_edge_dislo_den[0] + _u[_qp]) *
                     abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative edge dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][0][1];
              temp_edge_dislo_den[0] = temp_edge_dislo_den[0] - temp_dislo_den[0][0][1];
              source = _Ce * _ke_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * (temp_edge_dislo_den[0] + _u[_qp]) *
                     abs_slip_increment;
              break;
          }
          break;

          // to calculate the source and sink terms of the screw dislocation density
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][1][0];
              temp_screw_dislo_den[0] = temp_screw_dislo_den[0] - temp_dislo_den[0][1][0];
              source = _Cs * _ks_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers *
                     (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                          std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) +
                      2.0 * _ds) *
                     temp_screw_dislo_den[0] * abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][1][1];
              temp_screw_dislo_den[0] = temp_screw_dislo_den[0] - temp_dislo_den[0][1][1];
              source = _Cs * _ks_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers *
                     (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                          std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) +
                      2.0 * _ds) *
                     temp_screw_dislo_den[0] * abs_slip_increment;
              break;
          }
          break;
      }
      break;

    case SlipSystem::SLIP_SYSTEM_2:
      switch (_dislo_character)
      {
          // to calculate the source and sink terms of the edge dislocation density
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][0][0];
              temp_edge_dislo_den[1] = temp_edge_dislo_den[1] - temp_dislo_den[1][0][0];
              source = _Ce * _ke_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * (temp_edge_dislo_den[1] + _u[_qp]) *
                     abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][0][1];
              temp_edge_dislo_den[1] = temp_edge_dislo_den[1] - temp_dislo_den[1][0][1];
              source = _Ce * _ke_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * (temp_edge_dislo_den[1] + _u[_qp]) *
                     abs_slip_increment;
              break;
          }
          break;

          // to calculate the source and sink terms of the screw dislocation density
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][1][0];
              temp_screw_dislo_den[1] = temp_screw_dislo_den[1] - temp_dislo_den[1][1][0];
              source = _Cs * _ks_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers *
                     (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                          std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) +
                      2.0 * _ds) *
                     temp_screw_dislo_den[1] * abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][1][1];
              temp_screw_dislo_den[1] = temp_screw_dislo_den[1] - temp_dislo_den[1][1][1];
              source = _Cs * _ks_b * std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers *
                     (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                          std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) +
                      2.0 * _ds) *
                     temp_screw_dislo_den[1] * abs_slip_increment;
              break;
          }
          break;
      }
      break;
      // ... Add cases for remaining FCC systems (3-12) with your specific code for each system
  }
  return _test[_i][_qp] * (source + sink);
}

Real
ConservativeSourceSink::computeQpResidual()
{
  if (_check_rho_positive && _u[_qp] <= 0.0)
  {

    return 0.0;
  }
  else
  {

    return sourcesink();
  }
}

Real
ConservativeSourceSink::sourcesink_jacobian()
{
  Real TotalDislocationDensity_ALL;
  Real source = 0.0;
  Real sink = 0.0;

  SlipSystem active_system;

  const Real abs_slip_increment = std::abs(_slip_increment[_qp][_slip_sys_index]);

  // Validate index
  if (_slip_sys_index > 12)
  {
    active_system = SlipSystem::UNSUPPORTED;
    std::cerr << "Error: Invalid slip system index. Using unsupported system." << std::endl;
  }
  else
  {
    active_system = static_cast<SlipSystem>(_slip_sys_index + 1);
  }

  int type_dislo = 2; // to initialize the set

  std::vector<std::vector<std::vector<Real>>> temp_dislo_den;
  std::vector<Real> temp_edge_dislo_den;
  std::vector<Real> temp_screw_dislo_den;

  // Set the first dimension (x)
  temp_dislo_den.resize(_slip_sys_number);
  temp_edge_dislo_den.resize(_slip_sys_number);
  temp_screw_dislo_den.resize(_slip_sys_number);

  // Initialize the second and third dimensions (y, z)
  for (unsigned int i = 0; i < _slip_sys_number; ++i)
  {
    temp_dislo_den[i].resize(type_dislo);
    for (int j = 0; j < type_dislo; ++j)
    {
      temp_dislo_den[i][j].resize(_activated_quadrant);
    }
  }

  switch (active_system)
  {
    case SlipSystem::UNSUPPORTED:
      mooseWarning("Unsupported slip system selected. Code logic not implemented.");
      break;
    case SlipSystem::SLIP_SYSTEM_1:
      switch (_dislo_character)
      {
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = 0.0;
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = 0.0;
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
      }
      break;
    case SlipSystem::SLIP_SYSTEM_2:
      switch (_dislo_character)
      {
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = 0.0;
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = 0.0;
              break;
          }
          break;
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
      }
      break;
  }
  // temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
  // temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
  // temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
  // temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];

  TotalDislocationDensity_ALL = 0.0;
  for (const auto i : make_range(_slip_sys_number))
    for (const auto j : make_range(type_dislo))
      for (const auto k : make_range(_activated_quadrant))
      {
        TotalDislocationDensity_ALL += temp_dislo_den[i][j][k];
        if (j == 0)
          // The total dislocation density of edge dislocation density
          temp_edge_dislo_den[i] += temp_dislo_den[i][j][k];
        else if (j == 1)
          // The total dislocation density of screw dislocation density
          temp_screw_dislo_den[i] += temp_dislo_den[i][j][k];
        else
          mooseWarning("The type of dislocation density is more than 2.");
      }

  // Perform actions based on active system
  switch (active_system)
  {
    case SlipSystem::UNSUPPORTED:
      mooseWarning("Unsupported slip system selected. Code logic not implemented.");
      break;
    case SlipSystem::SLIP_SYSTEM_1:
      // this part is for the fcc system 1
      switch (_dislo_character)
      {
          // to calculate the source and sink terms of the edge dislocation density
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
              // here is a simple case that only positive and negative edge dislocation density
              // available in it, so only two option is given below
            case DisloSign::positive:
              // calculate the source and sink terms of positive edge dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][0][0];
              temp_edge_dislo_den[0] = temp_edge_dislo_den[0] - temp_dislo_den[0][0][0];
              source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative edge dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][0][1];
              temp_edge_dislo_den[0] = temp_edge_dislo_den[0] - temp_dislo_den[0][0][1];
              source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
              break;
          }
          break;

          // to calculate the source and sink terms of the screw dislocation density
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][1][0];
              temp_screw_dislo_den[0] = temp_screw_dislo_den[0] - temp_dislo_den[0][1][0];
              source = _Cs * _ks_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers * 0.5 /
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         temp_screw_dislo_den[0] * abs_slip_increment -
                     _Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) * abs_slip_increment -
                     _Cs / _burgers * 2.0 * _ds * abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][1][1];
              temp_screw_dislo_den[0] = temp_screw_dislo_den[0] - temp_dislo_den[0][1][1];
              source = _Cs * _ks_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers * 0.5 /
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         temp_screw_dislo_den[0] * abs_slip_increment -
                     _Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) * abs_slip_increment -
                     _Cs / _burgers * 2.0 * _ds * abs_slip_increment;
              break;
          }
          break;
      }
      break;

    case SlipSystem::SLIP_SYSTEM_2:
      switch (_dislo_character)
      {
          // to calculate the source and sink terms of the edge dislocation density
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][0][0];
              temp_edge_dislo_den[1] = temp_edge_dislo_den[1] - temp_dislo_den[1][0][0];
              source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][0][1];
              temp_edge_dislo_den[1] = temp_edge_dislo_den[1] - temp_dislo_den[1][0][1];
              source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
              break;
          }
          break;

          // to calculate the source and sink terms of the screw dislocation density
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][1][0];
              temp_screw_dislo_den[1] = temp_screw_dislo_den[1] - temp_dislo_den[1][1][0];
              source = _Cs * _ks_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers * 0.5 /
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         temp_screw_dislo_den[1] * abs_slip_increment -
                     _Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) * abs_slip_increment -
                     _Cs / _burgers * 2.0 * _ds * abs_slip_increment;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][1][1];
              temp_screw_dislo_den[1] = temp_screw_dislo_den[1] - temp_dislo_den[1][1][1];
              source = _Cs * _ks_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                       abs_slip_increment;
              sink = -_Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers * 0.5 /
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         temp_screw_dislo_den[1] * abs_slip_increment -
                     _Cs / _burgers * M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                         std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) * abs_slip_increment -
                     _Cs / _burgers * 2.0 * _ds * abs_slip_increment;
              break;
          }
          break;
      }
      break;
      // ... Add cases for remaining FCC systems (3-12) with your specific code for each system
  }
  return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
}

Real
ConservativeSourceSink::computeQpJacobian()
{
  // This is the no-upwinded version
  // It gets called via Kernel::computeJacobian()

  if (_check_rho_positive && _u[_qp] <= 0.0)
  {

    return 0.0;
  }
  else
  {

    return sourcesink_jacobian();
  }
}

Real
ConservativeSourceSink::computeQpOffDiagJacobian(unsigned int jvar)
{
  // This is the no-upwinded version
  // It gets called via Kernel::computeOffDiagJacobian()
  // Perform actions based on active system
  Real TotalDislocationDensity_ALL;
  Real source = 0.0;
  Real sink = 0.0;

  /// declare temporary total dislocation density
  std::vector<std::vector<std::vector<Real>>> temp_dislo_den;
  std::vector<Real> temp_edge_dislo_den;
  std::vector<Real> temp_screw_dislo_den;

  SlipSystem active_system;

  const Real abs_slip_increment = std::abs(_slip_increment[_qp][_slip_sys_index]);

  // Validate index
  if (_slip_sys_index > 12)
  {
    active_system = SlipSystem::UNSUPPORTED;
    std::cerr << "Error: Invalid slip system index. Using unsupported system." << std::endl;
  }
  else
  {
    active_system = static_cast<SlipSystem>(_slip_sys_index + 1);
  }

  int type_dislo = 2; // to initialize the set

  // Set the first dimension (x)
  temp_dislo_den.resize(_slip_sys_number);
  temp_edge_dislo_den.resize(_slip_sys_number);
  temp_screw_dislo_den.resize(_slip_sys_number);

  // Initialize the second and third dimensions (y, z)
  for (unsigned int i = 0; i < _slip_sys_number; ++i)
  {
    temp_dislo_den[i].resize(type_dislo);
    for (int j = 0; j < type_dislo; ++j)
    {
      temp_dislo_den[i][j].resize(_activated_quadrant);
    }
  }

  switch (active_system)
  {
    case SlipSystem::UNSUPPORTED:
      mooseWarning("Unsupported slip system selected. Code logic not implemented.");
      break;
    case SlipSystem::SLIP_SYSTEM_1:
      switch (_dislo_character)
      {
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = 0.0;
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = 0.0;
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
      }
      break;
    case SlipSystem::SLIP_SYSTEM_2:
      switch (_dislo_character)
      {
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = 0.0;
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = 0.0;
              break;
          }
          break;
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
            case DisloSign::negative:
              temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
              temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
              temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
              temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];
              break;
          }
          break;
      }
      break;
  }
  // temp_dislo_den[0][0][0] = _edge_dislo_1_Q1[_qp];
  // temp_dislo_den[0][0][1] = _edge_dislo_1_Q3[_qp];
  // temp_dislo_den[1][0][0] = _edge_dislo_2_Q1[_qp];
  // temp_dislo_den[1][0][1] = _edge_dislo_2_Q3[_qp];

  TotalDislocationDensity_ALL = 0.0;
  for (const auto i : make_range(_slip_sys_number))
    for (const auto j : make_range(type_dislo))
      for (const auto k : make_range(_activated_quadrant))
      {
        TotalDislocationDensity_ALL += temp_dislo_den[i][j][k];
        if (j == 0)
          // The total dislocation density of edge dislocation density
          temp_edge_dislo_den[i] += temp_dislo_den[i][j][k];
        else if (j == 1)
          // The total dislocation density of screw dislocation density
          temp_screw_dislo_den[i] += temp_dislo_den[i][j][k];
        else
          mooseWarning("The type of dislocation density is more than 2.");
      }

  switch (active_system)
  {
    case SlipSystem::UNSUPPORTED:
      mooseWarning("Unsupported slip system selected. Code logic not implemented.");
      break;
    case SlipSystem::SLIP_SYSTEM_1:
      // this part is for the fcc system 1
      switch (_dislo_character)
      {
          // to calculate the source and sink terms of the edge dislocation density
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
              // here is a simple case that only positive and negative edge dislocation density
              // available in it, so only two option is given below
            case DisloSign::positive:
              // calculate the source and sink terms of positive edge dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][0][0];
              temp_edge_dislo_den[0] = temp_edge_dislo_den[0] - temp_dislo_den[0][0][0];
              if (jvar == _edge_dislo_1_Q3_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 1
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_2_Q1_var_number)
              {
                // this is to coupled the edge disloation of Q1 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_2_Q3_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else
                return 0.0;

              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative edge dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][0][1];
              temp_edge_dislo_den[0] = temp_edge_dislo_den[0] - temp_dislo_den[0][0][1];
              if (jvar == _edge_dislo_1_Q1_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 1
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_2_Q1_var_number)
              {
                // this is to coupled the edge disloation of Q1 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_2_Q3_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else
                return 0.0;

              break;
          }
          break;

          // to calculate the source and sink terms of the screw dislocation density
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][1][0];
              temp_screw_dislo_den[0] = temp_screw_dislo_den[0] - temp_dislo_den[0][1][0];

              // in this subroutine, no screw is considered, return 0
              return 0.0;

              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[0][1][1];
              temp_screw_dislo_den[0] = temp_screw_dislo_den[0] - temp_dislo_den[0][1][1];

              // in this subroutine, no screw is considered, return 0
              return 0.0;

              break;
          }
          break;
      }
      break;

    case SlipSystem::SLIP_SYSTEM_2:
      switch (_dislo_character)
      {
          // to calculate the source and sink terms of the edge dislocation density
        case DisloCharacter::edge:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][0][0];
              temp_edge_dislo_den[1] = temp_edge_dislo_den[1] - temp_dislo_den[1][0][0];

              if (jvar == _edge_dislo_1_Q1_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 1
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_1_Q3_var_number)
              {
                // this is to coupled the edge disloation of Q1 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_2_Q3_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else
                return 0.0;

              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][0][1];
              temp_edge_dislo_den[1] = temp_edge_dislo_den[1] - temp_dislo_den[1][0][1];

              if (jvar == _edge_dislo_1_Q1_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 1
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_1_Q3_var_number)
              {
                // this is to coupled the edge disloation of Q1 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = 0.0;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else if (jvar == _edge_dislo_2_Q1_var_number)
              {
                // this is to coupled the edge dislocation of Q3 of slip 2
                source = _Ce * _ke_b * 0.5 / std::sqrt(TotalDislocationDensity_ALL + _u[_qp]) *
                         abs_slip_increment;
                sink = -_Ce / _burgers * 2.0 * _de * abs_slip_increment;
                return _test[_i][_qp] * _phi[_j][_qp] * (source + sink);
              }
              else
                return 0.0;
              break;
          }
          break;

          // to calculate the source and sink terms of the screw dislocation density
        case DisloCharacter::screw:
          switch (_dislo_sign)
          {
            case DisloSign::positive:
              // calculate the source and sink terms of positive screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][1][0];
              temp_screw_dislo_den[1] = temp_screw_dislo_den[1] - temp_dislo_den[1][1][0];
              return 0.0;
              break;
            case DisloSign::negative:
              // calculate the source and sink terms of negative screw dislocation density
              TotalDislocationDensity_ALL = TotalDislocationDensity_ALL - temp_dislo_den[1][1][1];
              temp_screw_dislo_den[1] = temp_screw_dislo_den[1] - temp_dislo_den[1][1][1];
              return 0.0;
              break;
          }
          break;
      }
      break;
      // ... Add cases for remaining FCC systems (3-12) with your specific code for each system
  }
}

void
ConservativeSourceSink::computeResidual()
{
  Kernel::computeResidual();
}

void
ConservativeSourceSink::computeJacobian()
{
  Kernel::computeJacobian();
}

void
ConservativeSourceSink::computeOffDiagJacobian(const unsigned int jvar_num)
{
  Kernel::computeOffDiagJacobian(jvar_num);
}