# ============================================================================
# MMS Convergence Test with Slope Limiter
#
# Identical to mms_dg_tvd.i but with DGSlopeLimiter1D applied at each
# timestep end.  Use this to compare convergence with/without limiting.
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
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho
    upwinding_type = none
    dislo_sign = positive
    slip_sys_index = 0
    dislo_character = edge
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
  [advection]
    type = DGAdvectionCoupled
    variable = rho
    dislo_sign = positive
    slip_sys_index = 0
    dislo_character = edge
    implicit = false
  []
[]

[Materials]
  [const_vel]
    type = ConstantDislocationVelocity
    nss = 1
    velocity = 1.0
    edge_direction = 1.0
  []
[]

# No BCs — exact solution vanishes at boundaries

[UserObjects]
  [slope_limiter]
    type = DGSlopeLimiter1D
    variable = rho
    limiter_type = minmod
    left_boundary_value = 0.0
    right_boundary_value = 0.0
    execute_on = 'TIMESTEP_END'
  []
[]

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
