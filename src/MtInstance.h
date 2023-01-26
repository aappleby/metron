#pragma once
#include <vector>
#include <string>
#include "MtUtils.h"

struct MtField;
struct MtModule;
struct MtStruct;

struct MtArgumentInstance;
struct MtRetvalInstance;
struct MtFieldInstance;
struct MtMethodInstance;
struct MtModuleInstance;
struct MtStructInstance;

//------------------------------------------------------------------------------

struct MtArgumentInstance {
  std::string name;
  ContextType log_top;
  ContextType log_next;
};

//------------------------------------------------------------------------------

struct MtRetvalInstance {
  ContextType log_top;
  ContextType log_next;
};

//------------------------------------------------------------------------------

struct MtFieldInstance {
  ContextType log_top;
  ContextType log_next;

  MtField*  _field;
  std::vector<ContextType> action_log;
};

//------------------------------------------------------------------------------

struct MtMethodInstance {
  std::vector<MtArgumentInstance*> _args;
  MtRetvalInstance* _retval = nullptr;
};

//------------------------------------------------------------------------------

struct MtModuleInstance {
  MtModule* _mod;
  std::vector<MtFieldInstance*> _fields;
  std::vector<MtMethodInstance*> _methods;
};

//------------------------------------------------------------------------------

struct MtStructInstance {
  MtStruct* _struct;
  std::vector<MtFieldInstance*> _fields;
};

//------------------------------------------------------------------------------
