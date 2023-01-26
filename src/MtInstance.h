#pragma once
#include <vector>
#include <string>
#include "MtUtils.h"

struct MtField;
struct MtMethod;
struct MtModule;
struct MtStruct;

struct MtArgumentInstance;
struct MtFieldInstance;
struct MtMethodInstance;
struct MtModuleInstance;
struct MtRetvalInstance;
struct MtStructInstance;

//------------------------------------------------------------------------------

struct MtArgumentInstance {
  std::string _name;
  ContextType _log_top;
  ContextType _log_next;
};

//------------------------------------------------------------------------------

struct MtRetvalInstance {
  ContextType _log_top;
  ContextType _log_next;
};

//------------------------------------------------------------------------------

struct MtFieldInstance {
  MtFieldInstance(MtField* f);

  std::string _name;
  ContextType _log_top;
  ContextType _log_next;

  MtField*  _field;
  std::vector<ContextType> _action_log;
};

//------------------------------------------------------------------------------

struct MtMethodInstance {
  MtMethodInstance(MtMethod* m);

  MtMethod* _method;
  std::vector<MtArgumentInstance*> _args;
  MtRetvalInstance* _retval = nullptr;
};

//------------------------------------------------------------------------------

struct MtModuleInstance {
  MtModuleInstance(MtModule* m);

  MtModule* _mod;
  std::vector<MtFieldInstance*> _fields;
  std::vector<MtMethodInstance*> _methods;
};

//------------------------------------------------------------------------------

struct MtStructInstance {
  MtStructInstance(MtStruct* s);

  MtStruct* _struct;
  std::vector<MtFieldInstance*> _fields;
};

//------------------------------------------------------------------------------
