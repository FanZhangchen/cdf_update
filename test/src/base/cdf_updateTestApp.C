//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "cdf_updateTestApp.h"
#include "cdf_updateApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

InputParameters
cdf_updateTestApp::validParams()
{
  InputParameters params = cdf_updateApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  return params;
}

cdf_updateTestApp::cdf_updateTestApp(InputParameters parameters) : MooseApp(parameters)
{
  cdf_updateTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

cdf_updateTestApp::~cdf_updateTestApp() {}

void
cdf_updateTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  cdf_updateApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"cdf_updateTestApp"});
    Registry::registerActionsTo(af, {"cdf_updateTestApp"});
  }
}

void
cdf_updateTestApp::registerApps()
{
  registerApp(cdf_updateApp);
  registerApp(cdf_updateTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
cdf_updateTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  cdf_updateTestApp::registerAll(f, af, s);
}
extern "C" void
cdf_updateTestApp__registerApps()
{
  cdf_updateTestApp::registerApps();
}
