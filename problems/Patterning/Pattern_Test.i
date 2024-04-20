[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 1
    ny = 1
    xmin = 0.0
    ymin = 0.0
    xmax = 0.01
    ymax = 0.01    
  []
[]

[Variables]
  [edge_dislo_den_pos_1]
    initial_condition = 8.e3
  []
  [edge_dislo_den_neg_1]
    initial_condition = 8.e3
  []
  [edge_dislo_den_pos_2]
    initial_condition = 8.e3
  []
  [edge_dislo_den_neg_2]
    initial_condition = 8.e3
  []
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
    type = ConservativeAdvectionSchmid_NoMech_DS
    variable = edge_dislo_den_neg_1
    upwinding_type = none
      dislo_character = edge_00
      dislo_sign = negative
      slip_sys_index = 0
  []

  [Edeg_Pos_Time_Deri_2]
    type = TimeDerivative
    variable = edge_dislo_den_pos_2
  []
  [Edge_Pos_Flux_2]
    type = ConservativeAdvectionSchmid_NoMech_DS
    variable = edge_dislo_den_pos_2
    upwinding_type = none
      dislo_character = edge_90
      dislo_sign = positive
      slip_sys_index = 1
  []
  [Edeg_Neg_Time_Deri_2]
    type = TimeDerivative
    variable = edge_dislo_den_neg_2
  []
  [Edge_Neg_Flux_2]
    type = ConservativeAdvectionSchmid_NoMech_DS
    variable = edge_dislo_den_neg_2
    upwinding_type = none
      dislo_character = edge_90
      dislo_sign = negative
      slip_sys_index = 1
  []
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
    nss = 2
    taualpha = 2.63
    w1 = 0.0
    w2 = 0.0
    edge_dislo_den_pos_1 = edge_dislo_den_pos_1
    edge_dislo_den_neg_1 = edge_dislo_den_neg_1
    edge_dislo_den_pos_2 = edge_dislo_den_pos_2
    edge_dislo_den_neg_2 = edge_dislo_den_neg_2
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
[]

# Transient (time-dependent) details for simulations go here:
[Executioner]
  type = Transient   # Here we use the Transient Executioner (instead of steady)
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg          31'
  line_search = 'none'
  l_max_its = 50
  nl_max_its = 50
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-6
  l_tol = 1e-8

  start_time = 0.0
  end_time = 0.2
  dt = 2.e-6
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
[]

[Outputs]
  exodus = true
  interval = 50
  [csv]
    type = CSV
    file_base = patterning_test_x_out_l1e-1
    execute_on = final
  []
[]