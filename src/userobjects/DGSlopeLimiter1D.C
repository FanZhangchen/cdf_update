//* DG hierarchical slope limiter for MONOMIAL FIRST variables
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
      "Cockburn-Shu hierarchical slope limiter for MONOMIAL FIRST variables on "
      "structured QUAD4 meshes.  Limits the slope DOF(s) in the specified transport "
      "direction using minmod/vanleer/mc/superbee.  Run with --n-threads=1.");

  MooseEnum limiter_type("minmod vanleer mc superbee", "minmod");
  params.addParam<MooseEnum>("limiter_type", limiter_type,
                             "Limiter: minmod (most diffusive), vanleer, mc, superbee (sharpest)");

  MooseEnum direction("x y both", "y");
  params.addParam<MooseEnum>("direction", direction,
                             "Transport direction to limit: x, y, or both");

  params.addRequiredParam<VariableName>("variable",
                                        "Name of the MONOMIAL FIRST variable to limit");

  params.addParam<Real>("bnd_left",   0.0, "Boundary value when no left neighbour");
  params.addParam<Real>("bnd_right",  0.0, "Boundary value when no right neighbour");
  params.addParam<Real>("bnd_bottom", 0.0, "Boundary value when no bottom neighbour");
  params.addParam<Real>("bnd_top",    0.0, "Boundary value when no top neighbour");

  return params;
}

DGSlopeLimiter1D::DGSlopeLimiter1D(const InputParameters & parameters)
  : ElementUserObject(parameters),
    _limiter_type(getParam<MooseEnum>("limiter_type")),
    _var_name(getParam<VariableName>("variable")),
    _var_num(libMesh::invalid_uint),
    _var_num_resolved(false),
    _nl_sys(_fe_problem.getNonlinearSystemBase(0)),
    _sys_num(_nl_sys.number()),
    _direction(getParam<MooseEnum>("direction")),
    _bnd_left(getParam<Real>("bnd_left")),
    _bnd_right(getParam<Real>("bnd_right")),
    _bnd_bottom(getParam<Real>("bnd_bottom")),
    _bnd_top(getParam<Real>("bnd_top"))
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
  if (a * b <= 0.0 || a * c <= 0.0)
    return 0.0;
  const Real sign_a = (a > 0.0) ? 1.0 : -1.0;
  return sign_a * std::min({std::abs(a), std::abs(b), std::abs(c)});
}

Real
DGSlopeLimiter1D::vanleer(Real a, Real b) const
{
  if (a * b <= 0.0)
    return 0.0;
  return (a * std::abs(b) + std::abs(a) * b) / (std::abs(a) + std::abs(b));
}

Real
DGSlopeLimiter1D::mcLimiter(Real a, Real b, Real c) const
{
  const Real central = 0.5 * (a + b);
  return minmod3(2.0 * a, 2.0 * b, central);
}

Real
DGSlopeLimiter1D::applyLimiter(Real slope, Real grad_plus, Real grad_minus) const
{
  switch (_limiter_type)
  {
    case 0: // minmod
      return minmod3(slope, grad_plus, grad_minus);
    case 1: // vanleer
    {
      Real s = vanleer(grad_minus, grad_plus);
      if (std::abs(s) > std::abs(slope))
        s = slope;
      return s;
    }
    case 2: // mc
      return mcLimiter(grad_minus, grad_plus, slope);
    case 3: // superbee
    {
      Real s1 = minmod(grad_plus, 2.0 * grad_minus);
      Real s2 = minmod(2.0 * grad_plus, grad_minus);
      Real s = (std::abs(s1) >= std::abs(s2)) ? s1 : s2;
      if (std::abs(s) > std::abs(slope))
        s = slope;
      return s;
    }
    default:
      return minmod3(slope, grad_plus, grad_minus);
  }
}

// ── Element mean access ───────────────────────────────────────────────────

Real
DGSlopeLimiter1D::getElementMean(const Elem * elem, Real fallback) const
{
  if (!elem)
    return fallback;

  const DofMap & dof_map = _nl_sys.dofMap();
  std::vector<dof_id_type> dof_indices;
  dof_map.dof_indices(elem, dof_indices, _var_num);

  if (dof_indices.empty())
    return fallback;

  return _nl_sys.solution()(dof_indices[0]);
}

// ── Core limiting logic ───────────────────────────────────────────────────

void
DGSlopeLimiter1D::limitSlope(unsigned int dof_idx,
                              int side_minus, int side_plus,
                              Real bnd_minus, Real bnd_plus)
{
  const Elem * elem = _current_elem;

  const DofMap & dof_map = _nl_sys.dofMap();
  std::vector<dof_id_type> dof_indices;
  dof_map.dof_indices(elem, dof_indices, _var_num);

  if (dof_indices.size() <= dof_idx)
    return;

  NumericVector<Number> & solution = _nl_sys.solution();

  // Current element
  const Real u_mean  = solution(dof_indices[0]);
  const Real sigma   = solution(dof_indices[dof_idx]);

  // Neighbours
  const Elem * neigh_minus = elem->neighbor_ptr(side_minus);
  const Elem * neigh_plus  = elem->neighbor_ptr(side_plus);

  const Real u_minus = neigh_minus ? getElementMean(neigh_minus, bnd_minus) : bnd_minus;
  const Real u_plus  = neigh_plus  ? getElementMean(neigh_plus,  bnd_plus)  : bnd_plus;

  // Centroid distances for unit–consistent gradient comparison
  const unsigned int dim = (side_minus == 0 || side_minus == 2) ? 1 : 0; // y=dim 1, x=dim 0

  const Real dx_plus  = neigh_plus  ? std::abs(neigh_plus->vertex_average()(dim)  - elem->vertex_average()(dim)) : 1.0;
  const Real dx_minus = neigh_minus ? std::abs(elem->vertex_average()(dim) - neigh_minus->vertex_average()(dim)) : 1.0;

  // Finite-difference gradients (units match the slope DOF: gradient * h/2)
  // For uniform mesh: dx_plus = dx_minus = h, so grad ≈ (Δu/h) * (h/2) = Δu/2
  // The slope DOF σ represents the half-jump: u(±1) = ū ± σ
  // We compare σ with (Δu⁺/2) and (Δu⁻/2) which are the effective half-jumps
  const Real h_ref = 0.5 * (dx_plus + dx_minus);
  const Real grad_plus  = (u_plus  - u_mean) * (0.5 * h_ref / dx_plus);
  const Real grad_minus = (u_mean - u_minus) * (0.5 * h_ref / dx_minus);

  // Apply limiter
  const Real sigma_limited = applyLimiter(sigma, grad_plus, grad_minus);

  if (std::abs(sigma_limited - sigma) > 1.0e-14)
    solution.set(dof_indices[dof_idx], sigma_limited);
}

// ── UserObject interface ──────────────────────────────────────────────────

void
DGSlopeLimiter1D::initialize()
{
  if (!_var_num_resolved)
  {
    _var_num = _fe_problem.getVariable(_tid, _var_name).number();
    _var_num_resolved = true;
  }
}

void
DGSlopeLimiter1D::execute()
{
  const Elem * elem = _current_elem;

  // Only process QUAD elements
  if (elem->type() != QUAD4 && elem->type() != QUAD9)
    return;

  // Limit x-slope (DOF 1)  — sides: left=3, right=1
  if (_direction == "x" || _direction == "both")
    limitSlope(/*dof_idx=*/1, /*side_minus=*/3, /*side_plus=*/1,
               _bnd_left, _bnd_right);

  // Limit y-slope (DOF 2)  — sides: bottom=0, top=2
  if (_direction == "y" || _direction == "both")
    limitSlope(/*dof_idx=*/2, /*side_minus=*/0, /*side_plus=*/2,
               _bnd_bottom, _bnd_top);
}

void
DGSlopeLimiter1D::finalize()
{
  // Close the solution vector so subsequent reads see the modified values.
  // (Required because we called solution.set() in execute().)
  _nl_sys.solution().close();
}

void
DGSlopeLimiter1D::threadJoin(const UserObject & /*uo*/)
{
  // No cross-thread data to merge — use --n-threads=1
}
