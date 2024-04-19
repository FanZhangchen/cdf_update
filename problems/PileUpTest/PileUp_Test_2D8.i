[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 50
    ny = 50
    xmin = 0.0
    ymin = 0.0
    xmax = 0.1
    ymax = 0.1    
  []
[]

[Variables]
  [rhoe1]
    initial_condition = 8.e3
  []
  [rhoe2]
    initial_condition = 8.e3
  []
  [rhoe3]
    initial_condition = 8.e3
  []
  [rhoe4]
    initial_condition = 8.e3
  []
  [rhos1]
    initial_condition = 8.e3
  []
  [rhos2]
    initial_condition = 8.e3
  []
  [rhos3]
    initial_condition = 8.e3
  []
  [rhos4]
    initial_condition = 8.e3
  []
[]

# [AuxVariables]
#   [rhot]
#     order = CONSTANT
#     family = MONOMIAL_VEC
#   []
#   [rhognd]
#     order = CONSTANT
#     family = MONOMIAL_VEC
#   []
# []

[Kernels]

  [Edge_Dislo_Den_Time_1]
    type = TimeDerivative
    variable = rhoe1
  []
  [Edge_Dislo_Den_Flux_1]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhoe1
    upwinding_type = none
      dislo_character = edge
      dislo_sign = positive
      slip_sys_index = 0
    scale = 1.0
  []

  [Edge_Dislo_Den_Time_2]
    type = TimeDerivative
    variable = rhoe2
  []
  [Edge_Dislo_Den_Flux_2]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhoe2
    upwinding_type = none
      dislo_character = edge
      dislo_sign = negative
      slip_sys_index = 0
    scale = 1.0
  []

  [Edge_Dislo_Den_Time_3]
    type = TimeDerivative
    variable = rhoe3
  []
  [Edge_Dislo_Den_Flux_3]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhoe3
    upwinding_type = none
      dislo_character = edge
      dislo_sign = negative
      slip_sys_index = 0
    scale = 1.0
  []

  [Edge_Dislo_Den_Time_4]
    type = TimeDerivative
    variable = rhoe4
  []
  [Edge_Dislo_Den_Flux_4]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhoe4
    upwinding_type = none
      dislo_character = edge
      dislo_sign = positive
      slip_sys_index = 0
    scale = 1.0
  []

  [Screw_Dislo_Den_Time_1]
    type = TimeDerivative
    variable = rhos1
  []
  [Screw_Dislo_Den_Flux_1]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhos1
    upwinding_type = none
      dislo_character = screw
      dislo_sign = positive
      slip_sys_index = 0
    scale = 1.0
  []

  [Screw_Dislo_Den_Time_2]
    type = TimeDerivative
    variable = rhos2
  []
  [Screw_Dislo_Den_Flux_2]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhos2
    upwinding_type = none
      dislo_character = screw
      dislo_sign = positive
      slip_sys_index = 0
    scale = 1.0
  []

  [Screw_Dislo_Den_Time_3]
    type = TimeDerivative
    variable = rhos3
  []
  [Screw_Dislo_Den_Flux_3]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhos3
    upwinding_type = none
      dislo_character = screw
      dislo_sign = negative
      slip_sys_index = 0
    scale = 1.0
  []

  [Screw_Dislo_Den_Time_4]
    type = TimeDerivative
    variable = rhos4
  []
  [Screw_Dislo_Den_Flux_4]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhos4
    upwinding_type = none
      dislo_character = screw
      dislo_sign = negative
      slip_sys_index = 0
    scale = 1.0
  []
[]

[AuxKernels]
  # [rhot]
  #   type = TotalDislocationDensity
  #   variable = rhot
  #   execute_on = timestep_end
  #   rhoep = rhoep
  #   rhoen = rhoen
  # []
  # [rhognd]
  #   type = GNDDislocationDensity
  #   variable = rhognd
  #   execute_on = timestep_end
  #   rhoep = rhoep
  #   rhoen = rhoen
  # []
[]

[Materials]
  [vel]
    type = DisloVelocity_2D8
    nss = 1
    edge_dislo_den_1 = rhoe1
    edge_dislo_den_2 = rhoe2
    edge_dislo_den_3 = rhoe3
    edge_dislo_den_4 = rhoe4
    screw_dislo_den_1 = rhos1
    screw_dislo_den_2 = rhos2
    screw_dislo_den_3 = rhos3
    screw_dislo_den_4 = rhos4
    scale = 1.0
  []
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
  [rhoep]
    type = LineValueSampler
    variable = rhoe1
    start_point = '0 0.05 0'
    end_point = '0.1 0.05 0'
    num_points = 41
    sort_by = x
  []
  [rhoen]
    type = LineValueSampler
    variable = rhos1
    start_point = '0 0.05 0'
    end_point = '0.1 0.05 0'
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
    file_base = test_rhoe_x_out_l1e-1_2d8
    execute_on = final
  []
[]
