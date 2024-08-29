[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 100
    ny = 10
    xmin = 0.0
    ymin = 0.0
    xmax = 500.0
    ymax = 500.0
    elem_type = 'QUAD4'
  []
[]

[Variables]
  [edge_dislo_den_pos_1]
    order = FIRST
    family = LAGRANGE
  []
  [edge_dislo_den_neg_1]
    order = FIRST
    family = LAGRANGE
  []
  # [edge_dislo_den_pos_2]
  #   initial_condition = 1.0
  # []
  # [edge_dislo_den_neg_2]
  #   initial_condition = 1.0
  # []
[]

[ICs]
  [./edge_dislo_den_pos_1]
    type = FunctionIC
    variable = 'edge_dislo_den_pos_1'
    function = Fluctuation
  []
  [./edge_dislo_den_neg_1]
    type = FunctionIC
    variable = 'edge_dislo_den_neg_1'
    function = Fluctuation
  []
[]

[Functions]
  [./Gaussian]
    type = ParsedFunction
    expression = '0.2 * (1 / sigma / sqrt(2.0 * pi) * exp(-((x-mu)^2/(2*sigma^2)))) + 0.5'
    symbol_names = 'sigma mu'
    symbol_values = '1.2 250.0'
  [../]
  [./Fluctuation]
    type = ParsedFunction
    expression = 'if(x=250, 0.55, 0.5)'
  [../]
[]

[AuxVariables]
  # [rhot]
  #   order = CONSTANT
  #   family = MONOMIAL_VEC
  # []
  # [rhognd]
  #   order = CONSTANT
  #   family = MONOMIAL_VEC
  # []
[]

[Kernels]
  [Edeg_Pos_Time_Deri_1]
    type = TimeDerivative
    variable = edge_dislo_den_pos_1
  []
  [Edge_Pos_Flux_1]
    implicit = false
    type = ConservativeAdvectionSchmid_NoMech_DS
    variable = edge_dislo_den_pos_1
    upwinding_type = none
      dislo_character = edge_00
      dislo_sign = positive
      slip_sys_index = 0
  []

  [Edeg_Neg_Time_Deri_1]
    type = TimeDerivative
    variable = edge_dislo_den_neg_1
  []
  [Edge_Neg_Flux_1]
    implicit = false
    type = ConservativeAdvectionSchmid_NoMech_DS
    variable = edge_dislo_den_neg_1
    upwinding_type = none
      dislo_character = edge_00
      dislo_sign = negative
      slip_sys_index = 0
  []

  # [Edeg_Pos_Time_Deri_2]
  #   type = TimeDerivative
  #   variable = edge_dislo_den_pos_2
  # []
  # [Edge_Pos_Flux_2]
  #   implicit = false
  #   type = ConservativeAdvectionSchmid_NoMech_DS
  #   variable = edge_dislo_den_pos_2
  #   upwinding_type = none
  #     dislo_character = edge_90
  #     dislo_sign = positive
  #     slip_sys_index = 1
  # []

  # [Edeg_Neg_Time_Deri_2]
  #   type = TimeDerivative
  #   variable = edge_dislo_den_neg_2
  # []
  # [Edge_Neg_Flux_2]
  #   implicit = false
  #   type = ConservativeAdvectionSchmid_NoMech_DS
  #   variable = edge_dislo_den_neg_2
  #   upwinding_type = none
  #     dislo_character = edge_90
  #     dislo_sign = negative
  #     slip_sys_index = 1
  # []

[]

[DGKernels]

  [dg_edge_pos_1]
    implicit = false
    type = DGAdvectionCoupled_NoMech_DS
    variable = edge_dislo_den_pos_1
      dislo_character = edge_00
      dislo_sign = positive
      slip_sys_index = 0
  []

  [dg_edge_neg_1]
    implicit = false
    type = DGAdvectionCoupled_NoMech_DS
    variable = edge_dislo_den_neg_1
      dislo_character = edge_00
      dislo_sign = negative
      slip_sys_index = 0
  []

  # [dg_edge_pos_2]
  #   implicit = false
  #   type = DGAdvectionCoupled_NoMech_DS
  #   variable = edge_dislo_den_pos_2
  #     dislo_character = edge_90
  #     dislo_sign = positive
  #     slip_sys_index = 1
  # []

  # [dg_edge_neg_2]
  #   implicit = false
  #   type = DGAdvectionCoupled_NoMech_DS
  #   variable = edge_dislo_den_neg_2
  #     dislo_character = edge_90
  #     dislo_sign = negative
  #     slip_sys_index = 1
  # []

[]

[AuxKernels]
  # [rhot]
  #   type = TotalDislocationDensity
  #   variable = rhot
  #   execute_on = timestep_end
  #   rhoe1 = rhoep
  #   rhoe3 = rhoen
  #   initial_total_dislo_den = 16.e3
  # []
  # [rhognd]
  #   type = GNDDislocationDensity
  #   variable = rhognd
  #   execute_on = timestep_end
  #   rhoe1 = rhoep
  #   rhoe3 = rhoen
  #   initial_total_dislo_den = 16.e3
  # []
[]

[Materials]
  [vel]
    type = DisloVelocity_DbSlip
    nss = 1
    taualpha = 0.5
    boltzmann = 1.0
    abstemp = 1.0
    tau0hat = 1.0
    gdot0 = 1.0
    f0 = 1.0
    shear_modulus = 1.0
    burgers = 1.0
    w1 = 0.0
    w2 = 0.0
      c_backs = 0.5
      c_diffu = 0.5
    edge_dislo_den_pos_1 = edge_dislo_den_pos_1
    edge_dislo_den_neg_1 = edge_dislo_den_neg_1
    # edge_dislo_den_pos_2 = edge_dislo_den_pos_2
    # edge_dislo_den_neg_2 = edge_dislo_den_neg_2
  []
  #  [mat_bc]
  #    type = ParsedMaterial
  #    property_name = mat_bc
  #    coupled_variables = 'rhoep rhoen'
  #    expression = '(rhoep + rhoen) * 1e-5'
  #    outputs = exodus
  #  []
[]

[BCs]
  [./Periodic]
    [./auto_boundary_edge_pos_1_x]
      variable = edge_dislo_den_pos_1
      auto_direction = 'x'
    [../]
    [./auto_boundary_edge_neg_1_x]
      variable = edge_dislo_den_neg_1
      auto_direction = 'x'
    [../]
    [./auto_boundary_edge_pos_1_y]
      variable = edge_dislo_den_pos_1
      auto_direction = 'y'
    [../]
    [./auto_boundary_edge_neg_1_y]
      variable = edge_dislo_den_neg_1
      auto_direction = 'y'
    [../]
    # [./auto_boundary_edge_pos_2]
    #   variable = edge_dislo_den_pos_2
    #   auto_direction = 'y'
    # [../]
    # [./auto_boundary_edge_neg_2]
    #   variable = edge_dislo_den_neg_2
    #   auto_direction = 'y'
    # [../]
  [../]
[]

# Transient (time-dependent) details for simulations go here:
[Executioner]
  type = Transient   # Here we use the Transient Executioner (instead of steady)

  [./TimeIntegrator]
    # type = ImplicitEuler
    # type = BDF2
    # type = CrankNicolson
    # type = ImplicitMidpoint
    # type = LStableDirk2
    # type = LStableDirk3
    # type = LStableDirk4
    # type = AStableDirk4
    #
    # Explicit methods
    type = ExplicitEuler
    # type = ExplicitMidpoint
    # type = Heun
    # type = Ralston
    # type = ExplicitTVDRK2
  [../]

  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value = 'lu boomeramg          31'
  line_search = 'none'
  l_max_its = 50
  nl_max_its = 50
  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-3
  l_tol = 1e-5

  start_time = 0.0
  end_time = 50.0
  dt = 2.e-4
  dtmin = 1.e-9
[]

[VectorPostprocessors]
  [edge_dislo_den_pos_1]
    type = LineValueSampler
    variable = edge_dislo_den_pos_1
    start_point = '0 0.005 0'
    end_point = '0.01 0.005 0'
    num_points = 41
    sort_by = x
  []
  [edge_dislo_den_neg_1]
    type = LineValueSampler
    variable = edge_dislo_den_neg_1
    start_point = '0 0.005 0'
    end_point = '0.01 0.005 0'
    num_points = 41
    sort_by = x
  []
[]

[Postprocessors]
  [slip_sys_1_pos]
    type = ElementIntegralVariablePostprocessor
    variable = edge_dislo_den_pos_1
  []
  [slip_sys_1_neg]
    type = ElementIntegralVariablePostprocessor
    variable = edge_dislo_den_neg_1
  []
  # [slip_sys_2_pos]
  #   type = ElementIntegralVariablePostprocessor
  #   variable = edge_dislo_den_pos_2
  # []
  # [slip_sys_2_neg]
  #   type = ElementIntegralVariablePostprocessor
  #   variable = edge_dislo_den_neg_2
  # []
[]

[Outputs]
  exodus = true
  time_step_interval = 125
  [csv]
    type = CSV
    file_base = patterning_test_x_out_l1e-1
    execute_on = final
  []
  [num_slip_sys_1_pos]
    type = CSV
    file_base = patterning_num_slip_sys_1_pos_wo_source_out
    show = 'slip_sys_1_pos'
  []
  [num_slip_sys_1_neg]
    type = CSV
    file_base = patterning_num_slip_sys_1_neg_wo_source_out
    show = 'slip_sys_1_neg'
  []
#   [num_slip_sys_2_pos]
#     type = CSV
#     file_base = patterning_num_slip_sys_2_pos_wo_source_out
#     show = 'slip_sys_2_pos'
#   []
#   [num_slip_sys_2_neg]
#     type = CSV
#     file_base = patterning_num_slip_sys_2_neg_wo_source_out
#     show = 'slip_sys_2_neg'
#   []
[]
