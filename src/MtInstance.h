#pragma once
#include <vector>
#include <string>
#include <map>
#include "MtUtils.h"

struct MtField;
struct MtMethod;
struct MtModule;
struct MtStruct;
struct MnNode;


struct MtInstance;
struct MtArrayInstance;
struct MtPrimitiveInstance;
struct MtStructInstance;
struct MtMethodInstance;
struct MtModuleInstance;

typedef struct std::map<std::string, MtInstance*> field_map;
typedef struct std::map<std::string, MtMethodInstance*> method_map;

//------------------------------------------------------------------------------

struct MtScope {
  MtScope();
  virtual ~MtScope();
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtInstance {
  MtInstance();
  virtual ~MtInstance();
  virtual const std::string& name() const;
  virtual void dump() {}

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index) {
    return (index == path.size()) ? this : nullptr;
  }

  LogEntry log_top;
  //LogEntry log_next;
  //std::vector<LogEntry> action_log;
};

//------------------------------------------------------------------------------

struct MtPrimitiveInstance : public MtInstance {
  MtPrimitiveInstance();
  virtual ~MtPrimitiveInstance();
  virtual const std::string& name() const;
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtArrayInstance : public MtInstance {
  MtArrayInstance();
  virtual ~MtArrayInstance();
  virtual const std::string& name() const;
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtInstance {
  MtStructInstance(MtStruct* s);
  virtual ~MtStructInstance();
  virtual const std::string& name() const;
  virtual void dump();

  MtInstance* get_field(const std::string& name);
  MtInstance* resolve(const std::vector<std::string>& path, int index);

  MtStruct* _struct;
  field_map _fields;
};

//------------------------------------------------------------------------------













//------------------------------------------------------------------------------

struct MtMethodInstance : public MtInstance {
  MtMethodInstance(MtModuleInstance* module, MtMethod* method);
  virtual ~MtMethodInstance();
  virtual const std::string& name() const;
  MtInstance* get_param(const std::string& name);
  virtual void dump();

  MtInstance* resolve(const std::vector<std::string>& path, int index);

  std::string _name;
  MtMethod* _method;
  MtModuleInstance* _module;
  field_map _params;
  MtInstance* _retval = nullptr;
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtInstance {
  MtModuleInstance(MtModule* m);
  virtual ~MtModuleInstance();
  virtual void dump();

  MtMethodInstance* get_method(const std::string& name);
  MtInstance*       get_field (const std::string& name);

  MtInstance* resolve(const std::vector<std::string>& path, int index);

  MtModule*  _mod;
  field_map  _fields;
  method_map _methods;
};

//------------------------------------------------------------------------------
