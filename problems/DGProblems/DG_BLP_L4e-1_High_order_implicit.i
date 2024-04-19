[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [./gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 1
    ny = 50
    xmin = 0.0
    ymin = 0.0
    xmax = 0.04
    ymax = 0.4
  []
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
  [rho_edge_pos_1]
    initial_condition = 1.e6
  []
  [rho_edge_neg_1]
    initial_condition = 1.e6
  []
  [rho_edge_pos_2]
    initial_condition = 1.e6
  []
  [rho_edge_neg_2]
    initial_condition = 1.e6
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
  [./exy]
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
[]

[Functions]
  [disp_load]
    type = ParsedFunction
    expression = '0.005*4.0*t'
  []
[]

[Physics/SolidMechanics/QuasiStatic/all]
  strain = FINITE
  add_variables = true
  generate_output = 'stress_xy'
  additional_generate_output = 'strain_xy'
[]

[Kernels]

  [Edeg_Pos_Time_Deri_1]
    type = TimeDerivative
    variable = rho_edge_pos_1
  []
  [Edge_Pos_Flux_1]
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho_edge_pos_1
    upwinding_type = full
      dislo_sign = positive
      slip_sys_index = 0
      dislo_character = edge
  []

  [Edeg_Neg_Time_Deri_1]
    type = TimeDerivative
    variable = rho_edge_neg_1
  []
  [Edge_Neg_Flux_1]
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho_edge_neg_1
    upwinding_type = full
      dislo_sign = negative
      slip_sys_index = 0
      dislo_character = edge
  []

  [Edeg_Pos_Time_Deri_2]
    type = TimeDerivative
    variable = rho_edge_pos_2
  []
  [Edge_Pos_Flux_2]
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho_edge_pos_2
    upwinding_type = full
      dislo_sign = positive
      slip_sys_index = 1
      dislo_character = edge
  []

  [Edeg_Neg_Time_Deri_2]
    type = TimeDerivative
    variable = rho_edge_neg_2
  []
  [Edge_Neg_Flux_2]
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho_edge_neg_2
    upwinding_type = full
      dislo_sign = negative
      slip_sys_index = 1
      dislo_character = edge
  []

[]

[DGKernels]

  [dg_edge_pos_1]
    type = DGAdvectionCoupled
    variable = rho_edge_pos_1
      dislo_character = edge
      dislo_sign = positive
      slip_sys_index = 0
  []

  [dg_edge_neg_1]
    type = DGAdvectionCoupled
    variable = rho_edge_neg_1
      dislo_character = edge
      dislo_sign = negative
      slip_sys_index = 0
  []

  [dg_edge_pos_2]
    type = DGAdvectionCoupled
    variable = rho_edge_pos_2
      dislo_character = edge
      dislo_sign = positive
      slip_sys_index = 1
  []

  [dg_edge_neg_2]
    type = DGAdvectionCoupled
    variable = rho_edge_neg_2
      dislo_character = edge
      dislo_sign = negative
      slip_sys_index = 1
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
  [./exy]
    type = RankTwoAux
    variable = exy
    rank_two_tensor = total_lagrangian_strain
    index_j = 0
    index_i = 1
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
  [./slip_inc]
   type = MaterialStdVectorAux
   variable = slip_increment
   property = slip_increment
   index = 0
   execute_on = timestep_end
  [../]
  [./dislo_vel]
   type = MaterialStdVectorAux
   variable = dislo_velocity
   property = dislo_velocity
   index = 0
   execute_on = timestep_end
  [../]
  [./epeq]
   type = MaterialRealAux
   variable = epeq
   property = accumulated_equivalent_plastic_strain
   execute_on = timestep_end
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeElasticityTensorCP
    C_ijkl = '1.129e5 0.664e5 0.664e5 1.129e5 0.664e5 1.129e5 0.279e5 0.279e5 0.279e5'
    fill_method = symmetric9
    euler_angle_1 = 0.0
    euler_angle_2 = 0.0 
    euler_angle_3 = 0.0 
  [../]
  [./stress]
    type = ComputeCrystalPlasticityDislocationStress
    crystal_plasticity_models = 'trial_xtalpl'
    tan_mod_type = exact
  [../]
  [./trial_xtalpl]
    type = CrystalPlasticityBussoUpdate
    number_slip_systems = 2
    slip_sys_file_name = input_slip_sys_al.txt
      w1 = 0.0
      w2 = 0.0
      tau_0 = 8.0
      p = 0.141
      q = 1.1
      f0 = 3.e-19
      gdot0 = 1.73e6
    edge_dislo_den_pos_1 = rho_edge_pos_1
    edge_dislo_den_neg_1 = rho_edge_neg_1
    edge_dislo_den_pos_2 = rho_edge_pos_2
    edge_dislo_den_neg_2 = rho_edge_neg_2
  [../]
[]

[BCs]
  [bottom_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'bottom'
    value = 0.0
  []
  [bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom'
    value = 0.0
  []

  [top_x]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'top'
    function = disp_load
  []
  [top_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'top'
    value = 0.0
  []
  [./Periodic]
    [./auto_boundary_x]
      variable = disp_x
      auto_direction = 'x'
    [../]

    [./auto_boundary_y]
      variable = disp_y
      auto_direction = 'x'
    [../]
    
    [./auto_rho_edge_pos_1_boundary_x]
      variable = rho_edge_pos_1
      auto_direction = 'x'
    [../]
    
    [./auto_rho_edge_neg_1_boundary_x]
      variable = rho_edge_neg_1
      auto_direction = 'x'
    [../] 

    [./auto_rho_edge_pos_2_boundary_x]
      variable = rho_edge_pos_2
      auto_direction = 'x'
    [../] 

    [./auto_rho_edge_neg_2_boundary_x]
      variable = rho_edge_neg_2
      auto_direction = 'x'
    [../] 
  [../]
  # [./Periodic]

  #   [./auto_boundary_x]
  #     variable = disp_x
  #     primary = 'left'
  #   secondary = 'right'
  #   translation = '0.04 0.0 0.0'
  #   [../]

  #   [./auto_boundary_y]
  #     variable = disp_y
  #     primary = 'left'
  #   secondary = 'right'
  #   translation = '0.04 0.0 0.0'
  #   [../]

  #   [./auto_rho_edge_pos_boundary_x_1]
  #     variable = rho_edge_pos_1
  #     primary = 'left'
  #   secondary = 'right'
  #   translation = '0.04 0.0 0.0'
  #   [../]

  #   [./auto_rho_edge_neg_boundary_x_1]
  #     variable = rho_edge_neg_1
  #     primary = 'left'
  #   secondary = 'right'
  #   translation = '0.04 0.0 0.0'
  #   [../]

  #   [./auto_rho_edge_pos_boundary_x_2]
  #     variable = rho_edge_pos_2
  #     primary = 'left'
  #   secondary = 'right'
  #   translation = '0.04 0.0 0.0'
  #   [../]

  #   [./auto_rho_edge_neg_boundary_x_2]
  #     variable = rho_edge_neg_2
  #     primary = 'left'
  #   secondary = 'right'
  #   translation = '0.04 0.0 0.0'
  #   [../]

  # [../]

[]

[Preconditioning]
  active = 'smp'
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]

  type = Transient

  [./TimeIntegrator]
    # type = ImplicitEuler
    # type = BDF2
    # type = CrankNicolson
    type = ImplicitMidpoint
    # type = LStableDirk2
    # type = LStableDirk3
    # type = LStableDirk4
    # type = AStableDirk4
    #
    # Explicit methods
    # type = ExplicitEuler
    # type = ExplicitMidpoint
    # type = Heun
    # type = Ralston
  [../]

  solve_type = 'PJFNK'
  petsc_options = '-snes_ksp_ew'
  # petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -pc_factor_mat_solver_package'
  # petsc_options_value = 'gmres asm lu superlu_dist'
  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value = 'hypre    boomeramg          31'
  line_search = 'basic'
  automatic_scaling = true

  l_max_its = 50
  nl_max_its = 50
  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-3
  # l_tol = 1e-5

  start_time = 0.0
  end_time = 0.5
  dt = 2.e-6
  dtmin = 1.e-10
  # type = Transient
  # solve_type = 'NEWTON'
  # petsc_options = '-snes_ksp_ew'
  # petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  # petsc_options_value = 'lu    boomeramg          31'
  # line_search = 'none'
  # l_max_its = 50
  # nl_max_its = 50
  # nl_rel_tol = 1e-5
  # nl_abs_tol = 1e-3
  # l_tol = 1e-5

  # start_time = 0.0
  # end_time = 0.5
  # dt = 5.e-6
  # dtmin = 1.e-9
[]

[Postprocessors]
  [./stress_xy]
    type = ElementAverageValue
    variable = stress_xy
  [../]
  [./pk2]
   type = ElementAverageValue
   variable = pk2
  [../]
  [./fp_xx]
    type = ElementAverageValue
    variable = fp_xx
  [../]
  [./exy]
    type = ElementAverageValue
    variable = exy
  [../]
  [./slip_increment]
   type = ElementAverageValue
   variable = slip_increment
  [../]
  [./dislo_velocity]
   type = ElementAverageValue
   variable = dislo_velocity
  [../]
  [./disp_x]
     type = NodalVariableValue
     variable = disp_x
     nodeid = 1
  [../]
  [./strain_xy]
    type = ElementAverageValue
    variable = strain_xy
  [../]
  [./epeq]
    type = ElementAverageValue
    variable = epeq
  [../]
[]

[VectorPostprocessors]
  [rhoep]
    type = LineValueSampler
    variable = rho_edge_pos_1
    start_point = '0.005 0 0'
    end_point = '0.005 0.1 0'
    num_points = 51
    sort_by = y
  []
  [rhoen]
    type = LineValueSampler
    variable = rho_edge_neg_1
    start_point = '0.005 0 0'
    end_point = '0.005 0.1 0'
    num_points = 51
    sort_by = y
  []
[]

[Outputs]
  exodus = true
  interval = 20
  [csv]
    type = CSV
    file_base = dg_test_l400
    execute_on = final
  []
[]
