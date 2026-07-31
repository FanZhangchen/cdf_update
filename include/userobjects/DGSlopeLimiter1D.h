//* DG slope limiter for pseudo-1D MONOMIAL FIRST elements
//* Zhangchen Fan
//* Harbin Institute of Technology, Shenzhen
//* Centre for Micro-mechanics Modelling and Characterisation

#pragma once

#include "ElementUserObject.h"
#include "MooseEnum.h"

/**
 * DGSlopeLimiter1D applies a hierarchical slope limiter (Cockburn-Shu style)
 * to a MONOMIAL FIRST variable on a pseudo-1D mesh (dim=2, ny=1).
 *
 * For each element, the x-slope DOF is compared with the forward and backward
 * differences of element means.  If the slope creates a new extremum, it is
 * limited using the minmod function.  This is a post-processing step executed
 * at TIMESTEP_END — it does not feed back into the residual assembly of the
 * current time step.
 *
 * Supported limiter types: minmod, vanleer, mc, superbee.
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
  /// Minmod of two arguments
  Real minmod(Real a, Real b) const;

  /// Minmod of three arguments (standard hierarchical limiter)
  Real minmod3(Real a, Real b, Real c) const;

  /// Van Leer limiter
  Real vanleer(Real a, Real b) const;

  /// MC (monotonized central) limiter
  Real mcLimiter(Real a, Real b, Real c) const;

  /// Get the mean value (DOF 0) of the solution on a given element.
  /// Returns 0 if elem is nullptr (boundary).
  Real getElementMean(const Elem * elem) const;

  /// Limiter type
  const MooseEnum _limiter_type;

  /// Name of the variable to limit
  const VariableName _var_name;

  /// Variable number (resolved on first call to initialize())
  unsigned int _var_num;

  /// Flag: has _var_num been resolved yet?
  bool _var_num_resolved;

  /// Reference to the nonlinear system (for accessing/modifying the solution)
  NonlinearSystemBase & _nl_sys;

  /// System number
  const unsigned int _sys_num;

  /// Whether to also limit the y-slope (default: false for pseudo-1D)
  const bool _limit_y;

  /// Boundary values for elements without neighbours
  const Real _left_boundary_value;
  const Real _right_boundary_value;
};
