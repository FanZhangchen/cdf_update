# ============================================================================
# MMS Convergence Test for DG Advection + ExplicitTVDRK2
#
# PDE:  ∂ρ/∂t + v·∂ρ/∂x = S(x,t)    with v = 1.0
#
# Exact solution:  ρ(x,t) = exp(-t) * sin(2πx)
# Manufactured source: S(x,t) = exp(-t) * (2π·cos(2πx) - sin(2πx))
#
# Domain: x ∈ [0, 1], pseudo-1D (nx elements, ny=1)
# BCs:   Periodic in x
#
# Usage:
#   N=50:  ./cdf_update-opt -i mms_dg_tvd.i Mesh/gen/nx=50  Executioner/dt=0.001
#   N=100: ./cdf_update-opt -i mms_dg_tvd.i Mesh/gen/nx=100 Executioner/dt=0.0005
#   N=200: ./cdf_update-opt -i mms_dg_tvd.i Mesh/gen/nx=200 Executioner/dt=0.00025
# ============================================================================

[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 50                       # override via CLI for convergence study
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
    order = FIRST                 # piecewise linear → second-order spatial accuracy
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
    expression = 'exp(-t) * sin(2*pi*x)'
  []
  [src_func]
    type = ParsedFunction
    expression = 'exp(-t) * (2*pi*cos(2*pi*x) - sin(2*pi*x))'
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

[BCs]
  [Periodic]
    [x]
      variable = rho
      primary = 'left'
      secondary = 'right'
      translation = '1.0 0.0 0.0'
    []
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
