# ============================================================================
# MMS Convergence Test for DG Advection + TVDRK2
#
# PDE:  ∂ρ/∂t + v·∂ρ/∂x = S(x,t)    with v = 1.0
#
# Exact solution:  ρ(x,t) = exp(-t) * sin(2πx)
# Manufactured source: S(x,t) = exp(-t) * (2π·cos(2πx) - sin(2πx))
#
# Domain: x ∈ [0, 1], pseudo-1D (nx elements, ny=1)
# BCs:   Periodic in both x and y
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
  # Mass term: ∂ρ/∂t
  [time]
    type = TimeDerivative
    variable = rho
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
  # DG advection: v·∇ρ, with upwind flux (dislo_sign=positive, so velocity > 0 → upwind)
  [advection]
    type = DGAdvectionCoupled
    variable = rho
    dislo_sign = positive
    slip_sys_index = 0
    dislo_character = edge
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
      auto_direction = 'x'
    []
    [y]
      variable = rho
      auto_direction = 'y'
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
  [h]
    type = AverageElementSize
  []
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'jacobi'

  [TimeIntegrator]
    type = ExplicitTVDRK2
  []

  dt = 0.001                    # override via CLI: dt ∝ h for convergence study
  dtmin = 1.0e-12
  start_time = 0.0
  end_time = 0.5

  l_max_its = 10
  nl_max_its = 10
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-12
[]

[Outputs]
  exodus = true
  interval = 100
  [csv]
    type = CSV
  []
[]
