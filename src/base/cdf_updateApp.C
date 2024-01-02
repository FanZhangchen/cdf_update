#include "cdf_updateApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
cdf_updateApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  return params;
}

cdf_updateApp::cdf_updateApp(InputParameters parameters) : MooseApp(parameters)
{
  cdf_updateApp::registerAll(_factory, _action_factory, _syntax);
}

cdf_updateApp::~cdf_updateApp() {}

void 
cdf_updateApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAllObjects<cdf_updateApp>(f, af, s);
  Registry::registerObjectsTo(f, {"cdf_updateApp"});
  Registry::registerActionsTo(af, {"cdf_updateApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
cdf_updateApp::registerApps()
{
  registerApp(cdf_updateApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
cdf_updateApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  cdf_updateApp::registerAll(f, af, s);
}
extern "C" void
cdf_updateApp__registerApps()
{
  cdf_updateApp::registerApps();
}
