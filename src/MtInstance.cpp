#include "MtInstance.h"

#include "Log.h"
#include "MtModule.h"
#include "MtModLibrary.h"
#include "MtMethod.h"
#include "MtStruct.h"
#include "MtField.h"

//------------------------------------------------------------------------------

MtScope::MtScope() {
}

MtScope::~MtScope() {
}

void MtScope::dump() {
  LOG_B("MtScope @ %p\n", this);
}

//------------------------------------------------------------------------------

MtPrimitiveInstance::MtPrimitiveInstance() {
}

MtPrimitiveInstance::~MtPrimitiveInstance() {
}

void MtPrimitiveInstance::dump() {
  LOG_B("MtPrimitiveInstance @ %p\n", this);
}

//------------------------------------------------------------------------------

MtArrayInstance::MtArrayInstance() {
}

MtArrayInstance::~MtArrayInstance() {
}

void MtArrayInstance::dump() {
  LOG_B("MtArrayInstance @ %p\n", this);
}

//------------------------------------------------------------------------------

MtParamInstance::MtParamInstance(const std::string& name, MtInstance* value) {
  _name = name;
  _value = value;
}

MtParamInstance::~MtParamInstance() {
}

void MtParamInstance::dump() {
  LOG_B("MtParamInstance '%s' @ %p\n", _name.c_str(), this);
  LOG_INDENT_SCOPE();
  if (_value) _value->dump();
}

//------------------------------------------------------------------------------

MtFieldInstance::MtFieldInstance(MtField* field) : _field(field) {
  if (field->is_struct()) {
    _value = new MtStructInstance(field->_type_struct);
  }
  else if (field->is_component()) {
    _value = new MtModuleInstance(field->_type_mod);
  }
  else if (field->is_array()) {
    _value = new MtArrayInstance();
  }
  else {
    _value = new MtPrimitiveInstance();
  }
}

MtFieldInstance::~MtFieldInstance() {
  delete _value;
  _value = nullptr;
}

void MtFieldInstance::dump() {
  LOG_B("MtFieldInstance @ %p\n", this);
  LOG_INDENT_SCOPE();
  if (_value) _value->dump();
}

//------------------------------------------------------------------------------

MtMethodInstance::MtMethodInstance(MtModuleInstance* module, MtMethod* method) : _module(module), _method(method) {
  _name = method->name();

  auto lib = _method->_lib;

  for (auto c : _method->param_nodes) {
    //c.dump_tree();
    auto param_type = c.type5();
    auto param_name = c.name4();

    if (auto s = lib->get_struct(param_type)) {
      _params.push_back(new MtParamInstance(param_name, new MtStructInstance(s)));
    }
    else {
      _params.push_back(new MtParamInstance(param_name, new MtPrimitiveInstance()));
    }
  }

  if (_method->has_return()) {
    //_retval = new MtRetvalInstance();
  }
}

MtMethodInstance::~MtMethodInstance() {
  for (auto a : _params) delete a;
  _params.clear();
}

void MtMethodInstance::dump() {
  LOG_B("MtMethodInstance '%s' @ %p\n", _name.c_str(), this);
  LOG_INDENT_SCOPE();
  for (auto p : _params) p->dump();
  if (_retval) _retval->dump();
}

//------------------------------------------------------------------------------

MtStructInstance::MtStructInstance(MtStruct* _struct)
: _struct(_struct)
{
#if 0
  for (auto cf : _struct->fields) {
  }
#endif
}

MtStructInstance::~MtStructInstance() {
}

void MtStructInstance::dump() {
  LOG_B("MtStructInstance @ %p\n", this);
  LOG_INDENT_SCOPE();
  for (auto f : _fields) f->dump();
}

//------------------------------------------------------------------------------

MtModuleInstance::MtModuleInstance(MtModule* _mod) {
  assert(_mod);

  this->_mod = _mod;

  for (auto cf : _mod->all_fields) {
    if (cf->is_struct()) {
      _fields.push_back(new MtFieldInstance(cf));
    }
    else if (cf->is_component()) {
      _fields.push_back(new MtFieldInstance(cf));
    }
    else if (cf->is_array()) {
      _fields.push_back(new MtFieldInstance(cf));
    }
    else {
      _fields.push_back(new MtFieldInstance(cf));
    }
  }

  for (auto m : _mod->all_methods) {
    _methods.push_back(new MtMethodInstance(this, m));
  }
}

//----------------------------------------

MtModuleInstance::~MtModuleInstance() {
  for (auto f : _fields) delete f;
  for (auto m : _methods) delete m;
}

//----------------------------------------

void MtModuleInstance::dump() {
  LOG_B("MtModuleInstance @ %p\n", this);
  LOG_INDENT_SCOPE();
  for (auto f : _fields)  f->dump();
  for (auto m : _methods) m->dump();
}

//----------------------------------------

MtMethodInstance* MtModuleInstance::get_method(const std::string& name) {
  for (auto m : _methods) if (m->_method->name() == name) return m;
  return nullptr;
}

MtFieldInstance* MtModuleInstance::get_field(const std::string& name) {
  for (auto f : _fields) if (f->_name == name) return f;
  return nullptr;
}

//------------------------------------------------------------------------------
