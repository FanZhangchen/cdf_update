//* DG hierarchical slope limiter for MONOMIAL FIRST variables
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation

#pragma once

#include "ElementUserObject.h"
#include "MooseEnum.h"

/**
 * DGSlopeLimiter1D applies a Cockburn-Shu hierarchical slope limiter to
 * MONOMIAL FIRST variables on structured QUAD4 meshes.
 *
 * For each element, the slope DOF (in the transport direction) is compared
 * with forward/backward finite-difference gradients computed from neighbouring
 * element means.  If the slope would create a new extremum (overshoot /
 * undershoot), it is limited via minmod (or vanleer / mc / superbee).
 *
 * Executed at TIMESTEP_END; does not feed back into the current-step assembly.
 *
 * IMPORTANT: Run with --n-threads=1 (the limiter directly modifies the
 * global solution vector, which is not thread-safe).
 */
class DGSlopeLimiter1D : public ElementUserObject
{
public:
  static InputParameters validParams();

  DGSlopeLimiter1D(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override;
  virtual void threadJoin(const UserObject & uo) override;

protected:
  /// Minmod of two scalars (zero if signs differ)
  Real minmod(Real a, Real b) const;

  /// Hierarchical minmod of three scalars
  Real minmod3(Real a, Real b, Real c) const;

  /// Van Leer limiter
  Real vanleer(Real a, Real b) const;

  /// MC (monotonized central-difference) limiter
  Real mcLimiter(Real a, Real b, Real c) const;

  /// Return the mean value (DOF 0) of the variable on element @p elem.
  /// Returns @p fallback if elem is nullptr (boundary).
  Real getElementMean(const Elem * elem, Real fallback) const;

  /// Apply the selected limiter: minmod3(slope, grad_plus, grad_minus)
  Real applyLimiter(Real slope, Real grad_plus, Real grad_minus) const;

  /// Limit one slope component (DOF index @p dof_idx) using neighbours on
  /// sides @p side_minus and @p side_plus, with boundary fallback values.
  void limitSlope(unsigned int dof_idx,
                  int side_minus, int side_plus,
                  Real bnd_minus, Real bnd_plus);

  /// Limiter type
  const MooseEnum _limiter_type;

  /// Variable name to limit
  const VariableName _var_name;

  /// Variable number (resolved on first call)
  unsigned int _var_num;

  /// Whether _var_num has been resolved
  bool _var_num_resolved;

  /// Reference to the nonlinear system
  NonlinearSystemBase & _nl_sys;

  /// System number
  const unsigned int _sys_num;

  /// Transport direction to limit: x, y, or both
  const MooseEnum _direction;

  /// Boundary fallback values
  const Real _bnd_left, _bnd_right;
  const Real _bnd_bottom, _bnd_top;
};
