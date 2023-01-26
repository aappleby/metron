#pragma once
#include <vector>
#include "MtUtils.h"

struct MtField;
struct MtModule;
struct MtStruct;

struct MtFieldInstance;
struct MtModuleInstance;
struct MtStructInstance;
struct MtCallInstance;
struct MtArgumentInstance;
struct MtRetvalInstance;

//------------------------------------------------------------------------------

struct MtArgumentInstance {
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

struct MtModuleInstance {
  MtModule* _mod;
  std::vector<MtFieldInstance*> _fields;
};

//------------------------------------------------------------------------------

struct MtStructInstance {
  MtStruct* _struct;
  std::vector<MtFieldInstance*> _fields;
};

//------------------------------------------------------------------------------

struct MtMethodInstance {
  std::vector<MtArgumentInstance*> _args;
  MtRetvalInstance* _retval = nullptr;
};

//------------------------------------------------------------------------------
