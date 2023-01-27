#pragma once
#include <vector>
#include <string>
#include "MtUtils.h"

struct MtField;
struct MtMethod;
struct MtModule;
struct MtStruct;

struct MtInstance;
struct MtPrimitiveInstance;
struct MtStructInstance;
struct MtMethodInstance;
struct MtModuleInstance;

//------------------------------------------------------------------------------

struct MtInstance {
  MtInstance(const std::string& name);
  virtual ~MtInstance();

  virtual void dump();

  std::string _name;
  ContextType _log_top;
  ContextType _log_next;
  std::vector<ContextType> _action_log;
};

//------------------------------------------------------------------------------

struct MtPrimitiveInstance : public MtInstance {
  MtPrimitiveInstance(MtField* _field);
  virtual ~MtPrimitiveInstance();
  void dump() override;

  MtField* _field;
};

//------------------------------------------------------------------------------

struct MtArrayInstance : public MtInstance {
  MtArrayInstance(MtField* _field);
  virtual ~MtArrayInstance();
  void dump() override;

  MtField* _field;
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtInstance {
  MtStructInstance(MtField* f);
  void dump() override;

  MtField* _field;
  std::vector<MtInstance*> _fields;
};

//------------------------------------------------------------------------------

struct MtMethodInstance : public MtInstance {
  MtMethodInstance(MtMethod* m);
  virtual ~MtMethodInstance();
  void dump() override;

  MtMethod* _method;
  std::vector<MtInstance*> _args;
  MtInstance* _retval = nullptr;
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtInstance {
  MtModuleInstance(MtModule* m);
  MtModuleInstance(MtField* f);
  virtual ~MtModuleInstance();
  void dump() override;

  MtField* _field;
  MtModule* _mod;
  std::vector<MtInstance*> _fields;
  std::vector<MtMethodInstance*> _methods;
};

//------------------------------------------------------------------------------
