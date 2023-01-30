#include "MtInstance.h"

#include "Log.h"
#include "MtModule.h"
#include "MtModLibrary.h"
#include "MtMethod.h"
#include "MtStruct.h"
#include "MtField.h"

//------------------------------------------------------------------------------

const std::string& MtInstance::name() const {
  static const std::string dummy = "<none>";
  return dummy;
}

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

const std::string& MtPrimitiveInstance::name() const {
  static const std::string dummy = "<primitive>";
  return dummy;
}

void MtPrimitiveInstance::dump() {
  LOG_B("MtPrimitiveInstance @ %p\n", this);
}

//------------------------------------------------------------------------------

MtArrayInstance::MtArrayInstance() {
}

MtArrayInstance::~MtArrayInstance() {
}

const std::string& MtArrayInstance::name() const {
  static const std::string dummy = "<array>";
  return dummy;
}

void MtArrayInstance::dump() {
  LOG_B("MtArrayInstance @ %p\n", this);
}

//------------------------------------------------------------------------------

MtInstance* field_to_inst(MtField* field) {

  if (field->is_struct()) {
    return new MtStructInstance(field->_type_struct);
  }
  else if (field->is_component()) {
    return new MtModuleInstance(field->_type_mod);
  }
  else if (field->is_array()) {
    return new MtArrayInstance();
  }
  else {
    return new MtPrimitiveInstance();
  }
}

//------------------------------------------------------------------------------

MtStructInstance::MtStructInstance(MtStruct* _struct)
: _struct(_struct)
{
  for (auto cf : _struct->fields) {
    _fields[cf->name()] = field_to_inst(cf);
  }

}

MtStructInstance::~MtStructInstance() {
}

const std::string& MtStructInstance::name() const {
  static const std::string dummy = "<nullptr>";
  return _struct ? _struct->name : dummy;
}

void MtStructInstance::dump() {
  LOG_B("MtStructInstance @ %p\n", this);
  LOG_INDENT_SCOPE();

  for (const auto& f : _fields) {
    LOG_G("%s: ", f.first.c_str());
    f.second->dump();
  }
}

MtInstance* MtStructInstance::get_field(const std::string& name) {
  return _fields[name];
}

MtInstance* MtStructInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;

  auto result = _fields[path[index]];
  if (result) {
    return result->resolve(path, index + 1);
  }
  return nullptr;
}

//------------------------------------------------------------------------------

MtMethodInstance::MtMethodInstance(MtModuleInstance* module, MtMethod* method) : _module(module), _method(method) {
  _name = method->name();

  auto lib = _method->_lib;

  for (auto c : _method->param_nodes) {
    auto param_type = c.type5();
    auto param_decl = c.get_field(field_declarator);
    auto param_name = c.name4();

    if (auto s = lib->get_struct(param_type)) {
      _params[param_name] = new MtStructInstance(s);
    }
    else if (param_decl.sym == sym_array_declarator) {
      _params[param_name] = new MtArrayInstance();
    }
    else {
      _params[param_name] = new MtPrimitiveInstance();
    }
  }

  if (_method->has_return()) {
    //_retval = new MtRetvalInstance();
  }
}

MtMethodInstance::~MtMethodInstance() {
  for (auto a : _params) delete a.second;
  _params.clear();
}

const std::string& MtMethodInstance::name() const {
  return _name;
}

MtInstance* MtMethodInstance::get_param(const std::string& name) {
  return _params[name];
}

void MtMethodInstance::dump() {
  LOG_B("MtMethodInstance '%s' @ %p\n", _name.c_str(), this);
  LOG_INDENT_SCOPE();
  for (auto p : _params) {
    LOG_G("%s: ", p.first.c_str());
    p.second->dump();
  }
  if (_retval) _retval->dump();
}

MtInstance* MtMethodInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;

  auto param_inst = _params[path[index]];
  if (param_inst) {
    return param_inst->resolve(path, index + 1);
  }

  return _module->resolve(path, index);
}

//------------------------------------------------------------------------------

MtModuleInstance::MtModuleInstance(MtModule* _mod) {
  assert(_mod);

  this->_mod = _mod;

  for (auto cf : _mod->all_fields) {
    _fields[cf->name()] = field_to_inst(cf);
  }

  for (auto m : _mod->all_methods) {
    _methods[m->name()] = new MtMethodInstance(this, m);
  }
}

//----------------------------------------

MtModuleInstance::~MtModuleInstance() {
  for (auto f : _fields) delete f.second;
  for (auto m : _methods) delete m.second;
}

//----------------------------------------

void MtModuleInstance::dump() {
  LOG_B("MtModuleInstance @ %p\n", this);
  LOG_INDENT_SCOPE();
  for (auto f : _fields) {
    LOG_G("%s: ", f.first.c_str());
    f.second->dump();
  }
  for (auto m : _methods) {
    LOG_G("%s: ", m.first.c_str());
    m.second->dump();
  }
}

//----------------------------------------

MtMethodInstance* MtModuleInstance::get_method(const std::string& name) {
  return _methods[name];
}

MtInstance* MtModuleInstance::get_field(const std::string& name) {
  return _fields[name];
}

//------------------------------------------------------------------------------

MtInstance* MtModuleInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;

  for (auto f : _fields) {
    if (f.second->name() == path[index]) return f.second->resolve(path, index + 1);
  }

  for (auto m : _methods) {
    if (m.second->_name == path[index]) return m.second;
  }

  return nullptr;
}

//------------------------------------------------------------------------------
