//* DG slope limiter for pseudo-1D MONOMIAL FIRST elements
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation

#include "DGSlopeLimiter1D.h"
#include "MooseVariableFE.h"
#include "NonlinearSystemBase.h"
#include "FEProblemBase.h"

#include "libmesh/elem.h"
#include "libmesh/dof_map.h"

registerMooseObject("cdf_updateApp", DGSlopeLimiter1D);

InputParameters
DGSlopeLimiter1D::validParams()
{
  InputParameters params = ElementUserObject::validParams();
  params.addClassDescription(
      "Hierarchical slope limiter for MONOMIAL FIRST variables on pseudo-1D meshes. "
      "Limits the x-slope DOF using the minmod (or vanleer/mc/superbee) function, "
      "comparing with forward/backward differences of element means.");

  MooseEnum limiter_type("minmod vanleer mc superbee", "minmod");
  params.addParam<MooseEnum>("limiter_type", limiter_type,
                             "Slope limiter type: minmod, vanleer, mc, superbee");

  params.addRequiredParam<VariableName>("variable", "Name of the MONOMIAL FIRST variable to limit");

  params.addParam<bool>("limit_y", false,
                        "Also limit the y-slope DOF (default: false, pseudo-1D mode)");

  params.addParam<Real>("left_boundary_value", 0.0,
                        "Value to use when no left neighbour exists (default: 0)");
  params.addParam<Real>("right_boundary_value", 0.0,
                        "Value to use when no right neighbour exists (default: 0)");

  params.addParam<bool>("verbose", false, "Print limiter activity statistics");
  return params;
}

DGSlopeLimiter1D::DGSlopeLimiter1D(const InputParameters & parameters)
  : ElementUserObject(parameters),
    _limiter_type(getParam<MooseEnum>("limiter_type")),
    _var_name(getParam<VariableName>("variable")),
    _var_num(libMesh::invalid_uint),
    _var_num_resolved(false),
    _nl_sys(_fe_problem.getNonlinearSystemBase()),
    _sys_num(_nl_sys.number()),
    _limit_y(getParam<bool>("limit_y")),
    _left_boundary_value(getParam<Real>("left_boundary_value")),
    _right_boundary_value(getParam<Real>("right_boundary_value"))
{
}

// ── Limiter functions ─────────────────────────────────────────────────────

Real
DGSlopeLimiter1D::minmod(Real a, Real b) const
{
  if (a * b <= 0.0)
    return 0.0;
  return (std::abs(a) < std::abs(b)) ? a : b;
}

Real
DGSlopeLimiter1D::minmod3(Real a, Real b, Real c) const
{
  // Standard hierarchical minmod: zero if signs differ, else the min modulus
  if (a * b <= 0.0 || a * c <= 0.0)
    return 0.0;
  Real sign_a = (a > 0.0) ? 1.0 : -1.0;
  return sign_a * std::min({std::abs(a), std::abs(b), std::abs(c)});
}

Real
DGSlopeLimiter1D::vanleer(Real a, Real b) const
{
  // van Leer: (a*|b| + |a|*b) / (|a| + |b|),  zero if a*b <= 0
  if (a * b <= 0.0)
    return 0.0;
  return (a * std::abs(b) + std::abs(a) * b) / (std::abs(a) + std::abs(b));
}

Real
DGSlopeLimiter1D::mcLimiter(Real a, Real b, Real c) const
{
  // MC (monotonized central): minmod(2*a, 2*b, c) where c = (a+b)/2
  Real central = 0.5 * (a + b);
  return minmod3(2.0 * a, 2.0 * b, central);
}

// ── Element mean access ───────────────────────────────────────────────────

Real
DGSlopeLimiter1D::getElementMean(const Elem * elem) const
{
  if (!elem)
    return 0.0; // boundary fallback

  // Get DOF indices for the variable on this element
  const DofMap & dof_map = _nl_sys.dofMap();
  std::vector<dof_id_type> dof_indices;
  dof_map.dof_indices(elem, dof_indices, _var_num);

  if (dof_indices.empty())
    return 0.0;

  // DOF 0 is the constant term (element mean for MONOMIAL FIRST)
  return _nl_sys.solution()(dof_indices[0]);
}

// ── UserObject interface ──────────────────────────────────────────────────

void
DGSlopeLimiter1D::initialize()
{
  // Resolve variable number once (it stays constant across elements)
  if (!_var_num_resolved)
  {
    const Variable & var = _fe_problem.getVariable(_tid, _var_name);
    _var_num = var.number();
    _var_num_resolved = true;
  }
}

void
DGSlopeLimiter1D::execute()
{
  const Elem * elem = _current_elem;

  // Skip non-quad elements (safety check for pseudo-1D mesh)
  if (elem->type() != QUAD4 && elem->type() != QUAD9)
    return;

  // Get DOF indices for this element
  const DofMap & dof_map = _nl_sys.dofMap();
  std::vector<dof_id_type> dof_indices;
  dof_map.dof_indices(elem, dof_indices, _var_num);

  // MONOMIAL FIRST on QUAD4 has 3 DOFs: [mean, x_slope, y_slope]
  if (dof_indices.size() < 2)
    return;

  NumericVector<Number> & solution = _nl_sys.solution();

  const Real u_mean = solution(dof_indices[0]);
  const Real sigma_x = solution(dof_indices[1]);

  // ── Limit x-slope ──────────────────────────────────────────────────
  // Neighbour means (left = side 3, right = side 1 for structured quad)
  const Elem * left_neighbor  = elem->neighbor_ptr(3);
  const Elem * right_neighbor = elem->neighbor_ptr(1);

  const Real u_left =
      left_neighbor ? getElementMean(left_neighbor) : _left_boundary_value;
  const Real u_right =
      right_neighbor ? getElementMean(right_neighbor) : _right_boundary_value;

  // Forward and backward differences
  const Real delta_plus  = u_right - u_mean;
  const Real delta_minus = u_mean - u_left;

  // Apply limiter
  Real sigma_limited;
  switch (_limiter_type)
  {
    case 0: // minmod
      sigma_limited = minmod3(sigma_x, delta_plus, delta_minus);
      break;
    case 1: // vanleer
      sigma_limited = vanleer(delta_minus, delta_plus);
      // clamp: cannot exceed original slope magnitude
      if (std::abs(sigma_limited) > std::abs(sigma_x))
        sigma_limited = sigma_x;
      break;
    case 2: // mc
      sigma_limited = mcLimiter(delta_minus, delta_plus, sigma_x);
      break;
    case 3: // superbee
    {
      Real s1 = minmod(delta_plus, 2.0 * delta_minus);
      Real s2 = minmod(2.0 * delta_plus, delta_minus);
      sigma_limited = (std::abs(s1) >= std::abs(s2)) ? s1 : s2;
      // clamp
      if (std::abs(sigma_limited) > std::abs(sigma_x))
        sigma_limited = sigma_x;
      break;
    }
    default:
      sigma_limited = minmod3(sigma_x, delta_plus, delta_minus);
  }

  // Overwrite if limited value differs
  if (std::abs(sigma_limited - sigma_x) > 1.0e-14)
    solution.set(dof_indices[1], sigma_limited);

  // ── Optionally limit y-slope ───────────────────────────────────────
  if (_limit_y && dof_indices.size() > 2)
  {
    const Real sigma_y = solution(dof_indices[2]);
    const Elem * bottom_neighbor = elem->neighbor_ptr(0);
    const Elem * top_neighbor    = elem->neighbor_ptr(2);

    const Real u_bottom = bottom_neighbor ? getElementMean(bottom_neighbor) : 0.0;
    const Real u_top    = top_neighbor    ? getElementMean(top_neighbor)    : 0.0;

    const Real dy_plus  = u_top - u_mean;
    const Real dy_minus = u_mean - u_bottom;

    Real sigma_y_limited = minmod3(sigma_y, dy_plus, dy_minus);
    if (std::abs(sigma_y_limited - sigma_y) > 1.0e-14)
      solution.set(dof_indices[2], sigma_y_limited);
  }
}

void
DGSlopeLimiter1D::finalize()
{
  // No global reduction needed — each element is limited independently
}

void
DGSlopeLimiter1D::threadJoin(const UserObject & /*uo*/)
{
  // Thread-join not needed for serial-compatible element-wise limiting.
  // In parallel, each process limits its own elements independently.
  // The solution vector is ghosted so neighbour reads are safe.
}
