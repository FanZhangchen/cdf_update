# ============================================================================
# MMS Convergence Test — Custom DG Kernels  (after DGAdvectionCoupled fix)
#
# PDE:  ∂ρ/∂t + v·∂ρ/∂x = S(x,t)    with v = 1.0
#
# Exact:  ρ(x,t) = exp(-t) * x^2 * (1-x)^2
# Source: S(x,t) = exp(-t) * (-x^4 + 6x^3 - 7x^2 + 2x)
#
# Exact solution vanishes at boundaries → no BCs needed (avoids DG
# periodic-BC compatibility issues).  Apples-to-apples with mms_dg_clean.i.
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
  # Mass term: ∂ρ/∂t  — must stay implicit for explicit time integrators
  [time]
    type = TimeDerivative
    variable = rho
    implicit = true
  []
  # Volume advection: -∫ vρ · ∇φ dx  (no upwinding — DG handles it at interfaces)
  [advection_vol]
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho
    upwinding_type = none
    dislo_sign = positive
    slip_sys_index = 0
    dislo_character = edge
    implicit = false
  []
  # Manufactured source term S(x,t)
  [source]
    type = BodyForce
    variable = rho
    function = src_func
    implicit = false
  []
[]

[DGKernels]
  # DG advection: v·∇ρ, upwind flux — implicit = false for explicit RK
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

# No BCs — exact solution vanishes at boundaries, zero flux

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

  dt = 0.001                    # override via CLI: dt ∝ h for convergence study
  dtmin = 1.0e-12
  start_time = 0.0
  end_time = 0.5
[]

[Outputs]
  [csv]
    type = CSV
  []
[]
