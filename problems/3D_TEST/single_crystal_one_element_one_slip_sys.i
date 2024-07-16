[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 2
  xmin = 0.0
  ymin = 0.0
  zmin = 0.0
  xmax = 1.0
  ymax = 1.0
  zmax = 1.0
  elem_type = HEX8
[]

[Variables]
  [disp_x]
    order = FIRST
      family = LAGRANGE
  []
  [disp_y]
    order = FIRST
      family = LAGRANGE
  []
  [disp_z]
    order = FIRST
      family = LAGRANGE
  []
  [edge_dislo_1_Q1]
    initial_condition = 5.5e5
  []
  [edge_dislo_1_Q2]
    initial_condition = 5.5e5
  []
  [edge_dislo_1_Q3]
    initial_condition = 5.5e5
  []
  [edge_dislo_1_Q4]
    initial_condition = 5.5e5
  []
  [screw_dislo_1_Q1]
    initial_condition = 5.5e5
  []
  [screw_dislo_1_Q2]
    initial_condition = 5.5e5
  []
  [screw_dislo_1_Q3]
    initial_condition = 5.5e5
  []
  [screw_dislo_1_Q4]
    initial_condition = 5.5e5
  []
[]

[AuxVariables]
  [./pk2]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./fp_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./e_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_increment]
   order = CONSTANT
   family = MONOMIAL
  [../]
  [./dislo_velocity]
   order = CONSTANT
   family = MONOMIAL
  [../]
  [./epeq]
   order = CONSTANT
   family = MONOMIAL
  [../]
  [./gss]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./tau]
    order = CONSTANT
    family = MONOMIAL
  [../]
  # [./edge_dislocation_increment]
  #   order = CONSTANT
  #   family = MONOMIAL
  # [../]
  [total_dislo_den]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Functions]
  [./dts]
    type = PiecewiseLinear
    y = '0 1'
    x = '0 1000'
  [../]
[]

[Physics/SolidMechanics/QuasiStatic/all]
  strain = FINITE
  add_variables = true
  generate_output = 'stress_xx'
  additional_generate_output = 'strain_xx'
[]

[Kernels]
  # Slip 1 
  [Edge_Dislo_Den_Slip_1_Q1_Time]
    type = MassLumpedTimeDerivative
    variable = edge_dislo_1_Q1
  []
  [Edge_Dislo_Den_Slip_1_Q1_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = edge_dislo_1_Q1
    upwinding_type = full
      dislo_sign = positive
      slip_sys_index = 0
      dislo_character = edge
  []
  [Edge_Dislo_Den_Slip_1_Q1_Source]
    type = MatCoupledDisloDensityALL
    variable = edge_dislo_1_Q1
      dislo_character = edge
      slip_sys_index = 0
    v = 'edge_dislo_1_Q2 edge_dislo_1_Q3 edge_dislo_1_Q4 screw_dislo_1_Q1 screw_dislo_1_Q2 screw_dislo_1_Q3 screw_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

  [Edge_Dislo_Den_Slip_1_Q2_Time]
    type = MassLumpedTimeDerivative
    variable = edge_dislo_1_Q2
  []
  [Edge_Dislo_Den_Slip_1_Q2_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = edge_dislo_1_Q2
    upwinding_type = full
      dislo_sign = positive
      slip_sys_index = 0
      dislo_character = edge
  []
  [Edge_Dislo_Den_Slip_1_Q2_Source]
    type = MatCoupledDisloDensityALL
    variable = edge_dislo_1_Q2
      dislo_character = edge
      slip_sys_index = 0
    v = 'edge_dislo_1_Q1 edge_dislo_1_Q3 edge_dislo_1_Q4 screw_dislo_1_Q1 screw_dislo_1_Q2 screw_dislo_1_Q3 screw_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

  [Edge_Dislo_Den_Slip_1_Q3_Time]
    type = MassLumpedTimeDerivative
    variable = edge_dislo_1_Q3
  []
  [Edge_Dislo_Den_Slip_1_Q3_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = edge_dislo_1_Q3
    upwinding_type = full
      dislo_sign = negative
      slip_sys_index = 0
      dislo_character = edge
  []
  [Edge_Dislo_Den_Slip_1_Q3_Source]
    type = MatCoupledDisloDensityALL
    variable = edge_dislo_1_Q3
      dislo_character = edge
      slip_sys_index = 0
    v = 'edge_dislo_1_Q1 edge_dislo_1_Q2 edge_dislo_1_Q4 screw_dislo_1_Q1 screw_dislo_1_Q2 screw_dislo_1_Q3 screw_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

  [Edge_Dislo_Den_Slip_1_Q4_Time]
    type = MassLumpedTimeDerivative
    variable = edge_dislo_1_Q4
  []
  [Edge_Dislo_Den_Slip_1_Q4_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = edge_dislo_1_Q4
    upwinding_type = full
      dislo_sign = negative
      slip_sys_index = 0
      dislo_character = edge
  []
  [Edge_Dislo_Den_Slip_1_Q4_Source]
    type = MatCoupledDisloDensityALL
    variable = edge_dislo_1_Q4
      dislo_character = edge
      slip_sys_index = 0
    v = 'edge_dislo_1_Q1 edge_dislo_1_Q2 edge_dislo_1_Q3 screw_dislo_1_Q1 screw_dislo_1_Q2 screw_dislo_1_Q3 screw_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

  [screw_Dislo_Den_Slip_1_Q1_Time]
    type = MassLumpedTimeDerivative
    variable = screw_dislo_1_Q1
  []
  [screw_Dislo_Den_Slip_1_Q1_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = screw_dislo_1_Q1
    upwinding_type = full
      dislo_sign = positive
      slip_sys_index = 0
      dislo_character = screw
  []
  [screw_Dislo_Den_Slip_1_Q1_Source]
    type = MatCoupledDisloDensityALL
    variable = screw_dislo_1_Q1
      dislo_character = screw
      slip_sys_index = 0
    v = 'screw_dislo_1_Q2 screw_dislo_1_Q3 screw_dislo_1_Q4 edge_dislo_1_Q1 edge_dislo_1_Q2 edge_dislo_1_Q3 edge_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

  [screw_Dislo_Den_Slip_1_Q2_Time]
    type = MassLumpedTimeDerivative
    variable = screw_dislo_1_Q2
  []
  [screw_Dislo_Den_Slip_1_Q2_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = screw_dislo_1_Q2
    upwinding_type = full
      dislo_sign = negative
      slip_sys_index = 0
      dislo_character = screw
  []
  [screw_Dislo_Den_Slip_1_Q2_Source]
    type = MatCoupledDisloDensityALL
    variable = screw_dislo_1_Q2
      dislo_character = screw
      slip_sys_index = 0
    v = 'screw_dislo_1_Q1 screw_dislo_1_Q3 screw_dislo_1_Q4 edge_dislo_1_Q1 edge_dislo_1_Q2 edge_dislo_1_Q3 edge_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

  [screw_Dislo_Den_Slip_1_Q3_Time]
    type = MassLumpedTimeDerivative
    variable = screw_dislo_1_Q3
  []
  [screw_Dislo_Den_Slip_1_Q3_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = screw_dislo_1_Q3
    upwinding_type = full
      dislo_sign = negative
      slip_sys_index = 0
      dislo_character = screw
  []
  [screw_Dislo_Den_Slip_1_Q3_Source]
    type = MatCoupledDisloDensityALL
    variable = screw_dislo_1_Q3
      dislo_character = screw
      slip_sys_index = 0
    v = 'screw_dislo_1_Q1 screw_dislo_1_Q2 screw_dislo_1_Q4 edge_dislo_1_Q1 edge_dislo_1_Q2 edge_dislo_1_Q3 edge_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

  [screw_Dislo_Den_Slip_1_Q4_Time]
    type = MassLumpedTimeDerivative
    variable = screw_dislo_1_Q4
  []
  [screw_Dislo_Den_Slip_1_Q4_Flux]
    type = ConservativeAdvectionSchmidNoSSD
    variable = screw_dislo_1_Q4
    upwinding_type = full
      dislo_sign = positive
      slip_sys_index = 0
      dislo_character = screw
  []
  [screw_Dislo_Den_Slip_1_Q4_Source]
    type = MatCoupledDisloDensityALL
    variable = screw_dislo_1_Q4
      dislo_character = screw
      slip_sys_index = 0
    v = 'screw_dislo_1_Q1 screw_dislo_1_Q2 screw_dislo_1_Q3 edge_dislo_1_Q1 edge_dislo_1_Q2 edge_dislo_1_Q3 edge_dislo_1_Q4'
      ke_b = 52000
      ks_b = 104000
      de = 1.9e-6
      ds = 2.8e-6
    Ce = 0.5
  []

[]

[AuxKernels]
  [./pk2]
   type = RankTwoAux
   variable = pk2
   rank_two_tensor = second_piola_kirchhoff_stress
   index_j = 0
   index_i = 0
   execute_on = timestep_end
  [../]
  [./fp_xx]
    type = RankTwoAux
    variable = fp_xx
    rank_two_tensor = plastic_deformation_gradient
    index_j = 0
    index_i = 0
    execute_on = timestep_end
  [../]
  [./e_xx]
    type = RankTwoAux
    variable = e_xx
    rank_two_tensor = total_lagrangian_strain
    index_j = 0
    index_i = 0
    execute_on = timestep_end
  [../]
  [./tau]
   type = MaterialStdVectorAux
   variable = tau
   property = applied_shear_stress
   index = 0
   execute_on = timestep_end
  [../]
  [./gss]
   type = MaterialStdVectorAux
   variable = gss
   property = slip_resistance
   index = 0
   execute_on = timestep_end
  [../]
  [./slip_inc]
   type = MaterialStdVectorAux
   variable = slip_increment
   property = slip_increment
   index = 0
   execute_on = timestep_end
  [../]
  [./epeq]
   type = MaterialRealAux
   variable = epeq
   property = accumulated_equivalent_plastic_strain
   execute_on = timestep_end
  [../]
  [total_dislo_den]
    type = TotalDislocationDensitySingleSlipSys
    variable = total_dislo_den
      nss = 1
      initial_total_dislo_den = 4.4e6
    edge_dislo_den_1_Q1 = edge_dislo_1_Q1
    edge_dislo_den_1_Q2 = edge_dislo_1_Q2
    edge_dislo_den_1_Q3 = edge_dislo_1_Q3
    edge_dislo_den_1_Q4 = edge_dislo_1_Q4
    screw_dislo_den_1_Q1 = screw_dislo_1_Q1
    screw_dislo_den_1_Q2 = screw_dislo_1_Q2
    screw_dislo_den_1_Q3 = screw_dislo_1_Q3
    screw_dislo_den_1_Q4 = screw_dislo_1_Q4
  []
[]

[BCs]
  [./symmy]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmx]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./symmz]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./tdisp]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = right #front
    function = '0.003*t'
  [../]

  [./Periodic]
    [./auto_boundary_edge_dislo_1_Q1_x]
      variable = edge_dislo_1_Q1
      auto_direction = 'x'
    [../]
    [./auto_boundary_edge_dislo_1_Q1_y]
      variable = edge_dislo_1_Q1
      auto_direction = 'y'
    [../]
    [./auto_boundary_edge_dislo_1_Q1_z]
      variable = edge_dislo_1_Q1
      auto_direction = 'z'
    [../]
    [./auto_boundary_edge_dislo_1_Q2_x]
      variable = edge_dislo_1_Q2
      auto_direction = 'x'
    [../]
    [./auto_boundary_edge_dislo_1_Q2_y]
      variable = edge_dislo_1_Q2
      auto_direction = 'y'
    [../]
    [./auto_boundary_edge_dislo_1_Q2_z]
      variable = edge_dislo_1_Q2
      auto_direction = 'z'
    [../]
    [./auto_boundary_edge_dislo_1_Q3_x]
      variable = edge_dislo_1_Q3
      auto_direction = 'x'
    [../]
    [./auto_boundary_edge_dislo_1_Q3_y]
      variable = edge_dislo_1_Q3
      auto_direction = 'y'
    [../]
    [./auto_boundary_edge_dislo_1_Q3_z]
      variable = edge_dislo_1_Q3
      auto_direction = 'z'
    [../]
    [./auto_boundary_edge_dislo_1_Q4_x]
      variable = edge_dislo_1_Q4
      auto_direction = 'x'
    [../]
    [./auto_boundary_edge_dislo_1_Q4_y]
      variable = edge_dislo_1_Q4
      auto_direction = 'y'
    [../]
    [./auto_boundary_edge_dislo_1_Q4_z]
      variable = edge_dislo_1_Q4
      auto_direction = 'z'
    [../]
    [./auto_boundary_screw_dislo_1_Q1_x]
      variable = screw_dislo_1_Q1
      auto_direction = 'x'
      [../]
      [./auto_boundary_screw_dislo_1_Q1_y]
      variable = screw_dislo_1_Q1
      auto_direction = 'y'
      [../]
      [./auto_boundary_screw_dislo_1_Q1_z]
      variable = screw_dislo_1_Q1
      auto_direction = 'z'
      [../]
      [./auto_boundary_screw_dislo_1_Q2_x]
      variable = screw_dislo_1_Q2
      auto_direction = 'x'
      [../]
      [./auto_boundary_screw_dislo_1_Q2_y]
      variable = screw_dislo_1_Q2
      auto_direction = 'y'
      [../]
      [./auto_boundary_screw_dislo_1_Q2_z]
      variable = screw_dislo_1_Q2
      auto_direction = 'z'
      [../]
      [./auto_boundary_screw_dislo_1_Q3_x]
      variable = screw_dislo_1_Q3
      auto_direction = 'x'
      [../]
      [./auto_boundary_screw_dislo_1_Q3_y]
      variable = screw_dislo_1_Q3
      auto_direction = 'y'
      [../]
      [./auto_boundary_screw_dislo_1_Q3_z]
      variable = screw_dislo_1_Q3
      auto_direction = 'z'
      [../]
      [./auto_boundary_screw_dislo_1_Q4_x]
      variable = screw_dislo_1_Q4
      auto_direction = 'x'
      [../]
      [./auto_boundary_screw_dislo_1_Q4_y]
      variable = screw_dislo_1_Q4
      auto_direction = 'y'
      [../]
      [./auto_boundary_screw_dislo_1_Q4_z]
      variable = screw_dislo_1_Q4
      auto_direction = 'z'
      [../]
    
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeElasticityTensorCP
    C_ijkl = '188997.0 135312.0 135312.0 188997.0 135312.0 188997.0 90191.0 90191.0 90191.0'
    fill_method = symmetric9
    euler_angle_1 = 0.0
    euler_angle_2 = 0.0
    euler_angle_3 = 0.0

    # euler_angle_1 = -45.0
    # euler_angle_2 = 45.0
    # euler_angle_3 = -45.0

    # euler_angle_1 = 39.2315
    # euler_angle_2 = 65.9052
    # euler_angle_3 = -63.4349
  [../]
  [./stress]
    type = ComputeCrystalPlasticityDislocationStress
    crystal_plasticity_models = 'trial_xtalpl'
    tan_mod_type = exact
  [../]
  [./trial_xtalpl]
    type = CrystalPlasticityBussoUpdateSingleSlipSys
    number_slip_systems = 1
      w1 = 1.5
      w2 = 1.2
      tau_0 = 127.0
      p = 0.3
      q = 1.3
      f0 = 4.75e-19
      gdot0 = 1.e6
      scaling_Cb = 1.0
      shear_modulus = 90191
      burgers = 0.257e-6
    slip_sys_file_name = input_slip_single_slip_sys.txt
    edge_dislo_den_1_Q1 = edge_dislo_1_Q1
    edge_dislo_den_1_Q2 = edge_dislo_1_Q2
    edge_dislo_den_1_Q3 = edge_dislo_1_Q3
    edge_dislo_den_1_Q4 = edge_dislo_1_Q4
    screw_dislo_den_1_Q1 = screw_dislo_1_Q1
    screw_dislo_den_1_Q2 = screw_dislo_1_Q2
    screw_dislo_den_1_Q3 = screw_dislo_1_Q3
    screw_dislo_den_1_Q4 = screw_dislo_1_Q4
  [../]
[]

[Postprocessors]
  [./stress_xx]
    type = ElementAverageValue
    variable = stress_xx
  [../]
  [./pk2]
   type = ElementAverageValue
   variable = pk2
  [../]
  [./fp_xx]
    type = ElementAverageValue
    variable = fp_xx
  [../]
  [./e_xx]
    type = ElementAverageValue
    variable = e_xx
  [../]
  [./tau]
    type = ElementAverageValue
    variable = tau
  [../]
  [./gss]
    type = ElementAverageValue
    variable = gss
  [../]
  [./slip_increment]
   type = ElementAverageValue
   variable = slip_increment
  [../]
  [./epeq]
    type = ElementAverageValue
    variable = epeq
  [../]
  [./disp_x]
     type = NodalVariableValue
     variable = disp_x
     nodeid = 6
  [../]
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
  [../]
  [total_number_dislocation]
    type = ElementIntegralVariablePostprocessor
    variable = total_dislo_den
  []
  [slip_sys_1_pos]
    type = ElementIntegralVariablePostprocessor
    variable = edge_dislo_1_Q1
  []
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

# [Executioner]

#   [./TimeStepper]
#     type = IterationAdaptiveDT
#     dt = 1.e-3
#     growth_factor = 1.0000001
#     cutback_factor = 0.9
#     optimal_iterations = 15
#     iteration_window = 2
#     reject_large_step = true
#     reject_large_step_threshold = 0.1
#   [../]


#   type = Transient
#   nl_max_its = 15
#   scheme = bdf2
#   solve_type = PJFNK
#   petsc_options = '-snes_ksp_ew'
#   petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
#   petsc_options_value = '201                hypre    boomeramg      4'
#   l_max_its = 40
#   l_tol = 1.0e-3
#   nl_rel_tol = 1.0e-5
#   start_time = 0.0
#   num_steps = 20000
#   nl_abs_tol = 1e-5
#   line_search = none
#   snesmf_reuse_base = false
#   automatic_scaling = true
#   scaling_group_variables = 'disp_x disp_y disp_z'
#   [./Predictor]
#     type = SimplePredictor
#     scale = 1.0
#     skip_after_failed_timestep = true
#   [../]
# []
[Executioner]

  # type = Steady

  # solve_type = 'PJFNK'


  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'


  # type = Transient
  # solve_type = 'PJFNK'
  # solve_type = 'NEWTON'

  #petsc_options_iname = '-pc_type -pc_asm_overlap -sub_pc_type -ksp_type -ksp_gmres_restart'
  #petsc_options_value = ' asm      2              lu            gmres     200'
  # petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  # petsc_options_value = 'hypre boomeramg          31'
  # nl_abs_tol = 1e-3
  # nl_rel_tol = 1e-5
  # nl_abs_step_tol = 1e-10

  # dt = 5.e-6#0.03
  # dtmin = 1.e-12
  # dtmax = 10.0
  #num_steps = 10
  # end_time = 500

  type = Transient
  solve_type = 'PJFNK' #'NEWTON'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value = 'hypre    boomeramg          31'
  line_search = 'none'
  l_max_its = 50
  nl_max_its = 50
  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-3
  l_tol = 1e-5

  start_time = 0.0
  end_time = 10.0 #0.01
  dt = 5.e-4
  dtmin = 1.e-9

[]

[Outputs]
  exodus = true
  time_step_interval = 10
  [csv]
    type = CSV
    # file_base = data_output_100
    # file_base = data_output_110
    file_base = data_output_111
    execute_on = timestep_end
  []
  [total_dislo_den]
    type = CSV
    file_base = 3d_test_total_dislo_out
    show = 'total_number_dislocation'
  []
  [slip_sys_1_pos]
    type = CSV
    file_base = 3d_test_slip_sys_1_pos_out
    show = 'slip_sys_1_pos'
  []
[]

# [Debug]
#   show_material_props = true
# []