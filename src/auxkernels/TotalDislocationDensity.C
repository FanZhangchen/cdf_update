//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TotalDislocationDensity.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("cdf_updateApp", TotalDislocationDensity);

InputParameters
TotalDislocationDensity::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  params.addRequiredParam<int>("nss", "Number of slip systems");
  // Add a "coupling paramater" to get a variable from the input file.
  params.addCoupledVar("edge_dislo_den_1_Q1", 0.0, "edge dislocation density in Q1: slip system 1");
  params.addCoupledVar("edge_dislo_den_1_Q2", 0.0, "edge dislocation density in Q2: slip system 1");
  params.addCoupledVar("edge_dislo_den_1_Q3", 0.0, "edge dislocation density in Q3: slip system 1");
  params.addCoupledVar("edge_dislo_den_1_Q4", 0.0, "edge dislocation density in Q4: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q1", 0.0, "screw dislocation density in Q1: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q2", 0.0, "screw dislocation density in Q2: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q3", 0.0, "screw dislocation density in Q3: slip system 1");
  params.addCoupledVar(
      "screw_dislo_den_1_Q4", 0.0, "screw dislocation density in Q4: slip system 1");

  params.addCoupledVar("edge_dislo_den_2_Q1", 0.0, "edge dislocation density in Q1: slip system 2");
  params.addCoupledVar("edge_dislo_den_2_Q2", 0.0, "edge dislocation density in Q2: slip system 2");
  params.addCoupledVar("edge_dislo_den_2_Q3", 0.0, "edge dislocation density in Q3: slip system 2");
  params.addCoupledVar("edge_dislo_den_2_Q4", 0.0, "edge dislocation density in Q4: slip system 2");
  params.addCoupledVar(
      "screw_dislo_den_2_Q1", 0.0, "screw dislocation density in Q1: slip system 2");
  params.addCoupledVar(
      "screw_dislo_den_2_Q2", 0.0, "screw dislocation density in Q2: slip system 2");
  params.addCoupledVar(
      "screw_dislo_den_2_Q3", 0.0, "screw dislocation density in Q3: slip system 2");
  params.addCoupledVar(
      "screw_dislo_den_2_Q4", 0.0, "screw dislocation density in Q4: slip system 2");

  params.addCoupledVar("edge_dislo_den_3_Q1", 0.0, "edge dislocation density in Q1: slip system 3");
  params.addCoupledVar("edge_dislo_den_3_Q2", 0.0, "edge dislocation density in Q2: slip system 3");
  params.addCoupledVar("edge_dislo_den_3_Q3", 0.0, "edge dislocation density in Q3: slip system 3");
  params.addCoupledVar("edge_dislo_den_3_Q4", 0.0, "edge dislocation density in Q4: slip system 3");
  params.addCoupledVar(
      "screw_dislo_den_3_Q1", 0.0, "screw dislocation density in Q1: slip system 3");
  params.addCoupledVar(
      "screw_dislo_den_3_Q2", 0.0, "screw dislocation density in Q2: slip system 3");
  params.addCoupledVar(
      "screw_dislo_den_3_Q3", 0.0, "screw dislocation density in Q3: slip system 3");
  params.addCoupledVar(
      "screw_dislo_den_3_Q4", 0.0, "screw dislocation density in Q4: slip system 3");

  params.addCoupledVar("edge_dislo_den_4_Q1", 0.0, "edge dislocation density in Q1: slip system 4");
  params.addCoupledVar("edge_dislo_den_4_Q2", 0.0, "edge dislocation density in Q2: slip system 4");
  params.addCoupledVar("edge_dislo_den_4_Q3", 0.0, "edge dislocation density in Q3: slip system 4");
  params.addCoupledVar("edge_dislo_den_4_Q4", 0.0, "edge dislocation density in Q4: slip system 4");
  params.addCoupledVar(
      "screw_dislo_den_4_Q1", 0.0, "screw dislocation density in Q1: slip system 4");
  params.addCoupledVar(
      "screw_dislo_den_4_Q2", 0.0, "screw dislocation density in Q2: slip system 4");
  params.addCoupledVar(
      "screw_dislo_den_4_Q3", 0.0, "screw dislocation density in Q3: slip system 4");
  params.addCoupledVar(
      "screw_dislo_den_4_Q4", 0.0, "screw dislocation density in Q4: slip system 4");

  params.addCoupledVar("edge_dislo_den_5_Q1", 0.0, "edge dislocation density in Q1: slip system 5");
  params.addCoupledVar("edge_dislo_den_5_Q2", 0.0, "edge dislocation density in Q2: slip system 5");
  params.addCoupledVar("edge_dislo_den_5_Q3", 0.0, "edge dislocation density in Q3: slip system 5");
  params.addCoupledVar("edge_dislo_den_5_Q4", 0.0, "edge dislocation density in Q4: slip system 5");
  params.addCoupledVar(
      "screw_dislo_den_5_Q1", 0.0, "screw dislocation density in Q1: slip system 5");
  params.addCoupledVar(
      "screw_dislo_den_5_Q2", 0.0, "screw dislocation density in Q2: slip system 5");
  params.addCoupledVar(
      "screw_dislo_den_5_Q3", 0.0, "screw dislocation density in Q3: slip system 5");
  params.addCoupledVar(
      "screw_dislo_den_5_Q4", 0.0, "screw dislocation density in Q4: slip system 5");

  params.addCoupledVar("edge_dislo_den_6_Q1", 0.0, "edge dislocation density in Q1: slip system 6");
  params.addCoupledVar("edge_dislo_den_6_Q2", 0.0, "edge dislocation density in Q2: slip system 6");
  params.addCoupledVar("edge_dislo_den_6_Q3", 0.0, "edge dislocation density in Q3: slip system 6");
  params.addCoupledVar("edge_dislo_den_6_Q4", 0.0, "edge dislocation density in Q4: slip system 6");
  params.addCoupledVar(
      "screw_dislo_den_6_Q1", 0.0, "screw dislocation density in Q1: slip system 6");
  params.addCoupledVar(
      "screw_dislo_den_6_Q2", 0.0, "screw dislocation density in Q2: slip system 6");
  params.addCoupledVar(
      "screw_dislo_den_6_Q3", 0.0, "screw dislocation density in Q3: slip system 6");
  params.addCoupledVar(
      "screw_dislo_den_6_Q4", 0.0, "screw dislocation density in Q4: slip system 6");

  params.addCoupledVar("edge_dislo_den_7_Q1", 0.0, "edge dislocation density in Q1: slip system 7");
  params.addCoupledVar("edge_dislo_den_7_Q2", 0.0, "edge dislocation density in Q2: slip system 7");
  params.addCoupledVar("edge_dislo_den_7_Q3", 0.0, "edge dislocation density in Q3: slip system 7");
  params.addCoupledVar("edge_dislo_den_7_Q4", 0.0, "edge dislocation density in Q4: slip system 7");
  params.addCoupledVar(
      "screw_dislo_den_7_Q1", 0.0, "screw dislocation density in Q1: slip system 7");
  params.addCoupledVar(
      "screw_dislo_den_7_Q2", 0.0, "screw dislocation density in Q2: slip system 7");
  params.addCoupledVar(
      "screw_dislo_den_7_Q3", 0.0, "screw dislocation density in Q3: slip system 7");
  params.addCoupledVar(
      "screw_dislo_den_7_Q4", 0.0, "screw dislocation density in Q4: slip system 7");

  params.addCoupledVar("edge_dislo_den_8_Q1", 0.0, "edge dislocation density in Q1: slip system 8");
  params.addCoupledVar("edge_dislo_den_8_Q2", 0.0, "edge dislocation density in Q2: slip system 8");
  params.addCoupledVar("edge_dislo_den_8_Q3", 0.0, "edge dislocation density in Q3: slip system 8");
  params.addCoupledVar("edge_dislo_den_8_Q4", 0.0, "edge dislocation density in Q4: slip system 8");
  params.addCoupledVar(
      "screw_dislo_den_8_Q1", 0.0, "screw dislocation density in Q1: slip system 8");
  params.addCoupledVar(
      "screw_dislo_den_8_Q2", 0.0, "screw dislocation density in Q2: slip system 8");
  params.addCoupledVar(
      "screw_dislo_den_8_Q3", 0.0, "screw dislocation density in Q3: slip system 8");
  params.addCoupledVar(
      "screw_dislo_den_8_Q4", 0.0, "screw dislocation density in Q4: slip system 8");

  params.addCoupledVar("edge_dislo_den_9_Q1", 0.0, "edge dislocation density in Q1: slip system 9");
  params.addCoupledVar("edge_dislo_den_9_Q2", 0.0, "edge dislocation density in Q2: slip system 9");
  params.addCoupledVar("edge_dislo_den_9_Q3", 0.0, "edge dislocation density in Q3: slip system 9");
  params.addCoupledVar("edge_dislo_den_9_Q4", 0.0, "edge dislocation density in Q4: slip system 9");
  params.addCoupledVar(
      "screw_dislo_den_9_Q1", 0.0, "screw dislocation density in Q1: slip system 9");
  params.addCoupledVar(
      "screw_dislo_den_9_Q2", 0.0, "screw dislocation density in Q2: slip system 9");
  params.addCoupledVar(
      "screw_dislo_den_9_Q3", 0.0, "screw dislocation density in Q3: slip system 9");
  params.addCoupledVar(
      "screw_dislo_den_9_Q4", 0.0, "screw dislocation density in Q4: slip system 9");

  params.addCoupledVar(
      "edge_dislo_den_10_Q1", 0.0, "edge dislocation density in Q1: slip system 10");
  params.addCoupledVar(
      "edge_dislo_den_10_Q2", 0.0, "edge dislocation density in Q2: slip system 10");
  params.addCoupledVar(
      "edge_dislo_den_10_Q3", 0.0, "edge dislocation density in Q3: slip system 10");
  params.addCoupledVar(
      "edge_dislo_den_10_Q4", 0.0, "edge dislocation density in Q4: slip system 10");
  params.addCoupledVar(
      "screw_dislo_den_10_Q1", 0.0, "screw dislocation density in Q1: slip system 10");
  params.addCoupledVar(
      "screw_dislo_den_10_Q2", 0.0, "screw dislocation density in Q2: slip system 10");
  params.addCoupledVar(
      "screw_dislo_den_10_Q3", 0.0, "screw dislocation density in Q3: slip system 10");
  params.addCoupledVar(
      "screw_dislo_den_10_Q4", 0.0, "screw dislocation density in Q4: slip system 10");

  params.addCoupledVar(
      "edge_dislo_den_11_Q1", 0.0, "edge dislocation density in Q1: slip system 11");
  params.addCoupledVar(
      "edge_dislo_den_11_Q2", 0.0, "edge dislocation density in Q2: slip system 11");
  params.addCoupledVar(
      "edge_dislo_den_11_Q3", 0.0, "edge dislocation density in Q3: slip system 11");
  params.addCoupledVar(
      "edge_dislo_den_11_Q4", 0.0, "edge dislocation density in Q4: slip system 11");
  params.addCoupledVar(
      "screw_dislo_den_11_Q1", 0.0, "screw dislocation density in Q1: slip system 11");
  params.addCoupledVar(
      "screw_dislo_den_11_Q2", 0.0, "screw dislocation density in Q2: slip system 11");
  params.addCoupledVar(
      "screw_dislo_den_11_Q3", 0.0, "screw dislocation density in Q3: slip system 11");
  params.addCoupledVar(
      "screw_dislo_den_11_Q4", 0.0, "screw dislocation density in Q4: slip system 11");

  params.addCoupledVar(
      "edge_dislo_den_12_Q1", 0.0, "edge dislocation density in Q1: slip system 12");
  params.addCoupledVar(
      "edge_dislo_den_12_Q2", 0.0, "edge dislocation density in Q2: slip system 12");
  params.addCoupledVar(
      "edge_dislo_den_12_Q3", 0.0, "edge dislocation density in Q3: slip system 12");
  params.addCoupledVar(
      "edge_dislo_den_12_Q4", 0.0, "edge dislocation density in Q4: slip system 12");
  params.addCoupledVar(
      "screw_dislo_den_12_Q1", 0.0, "screw dislocation density in Q1: slip system 12");
  params.addCoupledVar(
      "screw_dislo_den_12_Q2", 0.0, "screw dislocation density in Q2: slip system 12");
  params.addCoupledVar(
      "screw_dislo_den_12_Q3", 0.0, "screw dislocation density in Q3: slip system 12");
  params.addCoupledVar(
      "screw_dislo_den_12_Q4", 0.0, "screw dislocation density in Q4: slip system 12");

  params.addParam<Real>("initial_total_dislo_den", 0.0, "Scale parameters");

  return params;
}

TotalDislocationDensity::TotalDislocationDensity(const InputParameters & parameters)
  : VectorAuxKernel(parameters),

    _nss(getParam<int>("nss")),

    _gssT(_nss),

    // slip 1
    _edge_dislo_den_1_Q1(coupledValue("edge_dislo_den_1_Q1")),
    _grad_edge_dislo_den_1_Q1(coupledGradient("edge_dislo_den_1_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_1_Q2(coupledValue("edge_dislo_den_1_Q2")),
    _grad_edge_dislo_den_1_Q2(coupledGradient("edge_dislo_den_1_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_1_Q3(coupledValue("edge_dislo_den_1_Q3")),
    _grad_edge_dislo_den_1_Q3(coupledGradient("edge_dislo_den_1_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_1_Q4(coupledValue("edge_dislo_den_1_Q4")),
    _grad_edge_dislo_den_1_Q4(coupledGradient("edge_dislo_den_1_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_1_Q1(coupledValue("screw_dislo_den_1_Q1")),
    _grad_screw_dislo_den_1_Q1(coupledGradient("screw_dislo_den_1_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_1_Q2(coupledValue("screw_dislo_den_1_Q2")),
    _grad_screw_dislo_den_1_Q2(coupledGradient("screw_dislo_den_1_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_1_Q3(coupledValue("screw_dislo_den_1_Q3")),
    _grad_screw_dislo_den_1_Q3(coupledGradient("screw_dislo_den_1_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_1_Q4(coupledValue("screw_dislo_den_1_Q4")),
    _grad_screw_dislo_den_1_Q4(coupledGradient("screw_dislo_den_1_Q4")), // Coupled rhoen gradient

    // slip 2
    _edge_dislo_den_2_Q1(coupledValue("edge_dislo_den_2_Q1")),
    _grad_edge_dislo_den_2_Q1(coupledGradient("edge_dislo_den_2_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_2_Q2(coupledValue("edge_dislo_den_2_Q2")),
    _grad_edge_dislo_den_2_Q2(coupledGradient("edge_dislo_den_2_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_2_Q3(coupledValue("edge_dislo_den_2_Q3")),
    _grad_edge_dislo_den_2_Q3(coupledGradient("edge_dislo_den_2_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_2_Q4(coupledValue("edge_dislo_den_2_Q4")),
    _grad_edge_dislo_den_2_Q4(coupledGradient("edge_dislo_den_2_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_2_Q1(coupledValue("screw_dislo_den_2_Q1")),
    _grad_screw_dislo_den_2_Q1(coupledGradient("screw_dislo_den_2_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_2_Q2(coupledValue("screw_dislo_den_2_Q2")),
    _grad_screw_dislo_den_2_Q2(coupledGradient("screw_dislo_den_2_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_2_Q3(coupledValue("screw_dislo_den_2_Q3")),
    _grad_screw_dislo_den_2_Q3(coupledGradient("screw_dislo_den_2_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_2_Q4(coupledValue("screw_dislo_den_2_Q4")),
    _grad_screw_dislo_den_2_Q4(coupledGradient("screw_dislo_den_2_Q4")), // Coupled rhoen gradient

    // slip 3
    _edge_dislo_den_3_Q1(coupledValue("edge_dislo_den_3_Q1")),
    _grad_edge_dislo_den_3_Q1(coupledGradient("edge_dislo_den_3_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_3_Q2(coupledValue("edge_dislo_den_3_Q2")),
    _grad_edge_dislo_den_3_Q2(coupledGradient("edge_dislo_den_3_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_3_Q3(coupledValue("edge_dislo_den_3_Q3")),
    _grad_edge_dislo_den_3_Q3(coupledGradient("edge_dislo_den_3_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_3_Q4(coupledValue("edge_dislo_den_3_Q4")),
    _grad_edge_dislo_den_3_Q4(coupledGradient("edge_dislo_den_3_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_3_Q1(coupledValue("screw_dislo_den_3_Q1")),
    _grad_screw_dislo_den_3_Q1(coupledGradient("screw_dislo_den_3_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_3_Q2(coupledValue("screw_dislo_den_3_Q2")),
    _grad_screw_dislo_den_3_Q2(coupledGradient("screw_dislo_den_3_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_3_Q3(coupledValue("screw_dislo_den_3_Q3")),
    _grad_screw_dislo_den_3_Q3(coupledGradient("screw_dislo_den_3_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_3_Q4(coupledValue("screw_dislo_den_3_Q4")),
    _grad_screw_dislo_den_3_Q4(coupledGradient("screw_dislo_den_3_Q4")), // Coupled rhoen gradient

    // slip 4
    _edge_dislo_den_4_Q1(coupledValue("edge_dislo_den_4_Q1")),
    _grad_edge_dislo_den_4_Q1(coupledGradient("edge_dislo_den_4_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_4_Q2(coupledValue("edge_dislo_den_4_Q2")),
    _grad_edge_dislo_den_4_Q2(coupledGradient("edge_dislo_den_4_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_4_Q3(coupledValue("edge_dislo_den_4_Q3")),
    _grad_edge_dislo_den_4_Q3(coupledGradient("edge_dislo_den_4_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_4_Q4(coupledValue("edge_dislo_den_4_Q4")),
    _grad_edge_dislo_den_4_Q4(coupledGradient("edge_dislo_den_4_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_4_Q1(coupledValue("screw_dislo_den_4_Q1")),
    _grad_screw_dislo_den_4_Q1(coupledGradient("screw_dislo_den_4_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_4_Q2(coupledValue("screw_dislo_den_4_Q2")),
    _grad_screw_dislo_den_4_Q2(coupledGradient("screw_dislo_den_4_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_4_Q3(coupledValue("screw_dislo_den_4_Q3")),
    _grad_screw_dislo_den_4_Q3(coupledGradient("screw_dislo_den_4_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_4_Q4(coupledValue("screw_dislo_den_4_Q4")),
    _grad_screw_dislo_den_4_Q4(coupledGradient("screw_dislo_den_4_Q4")), // Coupled rhoen gradient

    // slip 5
    _edge_dislo_den_5_Q1(coupledValue("edge_dislo_den_5_Q1")),
    _grad_edge_dislo_den_5_Q1(coupledGradient("edge_dislo_den_5_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_5_Q2(coupledValue("edge_dislo_den_5_Q2")),
    _grad_edge_dislo_den_5_Q2(coupledGradient("edge_dislo_den_5_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_5_Q3(coupledValue("edge_dislo_den_5_Q3")),
    _grad_edge_dislo_den_5_Q3(coupledGradient("edge_dislo_den_5_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_5_Q4(coupledValue("edge_dislo_den_5_Q4")),
    _grad_edge_dislo_den_5_Q4(coupledGradient("edge_dislo_den_5_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_5_Q1(coupledValue("screw_dislo_den_5_Q1")),
    _grad_screw_dislo_den_5_Q1(coupledGradient("screw_dislo_den_5_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_5_Q2(coupledValue("screw_dislo_den_5_Q2")),
    _grad_screw_dislo_den_5_Q2(coupledGradient("screw_dislo_den_5_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_5_Q3(coupledValue("screw_dislo_den_5_Q3")),
    _grad_screw_dislo_den_5_Q3(coupledGradient("screw_dislo_den_5_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_5_Q4(coupledValue("screw_dislo_den_5_Q4")),
    _grad_screw_dislo_den_5_Q4(coupledGradient("screw_dislo_den_5_Q4")), // Coupled rhoen gradient

    // slip 6
    _edge_dislo_den_6_Q1(coupledValue("edge_dislo_den_6_Q1")),
    _grad_edge_dislo_den_6_Q1(coupledGradient("edge_dislo_den_6_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_6_Q2(coupledValue("edge_dislo_den_6_Q2")),
    _grad_edge_dislo_den_6_Q2(coupledGradient("edge_dislo_den_6_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_6_Q3(coupledValue("edge_dislo_den_6_Q3")),
    _grad_edge_dislo_den_6_Q3(coupledGradient("edge_dislo_den_6_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_6_Q4(coupledValue("edge_dislo_den_6_Q4")),
    _grad_edge_dislo_den_6_Q4(coupledGradient("edge_dislo_den_6_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_6_Q1(coupledValue("screw_dislo_den_6_Q1")),
    _grad_screw_dislo_den_6_Q1(coupledGradient("screw_dislo_den_6_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_6_Q2(coupledValue("screw_dislo_den_6_Q2")),
    _grad_screw_dislo_den_6_Q2(coupledGradient("screw_dislo_den_6_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_6_Q3(coupledValue("screw_dislo_den_6_Q3")),
    _grad_screw_dislo_den_6_Q3(coupledGradient("screw_dislo_den_6_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_6_Q4(coupledValue("screw_dislo_den_6_Q4")),
    _grad_screw_dislo_den_6_Q4(coupledGradient("screw_dislo_den_6_Q4")), // Coupled rhoen gradient

    // slip 7
    _edge_dislo_den_7_Q1(coupledValue("edge_dislo_den_7_Q1")),
    _grad_edge_dislo_den_7_Q1(coupledGradient("edge_dislo_den_7_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_7_Q2(coupledValue("edge_dislo_den_7_Q2")),
    _grad_edge_dislo_den_7_Q2(coupledGradient("edge_dislo_den_7_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_7_Q3(coupledValue("edge_dislo_den_7_Q3")),
    _grad_edge_dislo_den_7_Q3(coupledGradient("edge_dislo_den_7_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_7_Q4(coupledValue("edge_dislo_den_7_Q4")),
    _grad_edge_dislo_den_7_Q4(coupledGradient("edge_dislo_den_7_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_7_Q1(coupledValue("screw_dislo_den_7_Q1")),
    _grad_screw_dislo_den_7_Q1(coupledGradient("screw_dislo_den_7_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_7_Q2(coupledValue("screw_dislo_den_7_Q2")),
    _grad_screw_dislo_den_7_Q2(coupledGradient("screw_dislo_den_7_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_7_Q3(coupledValue("screw_dislo_den_7_Q3")),
    _grad_screw_dislo_den_7_Q3(coupledGradient("screw_dislo_den_7_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_7_Q4(coupledValue("screw_dislo_den_7_Q4")),
    _grad_screw_dislo_den_7_Q4(coupledGradient("screw_dislo_den_7_Q4")), // Coupled rhoen gradient

    // slip 8
    _edge_dislo_den_8_Q1(coupledValue("edge_dislo_den_8_Q1")),
    _grad_edge_dislo_den_8_Q1(coupledGradient("edge_dislo_den_8_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_8_Q2(coupledValue("edge_dislo_den_8_Q2")),
    _grad_edge_dislo_den_8_Q2(coupledGradient("edge_dislo_den_8_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_8_Q3(coupledValue("edge_dislo_den_8_Q3")),
    _grad_edge_dislo_den_8_Q3(coupledGradient("edge_dislo_den_8_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_8_Q4(coupledValue("edge_dislo_den_8_Q4")),
    _grad_edge_dislo_den_8_Q4(coupledGradient("edge_dislo_den_8_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_8_Q1(coupledValue("screw_dislo_den_8_Q1")),
    _grad_screw_dislo_den_8_Q1(coupledGradient("screw_dislo_den_8_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_8_Q2(coupledValue("screw_dislo_den_8_Q2")),
    _grad_screw_dislo_den_8_Q2(coupledGradient("screw_dislo_den_8_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_8_Q3(coupledValue("screw_dislo_den_8_Q3")),
    _grad_screw_dislo_den_8_Q3(coupledGradient("screw_dislo_den_8_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_8_Q4(coupledValue("screw_dislo_den_8_Q4")),
    _grad_screw_dislo_den_8_Q4(coupledGradient("screw_dislo_den_8_Q4")), // Coupled rhoen gradient

    // slip 9
    _edge_dislo_den_9_Q1(coupledValue("edge_dislo_den_9_Q1")),
    _grad_edge_dislo_den_9_Q1(coupledGradient("edge_dislo_den_9_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_9_Q2(coupledValue("edge_dislo_den_9_Q2")),
    _grad_edge_dislo_den_9_Q2(coupledGradient("edge_dislo_den_9_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_9_Q3(coupledValue("edge_dislo_den_9_Q3")),
    _grad_edge_dislo_den_9_Q3(coupledGradient("edge_dislo_den_9_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_9_Q4(coupledValue("edge_dislo_den_9_Q4")),
    _grad_edge_dislo_den_9_Q4(coupledGradient("edge_dislo_den_9_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_9_Q1(coupledValue("screw_dislo_den_9_Q1")),
    _grad_screw_dislo_den_9_Q1(coupledGradient("screw_dislo_den_9_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_9_Q2(coupledValue("screw_dislo_den_9_Q2")),
    _grad_screw_dislo_den_9_Q2(coupledGradient("screw_dislo_den_9_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_9_Q3(coupledValue("screw_dislo_den_9_Q3")),
    _grad_screw_dislo_den_9_Q3(coupledGradient("screw_dislo_den_9_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_9_Q4(coupledValue("screw_dislo_den_9_Q4")),
    _grad_screw_dislo_den_9_Q4(coupledGradient("screw_dislo_den_9_Q4")), // Coupled rhoen gradient

    // slip 10
    _edge_dislo_den_10_Q1(coupledValue("edge_dislo_den_10_Q1")),
    _grad_edge_dislo_den_10_Q1(coupledGradient("edge_dislo_den_10_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_10_Q2(coupledValue("edge_dislo_den_10_Q2")),
    _grad_edge_dislo_den_10_Q2(coupledGradient("edge_dislo_den_10_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_10_Q3(coupledValue("edge_dislo_den_10_Q3")),
    _grad_edge_dislo_den_10_Q3(coupledGradient("edge_dislo_den_10_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_10_Q4(coupledValue("edge_dislo_den_10_Q4")),
    _grad_edge_dislo_den_10_Q4(coupledGradient("edge_dislo_den_10_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_10_Q1(coupledValue("screw_dislo_den_10_Q1")),
    _grad_screw_dislo_den_10_Q1(coupledGradient("screw_dislo_den_10_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_10_Q2(coupledValue("screw_dislo_den_10_Q2")),
    _grad_screw_dislo_den_10_Q2(coupledGradient("screw_dislo_den_10_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_10_Q3(coupledValue("screw_dislo_den_10_Q3")),
    _grad_screw_dislo_den_10_Q3(coupledGradient("screw_dislo_den_10_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_10_Q4(coupledValue("screw_dislo_den_10_Q4")),
    _grad_screw_dislo_den_10_Q4(coupledGradient("screw_dislo_den_10_Q4")), // Coupled rhoen gradient

    // slip 11
    _edge_dislo_den_11_Q1(coupledValue("edge_dislo_den_11_Q1")),
    _grad_edge_dislo_den_11_Q1(coupledGradient("edge_dislo_den_11_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_11_Q2(coupledValue("edge_dislo_den_11_Q2")),
    _grad_edge_dislo_den_11_Q2(coupledGradient("edge_dislo_den_11_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_11_Q3(coupledValue("edge_dislo_den_11_Q3")),
    _grad_edge_dislo_den_11_Q3(coupledGradient("edge_dislo_den_11_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_11_Q4(coupledValue("edge_dislo_den_11_Q4")),
    _grad_edge_dislo_den_11_Q4(coupledGradient("edge_dislo_den_11_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_11_Q1(coupledValue("screw_dislo_den_11_Q1")),
    _grad_screw_dislo_den_11_Q1(coupledGradient("screw_dislo_den_11_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_11_Q2(coupledValue("screw_dislo_den_11_Q2")),
    _grad_screw_dislo_den_11_Q2(coupledGradient("screw_dislo_den_11_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_11_Q3(coupledValue("screw_dislo_den_11_Q3")),
    _grad_screw_dislo_den_11_Q3(coupledGradient("screw_dislo_den_11_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_11_Q4(coupledValue("screw_dislo_den_11_Q4")),
    _grad_screw_dislo_den_11_Q4(coupledGradient("screw_dislo_den_11_Q4")), // Coupled rhoen gradient

    // slip 12
    _edge_dislo_den_12_Q1(coupledValue("edge_dislo_den_12_Q1")),
    _grad_edge_dislo_den_12_Q1(coupledGradient("edge_dislo_den_12_Q1")), // Coupled rhoep gradient
    _edge_dislo_den_12_Q2(coupledValue("edge_dislo_den_12_Q2")),
    _grad_edge_dislo_den_12_Q2(coupledGradient("edge_dislo_den_12_Q2")), // Coupled rhoen gradient
    _edge_dislo_den_12_Q3(coupledValue("edge_dislo_den_12_Q3")),
    _grad_edge_dislo_den_12_Q3(coupledGradient("edge_dislo_den_12_Q3")), // Coupled rhoep gradient
    _edge_dislo_den_12_Q4(coupledValue("edge_dislo_den_12_Q4")),
    _grad_edge_dislo_den_12_Q4(coupledGradient("edge_dislo_den_12_Q4")), // Coupled rhoen gradient
    _screw_dislo_den_12_Q1(coupledValue("screw_dislo_den_12_Q1")),
    _grad_screw_dislo_den_12_Q1(coupledGradient("screw_dislo_den_12_Q1")), // Coupled rhoep gradient
    _screw_dislo_den_12_Q2(coupledValue("screw_dislo_den_12_Q2")),
    _grad_screw_dislo_den_12_Q2(coupledGradient("screw_dislo_den_12_Q2")), // Coupled rhoen gradient
    _screw_dislo_den_12_Q3(coupledValue("screw_dislo_den_12_Q3")),
    _grad_screw_dislo_den_12_Q3(coupledGradient("screw_dislo_den_12_Q3")), // Coupled rhoep gradient
    _screw_dislo_den_12_Q4(coupledValue("screw_dislo_den_12_Q4")),
    _grad_screw_dislo_den_12_Q4(coupledGradient("screw_dislo_den_12_Q4")), // Coupled rhoen gradient

    _initial_total_dislo_den(getParam<Real>("initial_total_dislo_den"))

{
}

RealVectorValue
TotalDislocationDensity::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  Real rhot = 0.0;
  Real normalized_rhot = 0.0;

  std::vector<Real> rho_edge_Q1(_nss);
  std::vector<Real> rho_edge_Q2(_nss);
  std::vector<Real> rho_edge_Q3(_nss);
  std::vector<Real> rho_edge_Q4(_nss);
  std::vector<Real> rho_screw_Q1(_nss);
  std::vector<Real> rho_screw_Q2(_nss);
  std::vector<Real> rho_screw_Q3(_nss);
  std::vector<Real> rho_screw_Q4(_nss);

  rho_edge_Q1[0] = _edge_dislo_den_1_Q1[_qp];
  rho_edge_Q1[1] = _edge_dislo_den_2_Q1[_qp];
  rho_edge_Q1[2] = _edge_dislo_den_3_Q1[_qp];
  rho_edge_Q1[3] = _edge_dislo_den_4_Q1[_qp];
  rho_edge_Q1[4] = _edge_dislo_den_5_Q1[_qp];
  rho_edge_Q1[5] = _edge_dislo_den_6_Q1[_qp];
  rho_edge_Q1[6] = _edge_dislo_den_7_Q1[_qp];
  rho_edge_Q1[7] = _edge_dislo_den_8_Q1[_qp];
  rho_edge_Q1[8] = _edge_dislo_den_9_Q1[_qp];
  rho_edge_Q1[9] = _edge_dislo_den_10_Q1[_qp];
  rho_edge_Q1[10] = _edge_dislo_den_11_Q1[_qp];
  rho_edge_Q1[11] = _edge_dislo_den_12_Q1[_qp];

  rho_edge_Q2[0] = _edge_dislo_den_1_Q2[_qp];
  rho_edge_Q2[1] = _edge_dislo_den_2_Q2[_qp];
  rho_edge_Q2[2] = _edge_dislo_den_3_Q2[_qp];
  rho_edge_Q2[3] = _edge_dislo_den_4_Q2[_qp];
  rho_edge_Q2[4] = _edge_dislo_den_5_Q2[_qp];
  rho_edge_Q2[5] = _edge_dislo_den_6_Q2[_qp];
  rho_edge_Q2[6] = _edge_dislo_den_7_Q2[_qp];
  rho_edge_Q2[7] = _edge_dislo_den_8_Q2[_qp];
  rho_edge_Q2[8] = _edge_dislo_den_9_Q2[_qp];
  rho_edge_Q2[9] = _edge_dislo_den_10_Q2[_qp];
  rho_edge_Q2[10] = _edge_dislo_den_11_Q2[_qp];
  rho_edge_Q2[11] = _edge_dislo_den_12_Q2[_qp];

  rho_edge_Q3[0] = _edge_dislo_den_1_Q3[_qp];
  rho_edge_Q3[1] = _edge_dislo_den_2_Q3[_qp];
  rho_edge_Q3[2] = _edge_dislo_den_3_Q3[_qp];
  rho_edge_Q3[3] = _edge_dislo_den_4_Q3[_qp];
  rho_edge_Q3[4] = _edge_dislo_den_5_Q3[_qp];
  rho_edge_Q3[5] = _edge_dislo_den_6_Q3[_qp];
  rho_edge_Q3[6] = _edge_dislo_den_7_Q3[_qp];
  rho_edge_Q3[7] = _edge_dislo_den_8_Q3[_qp];
  rho_edge_Q3[8] = _edge_dislo_den_9_Q3[_qp];
  rho_edge_Q3[9] = _edge_dislo_den_10_Q3[_qp];
  rho_edge_Q3[10] = _edge_dislo_den_11_Q3[_qp];
  rho_edge_Q3[11] = _edge_dislo_den_12_Q3[_qp];

  rho_edge_Q4[0] = _edge_dislo_den_1_Q4[_qp];
  rho_edge_Q4[1] = _edge_dislo_den_2_Q4[_qp];
  rho_edge_Q4[2] = _edge_dislo_den_3_Q4[_qp];
  rho_edge_Q4[3] = _edge_dislo_den_4_Q4[_qp];
  rho_edge_Q4[4] = _edge_dislo_den_5_Q4[_qp];
  rho_edge_Q4[5] = _edge_dislo_den_6_Q4[_qp];
  rho_edge_Q4[6] = _edge_dislo_den_7_Q4[_qp];
  rho_edge_Q4[7] = _edge_dislo_den_8_Q4[_qp];
  rho_edge_Q4[8] = _edge_dislo_den_9_Q4[_qp];
  rho_edge_Q4[9] = _edge_dislo_den_10_Q4[_qp];
  rho_edge_Q4[10] = _edge_dislo_den_11_Q4[_qp];
  rho_edge_Q4[11] = _edge_dislo_den_12_Q4[_qp];

  rho_screw_Q1[0] = _screw_dislo_den_1_Q1[_qp];
  rho_screw_Q1[1] = _screw_dislo_den_2_Q1[_qp];
  rho_screw_Q1[2] = _screw_dislo_den_3_Q1[_qp];
  rho_screw_Q1[3] = _screw_dislo_den_4_Q1[_qp];
  rho_screw_Q1[4] = _screw_dislo_den_5_Q1[_qp];
  rho_screw_Q1[5] = _screw_dislo_den_6_Q1[_qp];
  rho_screw_Q1[6] = _screw_dislo_den_7_Q1[_qp];
  rho_screw_Q1[7] = _screw_dislo_den_8_Q1[_qp];
  rho_screw_Q1[8] = _screw_dislo_den_9_Q1[_qp];
  rho_screw_Q1[9] = _screw_dislo_den_10_Q1[_qp];
  rho_screw_Q1[10] = _screw_dislo_den_11_Q1[_qp];
  rho_screw_Q1[11] = _screw_dislo_den_12_Q1[_qp];

  rho_screw_Q2[0] = _screw_dislo_den_1_Q2[_qp];
  rho_screw_Q2[1] = _screw_dislo_den_2_Q2[_qp];
  rho_screw_Q2[2] = _screw_dislo_den_3_Q2[_qp];
  rho_screw_Q2[3] = _screw_dislo_den_4_Q2[_qp];
  rho_screw_Q2[4] = _screw_dislo_den_5_Q2[_qp];
  rho_screw_Q2[5] = _screw_dislo_den_6_Q2[_qp];
  rho_screw_Q2[6] = _screw_dislo_den_7_Q2[_qp];
  rho_screw_Q2[7] = _screw_dislo_den_8_Q2[_qp];
  rho_screw_Q2[8] = _screw_dislo_den_9_Q2[_qp];
  rho_screw_Q2[9] = _screw_dislo_den_10_Q2[_qp];
  rho_screw_Q2[10] = _screw_dislo_den_11_Q2[_qp];
  rho_screw_Q2[11] = _screw_dislo_den_12_Q2[_qp];

  rho_screw_Q3[0] = _screw_dislo_den_1_Q3[_qp];
  rho_screw_Q3[1] = _screw_dislo_den_2_Q3[_qp];
  rho_screw_Q3[2] = _screw_dislo_den_3_Q3[_qp];
  rho_screw_Q3[3] = _screw_dislo_den_4_Q3[_qp];
  rho_screw_Q3[4] = _screw_dislo_den_5_Q3[_qp];
  rho_screw_Q3[5] = _screw_dislo_den_6_Q3[_qp];
  rho_screw_Q3[6] = _screw_dislo_den_7_Q3[_qp];
  rho_screw_Q3[7] = _screw_dislo_den_8_Q3[_qp];
  rho_screw_Q3[8] = _screw_dislo_den_9_Q3[_qp];
  rho_screw_Q3[9] = _screw_dislo_den_10_Q3[_qp];
  rho_screw_Q3[10] = _screw_dislo_den_11_Q3[_qp];
  rho_screw_Q3[11] = _screw_dislo_den_12_Q3[_qp];

  rho_screw_Q4[0] = _screw_dislo_den_1_Q4[_qp];
  rho_screw_Q4[1] = _screw_dislo_den_2_Q4[_qp];
  rho_screw_Q4[2] = _screw_dislo_den_3_Q4[_qp];
  rho_screw_Q4[3] = _screw_dislo_den_4_Q4[_qp];
  rho_screw_Q4[4] = _screw_dislo_den_5_Q4[_qp];
  rho_screw_Q4[5] = _screw_dislo_den_6_Q4[_qp];
  rho_screw_Q4[6] = _screw_dislo_den_7_Q4[_qp];
  rho_screw_Q4[7] = _screw_dislo_den_8_Q4[_qp];
  rho_screw_Q4[8] = _screw_dislo_den_9_Q4[_qp];
  rho_screw_Q4[9] = _screw_dislo_den_10_Q4[_qp];
  rho_screw_Q4[10] = _screw_dislo_den_11_Q4[_qp];
  rho_screw_Q4[11] = _screw_dislo_den_12_Q4[_qp];

  for (const auto i : make_range(_nss))
  {
    rhot += rho_edge_Q1[i] + rho_edge_Q2[i] + rho_edge_Q3[i] + rho_edge_Q4[i] + rho_screw_Q1[i] +
            rho_screw_Q2[i] + rho_screw_Q3[i] + rho_screw_Q4[i];
  }

  normalized_rhot = rhot / _initial_total_dislo_den;

  return normalized_rhot;
}
