#include "MtInstance.h"

#include "Log.h"
#include "MtModule.h"
#include "MtModLibrary.h"
#include "MtMethod.h"
#include "MtStruct.h"
#include "MtField.h"

void dump_state(TraceState state) {
  switch(state) {
    case CTX_NONE:     LOG_C(0x444444, "CTX_NONE"); break;
    case CTX_INPUT:    LOG_C(0x88FFFF, "CTX_INPUT"); break;
    case CTX_OUTPUT:   LOG_C(0xFFCCCC, "CTX_OUTPUT"); break;
    case CTX_MAYBE:    LOG_C(0x44CCFF, "CTX_MAYBE"); break;
    case CTX_SIGNAL:   LOG_C(0x88FF88, "CTX_SIGNAL"); break;
    case CTX_REGISTER: LOG_C(0x88BBBB, "CTX_REGISTER"); break;
    case CTX_INVALID:  LOG_C(0xFF00FF, "CTX_INVALID"); break;
    case CTX_PENDING:  LOG_C(0x444444, "CTX_PENDING"); break;
    case CTX_NIL:      LOG_C(0x444444, "CTX_NIL"); break;
  }
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

MtInstance* node_to_inst(MnNode n, MtModLibrary* lib) {
  auto param_type = n.type5();
  auto param_decl = n.get_field(field_declarator);
  auto param_name = n.name4();

  if (auto s = lib->get_struct(param_type)) {
    return new MtStructInstance(s);
  }
  else if (param_decl.sym == sym_array_declarator) {
    return new MtArrayInstance();
  }
  else {
    return new MtPrimitiveInstance();
  }
}

//------------------------------------------------------------------------------

const std::string& MtInstance::name() const {
  static const std::string dummy = "<none>";
  return dummy;
}

//------------------------------------------------------------------------------

MtInstance::MtInstance() {
  log_top = {CTX_NONE};
  //log_next = {CTX_NONE};
}

MtInstance::~MtInstance() {
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
  LOG_B("Primitive @ 0x%04X ", uint64_t(this) & 0xFFFF);
  dump_state(log_top.state);
  LOG("\n");
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
  LOG_B("Array @ 0x%04X ", uint64_t(this) & 0xFFFF);
  dump_state(log_top.state);
  LOG("\n");
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
  LOG_B("Struct @ 0x%04X ", uint64_t(this) & 0xFFFF);
  dump_state(log_top.state);
  LOG("\n");
  LOG_INDENT_SCOPE();

  for (const auto& f : _fields) {
    LOG_G("%s: ", f.first.c_str());
    f.second->dump();
  }
}

MtInstance* MtStructInstance::get_field(const std::string& name) {
  auto it = _fields.find(name);
  if (it == _fields.end()) {
    return nullptr;
  }
  else {
    return (*it).second;
  }
}

MtInstance* MtStructInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;

  auto result = get_field(path[index]);
  if (result) {
    return result->resolve(path, index + 1);
  }
  return nullptr;
}

//------------------------------------------------------------------------------


MtMethodInstance::MtMethodInstance(MtModuleInstance* module, MtMethod* method) : _module(module), _method(method) {
  _name = method->name();
  for (auto c : _method->param_nodes) {
    _params[c.name4()] = node_to_inst(c, _method->_lib);
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
  auto it = _params.find(name);
  if (it == _params.end()) {
    return nullptr;
  }
  else {
    return (*it).second;
  }
}

void MtMethodInstance::dump() {
  LOG_B("Method '%s' @ 0x%04X ", _name.c_str(), uint64_t(this) & 0xFFFF);
  dump_state(log_top.state);
  LOG("\n");
  LOG_INDENT_SCOPE();
  for (auto p : _params) {
    LOG_G("%s: ", p.first.c_str());
    p.second->dump();
  }
  if (_retval) _retval->dump();
}

MtInstance* MtMethodInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;

  auto param_inst = get_param(path[index]);
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
  LOG_B("Module @ 0x%04X\n", uint64_t(this) & 0xFFFF);
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
  auto it = _methods.find(name);
  if (it == _methods.end()) {
    return nullptr;
  }
  else {
    return (*it).second;
  }
}

MtInstance* MtModuleInstance::get_field(const std::string& name) {
  auto it = _fields.find(name);
  return it == _fields.end() ? nullptr : (*it).second;
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
