[Mesh]
  [read]
    type = FileMeshGenerator
    file = c1m1_H0002f2.inp
  []
  # [mesh]
  #   type = CartesianMeshGenerator
  #   dim = 2
  #   dx = '1 1 1'
  #   dy = '1 1 1'
  #   subdomain_id = '7 6 5
  #                   8 0 4
  #                   1 2 3'
  # []
  # [gen1]
  #   type = GeneratedMeshGenerator
  #   dim = 2
  #   nx = 5
  #   ny = 10
  #   xmin = 0.00719
  #   ymin = 0.00206
  #   xmax = 0.01013
  #   ymax = 0.00794
  # []
  # [gen2]
  #   type = GeneratedMeshGenerator
  #   dim = 2
  #   nx = 2
  #   ny = 3
  #   xmin = 0
  #   ymin = 0.00706
  #   xmax = 0.00147
  #   ymax = 0.01
  # []
  # [gen3]
  #   type = GeneratedMeshGenerator
  #   dim = 2
  #   nx = 2
  #   ny = 3
  #   xmin = 0
  #   ymin = 0
  #   xmax = 0.00147
  #   ymax = 0.00294
  # []
  # [gen4]
  #   type = GeneratedMeshGenerator
  #   dim = 2
  #   nx = 2
  #   ny = 3
  #   xmin = 0.01585
  #   ymin = 0.00706
  #   xmax = 0.01732
  #   ymax = 0.01
  # []
  # [gen5]
  #   type = GeneratedMeshGenerator
  #   dim = 2
  #   nx = 2
  #   ny = 3
  #   xmin = 0.01585
  #   ymin = 0.0
  #   xmax = 0.01732
  #   ymax = 0.00294
  # []
  # [cmbn]
  #   type = CombinerGenerator
  #   inputs = 'gen1 gen2 gen3 gen4 gen5'
  # []
  # [main_block]
  #   input = cmbn
  #   type = SubdomainBoundingBoxGenerator
  #   block_id = 1
  #   bottom_left = '0 0 0'
  #   top_right = '0.01732 0.01 0'
  # [../]
[]

[Variables]
  [u]
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = u
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    boundary = 'left'
    value = 0
  []
  [right]
    type = DirichletBC
    variable = u
    boundary = 'right'
    value = 1
  []
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
