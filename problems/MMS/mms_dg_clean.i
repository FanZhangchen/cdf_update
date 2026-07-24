# ============================================================================
# MMS Convergence Test — Clean Baseline
#
# Uses MOOSE built-in DG kernels (ConservativeAdvection + DGConvection)
# with an exact solution that vanishes at boundaries, eliminating BC concerns.
#
# PDE:   ∂ρ/∂t + v·∂ρ/∂x = S(x,t),   v = 1.0
#
# Exact:  ρ(x,t)  = exp(-t) * x^2 * (1-x)^2
# Source: S(x,t)  = exp(-t) * (-x^4 + 6x^3 - 7x^2 + 2x)
#
# Key properties of the exact solution:
#   ρ(0,t) = ρ(1,t) = 0   and   ∂ρ/∂x|₀ = ∂ρ/∂x|₁ = 0
#   → no BCs needed, no flux across boundaries
#
# Mesh: N=50,100,200,400  (override via CLI)
# ============================================================================

[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 50
    ny = 1
    xmin = 0.0
    xmax = 1.0
    ymin = 0.0
    ymax = 0.02
  []
[]

[Variables]
  [rho]
    family = MONOMIAL
    order = FIRST
  []
[]

[ICs]
  [rho_ic]
    type = FunctionIC
    variable = rho
    function = rho_exact
  []
[]

[Functions]
  [rho_exact]
    type = ParsedFunction
    expression = 'exp(-t) * (x^4 - 2*x^3 + x^2)'
  []
  [src_func]
    type = ParsedFunction
    expression = 'exp(-t) * (-x^4 + 6*x^3 - 7*x^2 + 2*x)'
  []
[]

[Kernels]
  [time]
    type = TimeDerivative
    variable = rho
    implicit = true
  []
  [advection_vol]
    type = ConservativeAdvection
    variable = rho
    velocity = '1.0 0.0'
    implicit = false
  []
  [source]
    type = BodyForce
    variable = rho
    function = src_func
    implicit = false
  []
[]

[DGKernels]
  [advection_face]
    type = DGConvection
    variable = rho
    velocity = '1.0 0.0'
    implicit = false
  []
[]

# No [BCs] — exact solution vanishes at boundaries, zero flux

[Postprocessors]
  [l2_error]
    type = ElementL2Error
    variable = rho
    function = rho_exact
    execute_on = 'TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  solve_type = 'LINEAR'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'

  [TimeIntegrator]
    type = ExplicitTVDRK2
  []

  dt = 0.001
  dtmin = 1.0e-12
  start_time = 0.0
  end_time = 0.5
[]

[Outputs]
  [csv]
    type = CSV
  []
[]
