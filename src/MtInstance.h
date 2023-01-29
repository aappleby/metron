#pragma once
#include <vector>
#include <string>
#include "MtUtils.h"

struct MtField;
struct MtMethod;
struct MtModule;
struct MtStruct;
struct MnNode;


struct MtInstance;
struct MtPrimitiveInstance;
struct MtStructInstance;
struct MtMethodInstance;
struct MtModuleInstance;

struct MtInstance {
  MtInstance() {}
  virtual ~MtInstance() {}
  virtual const std::string& name() const;
  virtual void dump() {}
};

//------------------------------------------------------------------------------

struct MtScope {
  MtScope();
  virtual ~MtScope();
  virtual void dump();
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

struct MtParamInstance {
  MtParamInstance(const std::string& name, MtInstance* value);
  virtual ~MtParamInstance();
  virtual const std::string& name() const { return _name; }
  virtual void dump();

  std::string _name;
  MtInstance* _value;
};

//------------------------------------------------------------------------------

struct MtFieldInstance {
  MtFieldInstance(MtField* field);
  virtual ~MtFieldInstance();
  virtual const std::string& name() const { return _name; }
  virtual void dump();

  std::string _name;
  MtField*    _field;
  MtInstance* _value;
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtInstance {
  MtStructInstance(MtStruct* s);
  virtual ~MtStructInstance();
  virtual const std::string& name() const;
  virtual void dump();

  MtStruct* _struct;
  std::vector<MtFieldInstance*> _fields;
};

//------------------------------------------------------------------------------

struct MtMethodInstance {
  MtMethodInstance(MtModuleInstance* module, MtMethod* method);
  virtual ~MtMethodInstance();
  virtual const std::string& name() const;
  MtParamInstance* get_param(const std::string& name);
  virtual void dump();

  std::string _name;
  MtMethod* _method;
  MtModuleInstance* _module;
  std::vector<MtParamInstance*> _params;
  MtInstance* _retval = nullptr;
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtInstance {
  MtModuleInstance(MtModule* m);
  virtual ~MtModuleInstance();
  virtual void dump();

  MtMethodInstance* get_method(const std::string& name);
  MtFieldInstance*  get_field (const std::string& name);

  MtModule* _mod;
  std::vector<MtFieldInstance*>  _fields;
  std::vector<MtMethodInstance*> _methods;
};

//------------------------------------------------------------------------------
