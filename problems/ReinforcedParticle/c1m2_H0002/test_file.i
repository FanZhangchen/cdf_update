[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [read]
    type = FileMeshGenerator
    file = c1m2_H0002.inp
  []
  # [rename]
  #   type = RenameBlockGenerator
  #   input = read
  #   old_block = 'block1_quad4 block2_quad4 block3_quad4 block4_quad4 block5_quad4 block6_quad4'
  #   new_block = '0 1 1 1 1 1'
  # []
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
    initial_condition = 5.e7
    block = '0'
  []
  [rho_edge_neg_1]
    initial_condition = 5.e7
    block = '0'
  []
[]

[AuxVariables]
  [./pk2]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
  [./fp_xy]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
  [./exy]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
  [./slip_increment]
   order = CONSTANT
   family = MONOMIAL
   block = '0'
  [../]
  [./dislo_velocity]
   order = CONSTANT
   family = MONOMIAL
   block = '0'
  [../]
  [./epeq]
   order = CONSTANT
   family = MONOMIAL
   block = '0'
  [../]
[]

[Functions]
  [disp_load]
    type = PiecewiseLinear
    x = '0.0 1.92  3.84'
    y = '0.0 0.0000192 0.0'
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
    type = MassLumpedTimeDerivative
    variable = rho_edge_pos_1
  []
  [Edge_Pos_Flux_1]
    implicit = false
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho_edge_pos_1
    upwinding_type = full
      dislo_sign = positive
      slip_sys_index = 0
      dislo_character = edge
  []

  [Edeg_Neg_Time_Deri_1]
    type = MassLumpedTimeDerivative
    variable = rho_edge_neg_1
  []
  [Edge_Neg_Flux_1]
    implicit = false
    type = ConservativeAdvectionSchmidNoSSD
    variable = rho_edge_neg_1
    upwinding_type = full
      dislo_sign = negative
      slip_sys_index = 0
      dislo_character = edge
  []

[]

[DGKernels]

  [dg_edge_pos_1]
    implicit = false
    type = DGAdvectionCoupled
    variable = rho_edge_pos_1
      dislo_character = edge
      dislo_sign = positive
      slip_sys_index = 0
  []

  [dg_edge_neg_1]
    implicit = false
    type = DGAdvectionCoupled
    variable = rho_edge_neg_1
      dislo_character = edge
      dislo_sign = negative
      slip_sys_index = 0
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
   block = '0'
  [../]
  [./exy]
    type = RankTwoAux
    variable = exy
    rank_two_tensor = total_lagrangian_strain
    index_j = 0
    index_i = 1
    execute_on = timestep_end
    block = '0'
  [../]
  [./fp_xy]
    type = RankTwoAux
    variable = fp_xy
    rank_two_tensor = plastic_deformation_gradient
    index_j = 0
    index_i = 1
    execute_on = timestep_end
    block = '0'
  [../]
  [./slip_inc]
   type = MaterialStdVectorAux
   variable = slip_increment
   property = slip_increment
   index = 0
   execute_on = timestep_end
   block = '0'
  [../]
  [./dislo_vel]
   type = MaterialStdVectorAux
   variable = dislo_velocity
   property = dislo_velocity
   index = 0
   execute_on = timestep_end
   block = '0'
  [../]
  [./epeq]
   type = MaterialRealAux
   variable = epeq
   property = accumulated_equivalent_plastic_strain
   execute_on = timestep_end
   block = '0'
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
    block = '0'
  [../]
  [./stress]
    type = ComputeCrystalPlasticityDislocationStress
    crystal_plasticity_models = 'trial_xtalpl'
    tan_mod_type = exact
    block = '0'
  [../]
  [./trial_xtalpl]
    type = CrystalPlasticityBussoUpdate
    number_slip_systems = 1
    slip_sys_file_name = input_slip_sys_al.txt
      w1 = 0.0
      w2 = 0.0
      tau_0 = 8.0
      p = 0.141
      q = 1.1
      f0 = 3.e-19
      gdot0 = 1.73e6
      scaling_Cb = 0.241
    edge_dislo_den_pos_1 = rho_edge_pos_1
    edge_dislo_den_neg_1 = rho_edge_neg_1
    block = '0'
  [../]
  #new
  [./elasticity_tensor_hard]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 192.0e3
    poissons_ratio = 0.17
    block = '1'
  [../]
  [./stress_elastic]
    type = ComputeFiniteStrainElasticStress
    block = '1'
  [../]

[]

[BCs]
  [bottom_x]
    type = DirichletBC
    variable = disp_x
    boundary = '2'
    value = 0.0
  []
  [bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = '2'
    value = 0.0
  []

  [top_x]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = '6'
    function = disp_load
  []
  [top_y]
    type = DirichletBC
    variable = disp_y
    boundary = '6'
    value = 0.0
  []

  [./Periodic]
    [./auto_boundary_x]
      variable = disp_x
      primary = '4'
    secondary = '5'
    translation = '0.00346400007 0.0 0.0'
    [../]

    [./auto_boundary_y]
      variable = disp_y
      primary = '4'
    secondary = '5'
    translation = '0.00346400007 0.0 0.0'
    [../]

    [./auto_rho_edge_pos_boundary_x_1]
      variable = rho_edge_pos_1
      primary = '4'
    secondary = '5'
    translation = '0.00346400007 0.0 0.0'
    [../]

    [./auto_rho_edge_neg_boundary_x_1]
      variable = rho_edge_neg_1
      primary = '4'
    secondary = '5'
    translation = '0.00346400007 0.0 0.0'
    [../]
  [../]

[]

[Preconditioning]
  active = 'smp'
  [./smp]
    type = SMP
    full = true
  [../]
[]

# Transient (time-dependent) details for simulations go here:
[Executioner]

  type = Transient

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
  
  solve_type = 'NEWTON'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'
  #solve_type = 'crank-nicolson'
  # petsc_options = '-snes_ksp_ew'
  # petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  # petsc_options_value = 'lu    boomeramg          31'
  line_search = 'none'
  l_max_its = 50
  nl_max_its = 50
  nl_rel_tol = 1e-5 #1e-5
  nl_abs_tol = 1e-3
  l_tol = 1e-5  #1e-5 

  start_time = 0.0
  end_time = 3.84 #0.01
  dt = 1.e-6
  dtmin = 1.e-9
[]

[Postprocessors]
  [./stress_xy]
    type = ElementAverageValue
    variable = stress_xy
    block = '0 1'
  [../]
  [./pk2]
   type = ElementAverageValue
   variable = pk2
   block = '0'
  [../]
  [./fp_xy]
    type = ElementAverageValue
    variable = fp_xy
    block = '0'
  [../]
  [./exy]
    type = ElementAverageValue
    variable = exy
    block = '0'
  [../]
  [./slip_increment]
   type = ElementAverageValue
   variable = slip_increment
   block = '0'
  [../]
  [./dislo_velocity]
   type = ElementAverageValue
   variable = dislo_velocity
   block = '0'
  [../]
  [./disp_x]
     type = NodalVariableValue
     variable = disp_x
     nodeid = 101
  [../]
  [./strain_xy]
    type = ElementAverageValue
    variable = strain_xy
    block = '0 1'
  [../]
  [./epeq]
    type = ElementAverageValue
    variable = epeq
    block = '0'
  [../]
[]

[VectorPostprocessors]
[]

[Outputs]
  exodus = true
  time_step_interval = 500
  [csv]
    type = CSV
    file_base = dg_rhoe_x_out_h5
    execute_on = final
  []
[]
