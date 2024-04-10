#include "SSDUpdate.h"

registerMooseObject("cdf_updateApp", SSDUpdate);

InputParameters
SSDUpdate::validParams()
{
  InputParameters params = Material::validParams();

  // Parameter for radius of the spheres used to interpolate permeability.
  params.addClassDescription("Compute the evolution equation of SSD density");
  params.addParam<Real>("burgers", 2.54e-7, "magnitude of burgers vector");
  params.addParam<Real>("ke_b", 52000.0, "edge hardening constants");
  params.addParam<Real>("ks_b", 104000.0, "screw hardening constants");
  params.addParam<Real>("de", 0.8e-6, "critical edge annihilation distance");
  params.addParam<Real>("ds", 4.0e-6, "critical screw annihilation distance");
  params.addParam<Real>("Ce", 0.5, "edge proportional constant");
  params.addParam<Real>("Cs", 0.5, "screw proportional constant");

  params.addRequiredParam<int>("nss", "Number of slip systems");
  // Coupled Variables of Slip 1
  params.addCoupledVar("edge_dislo_den_1_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_1_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_1_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_1_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_1_Q1", 0.0, "screw dislocation density in Q1");
  params.addCoupledVar("screw_dislo_den_1_Q2", 0.0, "screw dislocation density in Q2");
  params.addCoupledVar("screw_dislo_den_1_Q3", 0.0, "screw dislocation density in Q3");
  params.addCoupledVar("screw_dislo_den_1_Q4", 0.0, "screw dislocation density in Q4");
  // Coupled Variables of Slip 2
  params.addCoupledVar("edge_dislo_den_2_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_2_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_2_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_2_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_2_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_2_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_2_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_2_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 3
  params.addCoupledVar("edge_dislo_den_3_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_3_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_3_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_3_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_3_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_3_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_3_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_3_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 4
  params.addCoupledVar("edge_dislo_den_4_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_4_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_4_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_4_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_4_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_4_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_4_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_4_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 5
  params.addCoupledVar("edge_dislo_den_5_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_5_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_5_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_5_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_5_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_5_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_5_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_5_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 6
  params.addCoupledVar("edge_dislo_den_6_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_6_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_6_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_6_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_6_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_6_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_6_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_6_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 7
  params.addCoupledVar("edge_dislo_den_7_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_7_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_7_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_7_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_7_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_7_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_7_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_7_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 8
  params.addCoupledVar("edge_dislo_den_8_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_8_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_8_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_8_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_8_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_8_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_8_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_8_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 9
  params.addCoupledVar("edge_dislo_den_9_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_9_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_9_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_9_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_9_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_9_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_9_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_9_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 10
  params.addCoupledVar("edge_dislo_den_10_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_10_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_10_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_10_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_10_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_10_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_10_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_10_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 11
  params.addCoupledVar("edge_dislo_den_11_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_11_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_11_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_11_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_11_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_11_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_11_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_11_Q4", 0.0, "negative screw dislocation density");
  // Coupled Variables of Slip 12
  params.addCoupledVar("edge_dislo_den_12_Q1", 0.0, "edge dislocation density in Q1");
  params.addCoupledVar("edge_dislo_den_12_Q2", 0.0, "edge dislocation density in Q2");
  params.addCoupledVar("edge_dislo_den_12_Q3", 0.0, "edge dislocation density in Q3");
  params.addCoupledVar("edge_dislo_den_12_Q4", 0.0, "edge dislocation density in Q4");
  params.addCoupledVar("screw_dislo_den_12_Q1", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_12_Q2", 0.0, "negative screw dislocation density");
  params.addCoupledVar("screw_dislo_den_12_Q3", 0.0, "positive screw dislocation density");
  params.addCoupledVar("screw_dislo_den_12_Q4", 0.0, "negative screw dislocation density");

  return params;
}

SSDUpdate::SSDUpdate(const InputParameters & parameters)
  : DerivativeMaterialInterface<Material>(parameters),

    // Get the parameters of the initial condition of SSD density from the input file
    _burgers(getParam<Real>("burgers")),
    _ke_b(getParam<Real>("ke_b")),
    _ks_b(getParam<Real>("ks_b")),
    _de(getParam<Real>("de")),
    _ds(getParam<Real>("ds")),
    _Ce(getParam<Real>("Ce")),
    _Cs(getParam<Real>("Cs")),

    _nss(getParam<int>("nss")),

    _gssT(_nss),

    // Get the coupled value of NDOFs of Slip 1
    _edge_dislo_den_1_Q1(coupledValue("edge_dislo_den_1_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_1_Q2(coupledValue("edge_dislo_den_1_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_1_Q3(coupledValue("edge_dislo_den_1_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_1_Q4(coupledValue("edge_dislo_den_1_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_1_Q1(coupledValue("screw_dislo_den_1_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_1_Q2(coupledValue("screw_dislo_den_1_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_1_Q3(coupledValue("screw_dislo_den_1_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_1_Q4(coupledValue("screw_dislo_den_1_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 2
    _edge_dislo_den_2_Q1(coupledValue("edge_dislo_den_2_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_2_Q2(coupledValue("edge_dislo_den_2_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_2_Q3(coupledValue("edge_dislo_den_2_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_2_Q4(coupledValue("edge_dislo_den_2_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_2_Q1(coupledValue("screw_dislo_den_2_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_2_Q2(coupledValue("screw_dislo_den_2_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_2_Q3(coupledValue("screw_dislo_den_2_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_2_Q4(coupledValue("screw_dislo_den_2_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 3
    _edge_dislo_den_3_Q1(coupledValue("edge_dislo_den_3_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_3_Q2(coupledValue("edge_dislo_den_3_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_3_Q3(coupledValue("edge_dislo_den_3_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_3_Q4(coupledValue("edge_dislo_den_3_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_3_Q1(coupledValue("screw_dislo_den_3_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_3_Q2(coupledValue("screw_dislo_den_3_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_3_Q3(coupledValue("screw_dislo_den_3_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_3_Q4(coupledValue("screw_dislo_den_3_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 4
    _edge_dislo_den_4_Q1(coupledValue("edge_dislo_den_4_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_4_Q2(coupledValue("edge_dislo_den_4_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_4_Q3(coupledValue("edge_dislo_den_4_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_4_Q4(coupledValue("edge_dislo_den_4_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_4_Q1(coupledValue("screw_dislo_den_4_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_4_Q2(coupledValue("screw_dislo_den_4_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_4_Q3(coupledValue("screw_dislo_den_4_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_4_Q4(coupledValue("screw_dislo_den_4_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 5
    _edge_dislo_den_5_Q1(coupledValue("edge_dislo_den_5_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_5_Q2(coupledValue("edge_dislo_den_5_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_5_Q3(coupledValue("edge_dislo_den_5_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_5_Q4(coupledValue("edge_dislo_den_5_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_5_Q1(coupledValue("screw_dislo_den_5_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_5_Q2(coupledValue("screw_dislo_den_5_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_5_Q3(coupledValue("screw_dislo_den_5_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_5_Q4(coupledValue("screw_dislo_den_5_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 6
    _edge_dislo_den_6_Q1(coupledValue("edge_dislo_den_6_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_6_Q2(coupledValue("edge_dislo_den_6_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_6_Q3(coupledValue("edge_dislo_den_6_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_6_Q4(coupledValue("edge_dislo_den_6_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_6_Q1(coupledValue("screw_dislo_den_6_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_6_Q2(coupledValue("screw_dislo_den_6_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_6_Q3(coupledValue("screw_dislo_den_6_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_6_Q4(coupledValue("screw_dislo_den_6_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 7
    _edge_dislo_den_7_Q1(coupledValue("edge_dislo_den_7_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_7_Q2(coupledValue("edge_dislo_den_7_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_7_Q3(coupledValue("edge_dislo_den_7_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_7_Q4(coupledValue("edge_dislo_den_7_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_7_Q1(coupledValue("screw_dislo_den_7_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_7_Q2(coupledValue("screw_dislo_den_7_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_7_Q3(coupledValue("screw_dislo_den_7_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_7_Q4(coupledValue("screw_dislo_den_7_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 8
    _edge_dislo_den_8_Q1(coupledValue("edge_dislo_den_8_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_8_Q2(coupledValue("edge_dislo_den_8_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_8_Q3(coupledValue("edge_dislo_den_8_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_8_Q4(coupledValue("edge_dislo_den_8_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_8_Q1(coupledValue("screw_dislo_den_8_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_8_Q2(coupledValue("screw_dislo_den_8_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_8_Q3(coupledValue("screw_dislo_den_8_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_8_Q4(coupledValue("screw_dislo_den_8_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 9
    _edge_dislo_den_9_Q1(coupledValue("edge_dislo_den_9_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_9_Q2(coupledValue("edge_dislo_den_9_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_9_Q3(coupledValue("edge_dislo_den_9_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_9_Q4(coupledValue("edge_dislo_den_9_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_9_Q1(coupledValue("screw_dislo_den_9_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_9_Q2(coupledValue("screw_dislo_den_9_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_9_Q3(coupledValue("screw_dislo_den_9_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_9_Q4(coupledValue("screw_dislo_den_9_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 10
    _edge_dislo_den_10_Q1(coupledValue("edge_dislo_den_10_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_10_Q2(coupledValue("edge_dislo_den_10_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_10_Q3(coupledValue("edge_dislo_den_10_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_10_Q4(coupledValue("edge_dislo_den_10_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_10_Q1(coupledValue("screw_dislo_den_10_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_10_Q2(coupledValue("screw_dislo_den_10_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_10_Q3(coupledValue("screw_dislo_den_10_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_10_Q4(coupledValue("screw_dislo_den_10_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 11
    _edge_dislo_den_11_Q1(coupledValue("edge_dislo_den_11_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_11_Q2(coupledValue("edge_dislo_den_11_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_11_Q3(coupledValue("edge_dislo_den_11_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_11_Q4(coupledValue("edge_dislo_den_11_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_11_Q1(coupledValue("screw_dislo_den_11_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_11_Q2(coupledValue("screw_dislo_den_11_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_11_Q3(coupledValue("screw_dislo_den_11_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_11_Q4(coupledValue("screw_dislo_den_11_Q4")), // Coupled Slip 1 Screw in Q4

    // Get the coupled value of NDOFs of Slip 12
    _edge_dislo_den_12_Q1(coupledValue("edge_dislo_den_12_Q1")),   // Coupled Slip 1 Edge in Q1
    _edge_dislo_den_12_Q2(coupledValue("edge_dislo_den_12_Q2")),   // Coupled Slip 1 Edge in Q2
    _edge_dislo_den_12_Q3(coupledValue("edge_dislo_den_12_Q3")),   // Coupled Slip 1 Edge in Q3
    _edge_dislo_den_12_Q4(coupledValue("edge_dislo_den_12_Q4")),   // Coupled Slip 1 Edge in Q4
    _screw_dislo_den_12_Q1(coupledValue("screw_dislo_den_12_Q1")), // Coupled Slip 1 Screw in Q1
    _screw_dislo_den_12_Q2(coupledValue("screw_dislo_den_12_Q2")), // Coupled Slip 1 Screw in Q2
    _screw_dislo_den_12_Q3(coupledValue("screw_dislo_den_12_Q3")), // Coupled Slip 1 Screw in Q3
    _screw_dislo_den_12_Q4(coupledValue("screw_dislo_den_12_Q4")), // Coupled Slip 1 Screw in Q4

    _slip_increment(getMaterialProperty<std::vector<Real>>("slip_increment")),
    // Declare the current value and older value of SSD density
    _edge_dislocation_increment(declareProperty<std::vector<Real>>("edge_dislocation_increment")),
    _edge_dislocation_increment_old(
        getMaterialPropertyOld<std::vector<Real>>("edge_dislocation_increment")),

    _screw_dislocation_increment(declareProperty<std::vector<Real>>("screw_dislocation_increment")),
    _screw_dislocation_increment_old(
        getMaterialPropertyOld<std::vector<Real>>("screw_dislocation_increment"))

{
}

void
SSDUpdate::initQpStatefulProperties()
{

  std::vector<Real> edge_dislo_den_Q1(_nss);
  std::vector<Real> edge_dislo_den_Q2(_nss);
  std::vector<Real> edge_dislo_den_Q3(_nss);
  std::vector<Real> edge_dislo_den_Q4(_nss);
  std::vector<Real> screw_dislo_den_Q1(_nss);
  std::vector<Real> screw_dislo_den_Q2(_nss);
  std::vector<Real> screw_dislo_den_Q3(_nss);
  std::vector<Real> screw_dislo_den_Q4(_nss);

  std::vector<Real> total_dislocation_density(_nss);
  std::vector<Real> edge_dislocation_density(_nss);
  std::vector<Real> screw_dislocation_density(_nss);

  _edge_dislocation_increment[_qp].resize(_nss);
  _screw_dislocation_increment[_qp].resize(_nss);

  Real TotalDislocationDensity_ALL;

  edge_dislo_den_Q1[0] = _edge_dislo_den_1_Q1[_qp];
  edge_dislo_den_Q1[1] = _edge_dislo_den_2_Q1[_qp];
  edge_dislo_den_Q1[2] = _edge_dislo_den_3_Q1[_qp];
  edge_dislo_den_Q1[3] = _edge_dislo_den_4_Q1[_qp];
  edge_dislo_den_Q1[4] = _edge_dislo_den_5_Q1[_qp];
  edge_dislo_den_Q1[5] = _edge_dislo_den_6_Q1[_qp];
  edge_dislo_den_Q1[6] = _edge_dislo_den_7_Q1[_qp];
  edge_dislo_den_Q1[7] = _edge_dislo_den_8_Q1[_qp];
  edge_dislo_den_Q1[8] = _edge_dislo_den_9_Q1[_qp];
  edge_dislo_den_Q1[9] = _edge_dislo_den_10_Q1[_qp];
  edge_dislo_den_Q1[10] = _edge_dislo_den_11_Q1[_qp];
  edge_dislo_den_Q1[11] = _edge_dislo_den_12_Q1[_qp];

  edge_dislo_den_Q2[0] = _edge_dislo_den_1_Q2[_qp];
  edge_dislo_den_Q2[1] = _edge_dislo_den_2_Q2[_qp];
  edge_dislo_den_Q2[2] = _edge_dislo_den_3_Q2[_qp];
  edge_dislo_den_Q2[3] = _edge_dislo_den_4_Q2[_qp];
  edge_dislo_den_Q2[4] = _edge_dislo_den_5_Q2[_qp];
  edge_dislo_den_Q2[5] = _edge_dislo_den_6_Q2[_qp];
  edge_dislo_den_Q2[6] = _edge_dislo_den_7_Q2[_qp];
  edge_dislo_den_Q2[7] = _edge_dislo_den_8_Q2[_qp];
  edge_dislo_den_Q2[8] = _edge_dislo_den_9_Q2[_qp];
  edge_dislo_den_Q2[9] = _edge_dislo_den_10_Q2[_qp];
  edge_dislo_den_Q2[10] = _edge_dislo_den_11_Q2[_qp];
  edge_dislo_den_Q2[11] = _edge_dislo_den_12_Q2[_qp];

  edge_dislo_den_Q3[0] = _edge_dislo_den_1_Q3[_qp];
  edge_dislo_den_Q3[1] = _edge_dislo_den_2_Q3[_qp];
  edge_dislo_den_Q3[2] = _edge_dislo_den_3_Q3[_qp];
  edge_dislo_den_Q3[3] = _edge_dislo_den_4_Q3[_qp];
  edge_dislo_den_Q3[4] = _edge_dislo_den_5_Q3[_qp];
  edge_dislo_den_Q3[5] = _edge_dislo_den_6_Q3[_qp];
  edge_dislo_den_Q3[6] = _edge_dislo_den_7_Q3[_qp];
  edge_dislo_den_Q3[7] = _edge_dislo_den_8_Q3[_qp];
  edge_dislo_den_Q3[8] = _edge_dislo_den_9_Q3[_qp];
  edge_dislo_den_Q3[9] = _edge_dislo_den_10_Q3[_qp];
  edge_dislo_den_Q3[10] = _edge_dislo_den_11_Q3[_qp];
  edge_dislo_den_Q3[11] = _edge_dislo_den_12_Q3[_qp];

  edge_dislo_den_Q4[0] = _edge_dislo_den_1_Q4[_qp];
  edge_dislo_den_Q4[1] = _edge_dislo_den_2_Q4[_qp];
  edge_dislo_den_Q4[2] = _edge_dislo_den_3_Q4[_qp];
  edge_dislo_den_Q4[3] = _edge_dislo_den_4_Q4[_qp];
  edge_dislo_den_Q4[4] = _edge_dislo_den_5_Q4[_qp];
  edge_dislo_den_Q4[5] = _edge_dislo_den_6_Q4[_qp];
  edge_dislo_den_Q4[6] = _edge_dislo_den_7_Q4[_qp];
  edge_dislo_den_Q4[7] = _edge_dislo_den_8_Q4[_qp];
  edge_dislo_den_Q4[8] = _edge_dislo_den_9_Q4[_qp];
  edge_dislo_den_Q4[9] = _edge_dislo_den_10_Q4[_qp];
  edge_dislo_den_Q4[10] = _edge_dislo_den_11_Q4[_qp];
  edge_dislo_den_Q4[11] = _edge_dislo_den_12_Q4[_qp];

  screw_dislo_den_Q1[0] = _screw_dislo_den_1_Q1[_qp];
  screw_dislo_den_Q1[1] = _screw_dislo_den_2_Q1[_qp];
  screw_dislo_den_Q1[2] = _screw_dislo_den_3_Q1[_qp];
  screw_dislo_den_Q1[3] = _screw_dislo_den_4_Q1[_qp];
  screw_dislo_den_Q1[4] = _screw_dislo_den_5_Q1[_qp];
  screw_dislo_den_Q1[5] = _screw_dislo_den_6_Q1[_qp];
  screw_dislo_den_Q1[6] = _screw_dislo_den_7_Q1[_qp];
  screw_dislo_den_Q1[7] = _screw_dislo_den_8_Q1[_qp];
  screw_dislo_den_Q1[8] = _screw_dislo_den_9_Q1[_qp];
  screw_dislo_den_Q1[9] = _screw_dislo_den_10_Q1[_qp];
  screw_dislo_den_Q1[10] = _screw_dislo_den_11_Q1[_qp];
  screw_dislo_den_Q1[11] = _screw_dislo_den_12_Q1[_qp];

  screw_dislo_den_Q2[0] = _screw_dislo_den_1_Q2[_qp];
  screw_dislo_den_Q2[1] = _screw_dislo_den_2_Q2[_qp];
  screw_dislo_den_Q2[2] = _screw_dislo_den_3_Q2[_qp];
  screw_dislo_den_Q2[3] = _screw_dislo_den_4_Q2[_qp];
  screw_dislo_den_Q2[4] = _screw_dislo_den_5_Q2[_qp];
  screw_dislo_den_Q2[5] = _screw_dislo_den_6_Q2[_qp];
  screw_dislo_den_Q2[6] = _screw_dislo_den_7_Q2[_qp];
  screw_dislo_den_Q2[7] = _screw_dislo_den_8_Q2[_qp];
  screw_dislo_den_Q2[8] = _screw_dislo_den_9_Q2[_qp];
  screw_dislo_den_Q2[9] = _screw_dislo_den_10_Q2[_qp];
  screw_dislo_den_Q2[10] = _screw_dislo_den_11_Q2[_qp];
  screw_dislo_den_Q2[11] = _screw_dislo_den_12_Q2[_qp];

  screw_dislo_den_Q3[0] = _screw_dislo_den_1_Q3[_qp];
  screw_dislo_den_Q3[1] = _screw_dislo_den_2_Q3[_qp];
  screw_dislo_den_Q3[2] = _screw_dislo_den_3_Q3[_qp];
  screw_dislo_den_Q3[3] = _screw_dislo_den_4_Q3[_qp];
  screw_dislo_den_Q3[4] = _screw_dislo_den_5_Q3[_qp];
  screw_dislo_den_Q3[5] = _screw_dislo_den_6_Q3[_qp];
  screw_dislo_den_Q3[6] = _screw_dislo_den_7_Q3[_qp];
  screw_dislo_den_Q3[7] = _screw_dislo_den_8_Q3[_qp];
  screw_dislo_den_Q3[8] = _screw_dislo_den_9_Q3[_qp];
  screw_dislo_den_Q3[9] = _screw_dislo_den_10_Q3[_qp];
  screw_dislo_den_Q3[10] = _screw_dislo_den_11_Q3[_qp];
  screw_dislo_den_Q3[11] = _screw_dislo_den_12_Q3[_qp];

  screw_dislo_den_Q4[0] = _screw_dislo_den_1_Q4[_qp];
  screw_dislo_den_Q4[1] = _screw_dislo_den_2_Q4[_qp];
  screw_dislo_den_Q4[2] = _screw_dislo_den_3_Q4[_qp];
  screw_dislo_den_Q4[3] = _screw_dislo_den_4_Q4[_qp];
  screw_dislo_den_Q4[4] = _screw_dislo_den_5_Q4[_qp];
  screw_dislo_den_Q4[5] = _screw_dislo_den_6_Q4[_qp];
  screw_dislo_den_Q4[6] = _screw_dislo_den_7_Q4[_qp];
  screw_dislo_den_Q4[7] = _screw_dislo_den_8_Q4[_qp];
  screw_dislo_den_Q4[8] = _screw_dislo_den_9_Q4[_qp];
  screw_dislo_den_Q4[9] = _screw_dislo_den_10_Q4[_qp];
  screw_dislo_den_Q4[10] = _screw_dislo_den_11_Q4[_qp];
  screw_dislo_den_Q4[11] = _screw_dislo_den_12_Q4[_qp];

  for (const auto i : make_range(_nss))
  {
    edge_dislocation_density[i] =
        edge_dislo_den_Q1[i] + edge_dislo_den_Q2[i] + edge_dislo_den_Q3[i] + edge_dislo_den_Q4[i];

    screw_dislocation_density[i] = screw_dislo_den_Q1[i] + screw_dislo_den_Q2[i] +
                                   screw_dislo_den_Q3[i] + screw_dislo_den_Q4[i];

    total_dislocation_density[i] = edge_dislocation_density[i] + screw_dislocation_density[i];

    TotalDislocationDensity_ALL = 0;
    const Real abs_slip_increment = std::abs(_slip_increment[_qp][i]);

    for (const auto j : make_range(_nss))
    {
      TotalDislocationDensity_ALL += total_dislocation_density[j];
    }

    if (edge_dislocation_density[i] > 0.0)
    {
      _edge_dislocation_increment[_qp][i] =
          _Ce * _ke_b * std::sqrt(TotalDislocationDensity_ALL) * abs_slip_increment -
          _Ce / _burgers * 2.0 * _de * edge_dislocation_density[i] * abs_slip_increment;
    }

    if (screw_dislocation_density[i] > 0.0)
    {
      _screw_dislocation_increment[_qp][i] =
          _Cs * _ks_b * std::sqrt(TotalDislocationDensity_ALL) * abs_slip_increment -
          _Cs / _burgers *
              (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                   std::sqrt(TotalDislocationDensity_ALL) +
               2.0 * _ds) *
              screw_dislocation_density[i] * abs_slip_increment;
    }
  }
}

void
SSDUpdate::computeQpProperties()
{
  std::vector<Real> edge_dislo_den_Q1(_nss);
  std::vector<Real> edge_dislo_den_Q2(_nss);
  std::vector<Real> edge_dislo_den_Q3(_nss);
  std::vector<Real> edge_dislo_den_Q4(_nss);
  std::vector<Real> screw_dislo_den_Q1(_nss);
  std::vector<Real> screw_dislo_den_Q2(_nss);
  std::vector<Real> screw_dislo_den_Q3(_nss);
  std::vector<Real> screw_dislo_den_Q4(_nss);

  std::vector<Real> total_dislocation_density(_nss);
  std::vector<Real> edge_dislocation_density(_nss);
  std::vector<Real> screw_dislocation_density(_nss);

  Real TotalDislocationDensity_ALL;

  edge_dislo_den_Q1[0] = _edge_dislo_den_1_Q1[_qp];
  edge_dislo_den_Q1[1] = _edge_dislo_den_2_Q1[_qp];
  edge_dislo_den_Q1[2] = _edge_dislo_den_3_Q1[_qp];
  edge_dislo_den_Q1[3] = _edge_dislo_den_4_Q1[_qp];
  edge_dislo_den_Q1[4] = _edge_dislo_den_5_Q1[_qp];
  edge_dislo_den_Q1[5] = _edge_dislo_den_6_Q1[_qp];
  edge_dislo_den_Q1[6] = _edge_dislo_den_7_Q1[_qp];
  edge_dislo_den_Q1[7] = _edge_dislo_den_8_Q1[_qp];
  edge_dislo_den_Q1[8] = _edge_dislo_den_9_Q1[_qp];
  edge_dislo_den_Q1[9] = _edge_dislo_den_10_Q1[_qp];
  edge_dislo_den_Q1[10] = _edge_dislo_den_11_Q1[_qp];
  edge_dislo_den_Q1[11] = _edge_dislo_den_12_Q1[_qp];

  edge_dislo_den_Q2[0] = _edge_dislo_den_1_Q2[_qp];
  edge_dislo_den_Q2[1] = _edge_dislo_den_2_Q2[_qp];
  edge_dislo_den_Q2[2] = _edge_dislo_den_3_Q2[_qp];
  edge_dislo_den_Q2[3] = _edge_dislo_den_4_Q2[_qp];
  edge_dislo_den_Q2[4] = _edge_dislo_den_5_Q2[_qp];
  edge_dislo_den_Q2[5] = _edge_dislo_den_6_Q2[_qp];
  edge_dislo_den_Q2[6] = _edge_dislo_den_7_Q2[_qp];
  edge_dislo_den_Q2[7] = _edge_dislo_den_8_Q2[_qp];
  edge_dislo_den_Q2[8] = _edge_dislo_den_9_Q2[_qp];
  edge_dislo_den_Q2[9] = _edge_dislo_den_10_Q2[_qp];
  edge_dislo_den_Q2[10] = _edge_dislo_den_11_Q2[_qp];
  edge_dislo_den_Q2[11] = _edge_dislo_den_12_Q2[_qp];

  edge_dislo_den_Q3[0] = _edge_dislo_den_1_Q3[_qp];
  edge_dislo_den_Q3[1] = _edge_dislo_den_2_Q3[_qp];
  edge_dislo_den_Q3[2] = _edge_dislo_den_3_Q3[_qp];
  edge_dislo_den_Q3[3] = _edge_dislo_den_4_Q3[_qp];
  edge_dislo_den_Q3[4] = _edge_dislo_den_5_Q3[_qp];
  edge_dislo_den_Q3[5] = _edge_dislo_den_6_Q3[_qp];
  edge_dislo_den_Q3[6] = _edge_dislo_den_7_Q3[_qp];
  edge_dislo_den_Q3[7] = _edge_dislo_den_8_Q3[_qp];
  edge_dislo_den_Q3[8] = _edge_dislo_den_9_Q3[_qp];
  edge_dislo_den_Q3[9] = _edge_dislo_den_10_Q3[_qp];
  edge_dislo_den_Q3[10] = _edge_dislo_den_11_Q3[_qp];
  edge_dislo_den_Q3[11] = _edge_dislo_den_12_Q3[_qp];

  edge_dislo_den_Q4[0] = _edge_dislo_den_1_Q4[_qp];
  edge_dislo_den_Q4[1] = _edge_dislo_den_2_Q4[_qp];
  edge_dislo_den_Q4[2] = _edge_dislo_den_3_Q4[_qp];
  edge_dislo_den_Q4[3] = _edge_dislo_den_4_Q4[_qp];
  edge_dislo_den_Q4[4] = _edge_dislo_den_5_Q4[_qp];
  edge_dislo_den_Q4[5] = _edge_dislo_den_6_Q4[_qp];
  edge_dislo_den_Q4[6] = _edge_dislo_den_7_Q4[_qp];
  edge_dislo_den_Q4[7] = _edge_dislo_den_8_Q4[_qp];
  edge_dislo_den_Q4[8] = _edge_dislo_den_9_Q4[_qp];
  edge_dislo_den_Q4[9] = _edge_dislo_den_10_Q4[_qp];
  edge_dislo_den_Q4[10] = _edge_dislo_den_11_Q4[_qp];
  edge_dislo_den_Q4[11] = _edge_dislo_den_12_Q4[_qp];

  screw_dislo_den_Q1[0] = _screw_dislo_den_1_Q1[_qp];
  screw_dislo_den_Q1[1] = _screw_dislo_den_2_Q1[_qp];
  screw_dislo_den_Q1[2] = _screw_dislo_den_3_Q1[_qp];
  screw_dislo_den_Q1[3] = _screw_dislo_den_4_Q1[_qp];
  screw_dislo_den_Q1[4] = _screw_dislo_den_5_Q1[_qp];
  screw_dislo_den_Q1[5] = _screw_dislo_den_6_Q1[_qp];
  screw_dislo_den_Q1[6] = _screw_dislo_den_7_Q1[_qp];
  screw_dislo_den_Q1[7] = _screw_dislo_den_8_Q1[_qp];
  screw_dislo_den_Q1[8] = _screw_dislo_den_9_Q1[_qp];
  screw_dislo_den_Q1[9] = _screw_dislo_den_10_Q1[_qp];
  screw_dislo_den_Q1[10] = _screw_dislo_den_11_Q1[_qp];
  screw_dislo_den_Q1[11] = _screw_dislo_den_12_Q1[_qp];

  screw_dislo_den_Q2[0] = _screw_dislo_den_1_Q2[_qp];
  screw_dislo_den_Q2[1] = _screw_dislo_den_2_Q2[_qp];
  screw_dislo_den_Q2[2] = _screw_dislo_den_3_Q2[_qp];
  screw_dislo_den_Q2[3] = _screw_dislo_den_4_Q2[_qp];
  screw_dislo_den_Q2[4] = _screw_dislo_den_5_Q2[_qp];
  screw_dislo_den_Q2[5] = _screw_dislo_den_6_Q2[_qp];
  screw_dislo_den_Q2[6] = _screw_dislo_den_7_Q2[_qp];
  screw_dislo_den_Q2[7] = _screw_dislo_den_8_Q2[_qp];
  screw_dislo_den_Q2[8] = _screw_dislo_den_9_Q2[_qp];
  screw_dislo_den_Q2[9] = _screw_dislo_den_10_Q2[_qp];
  screw_dislo_den_Q2[10] = _screw_dislo_den_11_Q2[_qp];
  screw_dislo_den_Q2[11] = _screw_dislo_den_12_Q2[_qp];

  screw_dislo_den_Q3[0] = _screw_dislo_den_1_Q3[_qp];
  screw_dislo_den_Q3[1] = _screw_dislo_den_2_Q3[_qp];
  screw_dislo_den_Q3[2] = _screw_dislo_den_3_Q3[_qp];
  screw_dislo_den_Q3[3] = _screw_dislo_den_4_Q3[_qp];
  screw_dislo_den_Q3[4] = _screw_dislo_den_5_Q3[_qp];
  screw_dislo_den_Q3[5] = _screw_dislo_den_6_Q3[_qp];
  screw_dislo_den_Q3[6] = _screw_dislo_den_7_Q3[_qp];
  screw_dislo_den_Q3[7] = _screw_dislo_den_8_Q3[_qp];
  screw_dislo_den_Q3[8] = _screw_dislo_den_9_Q3[_qp];
  screw_dislo_den_Q3[9] = _screw_dislo_den_10_Q3[_qp];
  screw_dislo_den_Q3[10] = _screw_dislo_den_11_Q3[_qp];
  screw_dislo_den_Q3[11] = _screw_dislo_den_12_Q3[_qp];

  screw_dislo_den_Q4[0] = _screw_dislo_den_1_Q4[_qp];
  screw_dislo_den_Q4[1] = _screw_dislo_den_2_Q4[_qp];
  screw_dislo_den_Q4[2] = _screw_dislo_den_3_Q4[_qp];
  screw_dislo_den_Q4[3] = _screw_dislo_den_4_Q4[_qp];
  screw_dislo_den_Q4[4] = _screw_dislo_den_5_Q4[_qp];
  screw_dislo_den_Q4[5] = _screw_dislo_den_6_Q4[_qp];
  screw_dislo_den_Q4[6] = _screw_dislo_den_7_Q4[_qp];
  screw_dislo_den_Q4[7] = _screw_dislo_den_8_Q4[_qp];
  screw_dislo_den_Q4[8] = _screw_dislo_den_9_Q4[_qp];
  screw_dislo_den_Q4[9] = _screw_dislo_den_10_Q4[_qp];
  screw_dislo_den_Q4[10] = _screw_dislo_den_11_Q4[_qp];
  screw_dislo_den_Q4[11] = _screw_dislo_den_12_Q4[_qp];

  for (const auto i : make_range(_nss))
  {
    edge_dislocation_density[i] =
        edge_dislo_den_Q1[i] + edge_dislo_den_Q2[i] + edge_dislo_den_Q3[i] + edge_dislo_den_Q4[i];

    screw_dislocation_density[i] = screw_dislo_den_Q1[i] + screw_dislo_den_Q2[i] +
                                   screw_dislo_den_Q3[i] + screw_dislo_den_Q4[i];

    total_dislocation_density[i] = edge_dislocation_density[i] + screw_dislocation_density[i];

    TotalDislocationDensity_ALL = 0;

    const Real abs_slip_increment = std::abs(_slip_increment[_qp][i]);

    for (const auto j : make_range(_nss))
    {
      TotalDislocationDensity_ALL += total_dislocation_density[j];
    }

    if (edge_dislocation_density[i] > 0.0)
    {
      _edge_dislocation_increment[_qp][i] =
          _edge_dislocation_increment_old[_qp][i] +
          (_Ce * _ke_b * std::sqrt(TotalDislocationDensity_ALL) * abs_slip_increment -
           _Ce / _burgers * 2.0 * _de * edge_dislocation_density[i] * abs_slip_increment) *
              _dt;
    }

    if (screw_dislocation_density[i] > 0.0)
    {
      _screw_dislocation_increment[_qp][i] =
          _screw_dislocation_increment_old[_qp][i] +
          (_Cs * _ks_b * std::sqrt(TotalDislocationDensity_ALL) * abs_slip_increment -
           _Cs / _burgers *
               (M_PI * std::pow(_ds, 2.0) * _ks_b * _burgers *
                    std::sqrt(TotalDislocationDensity_ALL) +
                2.0 * _ds) *
               screw_dislocation_density[i] * abs_slip_increment) *
              _dt;
    }
  }
}
