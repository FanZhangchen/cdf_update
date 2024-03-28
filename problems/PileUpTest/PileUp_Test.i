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
  [rhoep]
    initial_condition = 8.e3
  []
  [rhoen]
    initial_condition = 8.e3
  []
[]

[AuxVariables]
  [rhot]
    order = CONSTANT
    family = MONOMIAL_VEC
  []
  [rhognd]
    order = CONSTANT
    family = MONOMIAL_VEC
  []
[]

[Kernels]
  [Edeg_Pos_Time_Deri]
    type = TimeDerivative
    variable = rhoep
  []
  [Edge_Pos_Flux]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhoep
    upwinding_type = none
      dislo_character = edge
      dislo_sign = positive
      slip_sys_index = 0
  []
  [Edeg_Neg_Time_Deri]
    type = TimeDerivative
    variable = rhoen
  []
  [Edge_Neg_Flux]
    type = ConservativeAdvectionSchmid_NoMech
    variable = rhoen
    upwinding_type = none
      dislo_character = edge
      dislo_sign = negative
      slip_sys_index = 0
  []
[]

[AuxKernels]
  [rhot]
    type = TotalDislocationDensity
    variable = rhot
    execute_on = timestep_end
    rhoe1 = rhoep
    rhoe3 = rhoen
    initial_total_dislo_den = 16.e3
  []
  [rhognd]
    type = GNDDislocationDensity
    variable = rhognd
    execute_on = timestep_end
    rhoe1 = rhoep
    rhoe3 = rhoen
    initial_total_dislo_den = 16.e3
  []
[]

[Materials]
  [vel]
    type = DisloVelocity_1D
    nss = 1
    rhoen = rhoen
    rhoep = rhoep
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
  [rhoep]
    type = LineValueSampler
    variable = rhoep
    start_point = '0 0.05 0'
    end_point = '0.1 0.05 0'
    num_points = 41
    sort_by = x
  []
  [rhoen]
    type = LineValueSampler
    variable = rhoen
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
    file_base = test_rhoe_x_out_l1e-1
    execute_on = final
  []
[]
