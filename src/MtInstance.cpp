#include "MtInstance.h"

#include "Log.h"
#include "MtModule.h"
#include "MtModLibrary.h"
#include "MtMethod.h"
#include "MtStruct.h"
#include "MtField.h"

void dump_state(TraceState state) {
  switch(state) {
    case CTX_NONE:     LOG_C(0x444444, "NONE"); break;
    case CTX_INPUT:    LOG_C(0x88FFFF, "INPUT"); break;
    case CTX_OUTPUT:   LOG_C(0xFFCCCC, "OUTPUT"); break;
    case CTX_MAYBE:    LOG_C(0x44CCFF, "MAYBE"); break;
    case CTX_SIGNAL:   LOG_C(0x88FF88, "SIGNAL"); break;
    case CTX_REGISTER: LOG_C(0x88BBBB, "REGISTER"); break;
    case CTX_INVALID:  LOG_C(0xFF00FF, "INVALID"); break;
    case CTX_PENDING:  LOG_C(0x444444, "PENDING"); break;
  }
}

//------------------------------------------------------------------------------

MtInstance* field_to_inst(const std::string& name, const std::string& path, MtField* field) {

  if (field->is_struct()) {
    return new MtStructInstance(name, path, field->_type_struct);
  }
  else if (field->is_component()) {
    return new MtModuleInstance(name, path, field->_type_mod);
  }
  else if (field->is_array()) {
    return new MtArrayInstance(name, path);
  }
  else {
    return new MtPrimitiveInstance(name, path);
  }
}

//------------------------------------------------------------------------------

MtInstance* node_to_inst(const std::string& name, const std::string& path, MnNode n, MtModLibrary* lib) {
  auto param_type = n.type5();
  auto param_decl = n.get_field(field_declarator);
  auto param_name = n.name4();

  if (auto s = lib->get_struct(param_type)) {
    return new MtStructInstance(name, path, s);
  }
  else if (param_decl.sym == sym_array_declarator) {
    return new MtArrayInstance(name, path);
  }
  else {
    return new MtPrimitiveInstance(name, path);
  }
}

//------------------------------------------------------------------------------

void dump_log(const std::vector<LogEntry>& log) {
  for (auto a : log) {
    SourceRange source_range;
    if (a.node) {
      source_range = a.node.get_source();
      source_range = source_range.trim();
      LOG_C(0x444444, "%s@%d\n", source_range.path, source_range.row + 1);
    }
    else {
      LOG_C(0x444444, "<no source>\n");
    }

    LOG_INDENT_SCOPE();

    TinyLog::get().set_color(0);
    if (a.node) {
      LOG("\"");
      LOG_RANGE(source_range);
      LOG("\" :: ");
    }


    //a.node.dump_tree();

    if (a.action == ACT_READ || a.action == ACT_WRITE) {
      LOG("%s(", to_string(a.action));
      dump_state(a.old_state);
      LOG(") = ");
      dump_state(a.new_state);
    }
    if (a.action == ACT_PUSH) {
      LOG("%s(", to_string(a.action));
      dump_state(a.old_state);
      LOG(")");
    }
    if (a.action == ACT_POP) {
      LOG("%s(", to_string(a.action));
      dump_state(a.old_state);
      LOG(")");
    }
    if (a.action == ACT_SWAP) {
      LOG("%s(", to_string(a.action));
      dump_state(a.old_state);
      LOG(", ");
      dump_state(a.new_state);
      LOG(")");
    }
    if (a.action == ACT_MERGE) {
      LOG("%s(", to_string(a.action));
      dump_state(a.old_state);
      LOG(", ");
      dump_state(a.new_state);
      LOG(") = ");
      dump_state(merge_branch(a.old_state, a.new_state));
    }
    LOG("\n");

    if (a.action == ACT_PUSH)  LOG_INDENT();
    if (a.action == ACT_POP)   LOG_DEDENT();
    if (a.action == ACT_MERGE) LOG_DEDENT();

    //LOG("%s\n", merge_message(a.old_state, a.action));
  }
}

//------------------------------------------------------------------------------

MtInstance::MtInstance(const std::string& name, const std::string& path)
: _name(name), _path(path) {
  state_stack = {CTX_NONE};
}

//----------------------------------------

MtInstance::~MtInstance() {
}

//----------------------------------------

void MtInstance::reset_state() {
  state_stack = {CTX_NONE};
}

//----------------------------------------

void MtInstance::dump_log() {
  //::dump_log(action_log);
}

//----------------------------------------

CHECK_RETURN Err MtInstance::sanity_check() {
  Err err;
  if (state_stack.size() > 1) {
    err << ERR("Had more than one item in the state stack after trace\n");
  }
  return err;
}

//------------------------------------------------------------------------------

MtPrimitiveInstance::MtPrimitiveInstance(const std::string& name, const std::string& path) : MtInstance(name, path) {
}

//----------------------------------------

MtPrimitiveInstance::~MtPrimitiveInstance() {
}

//----------------------------------------

void MtPrimitiveInstance::dump() {
  LOG_B("Primitive '%s' @ 0x%04X ", _path.c_str(), uint64_t(this) & 0xFFFF);
  dump_state(state_stack.back());
  LOG(" - %s\n", to_string(field_type));
  LOG_INDENT_SCOPE();
  dump_log();
  assert(state_stack.size() == 1);
}

//----------------------------------------

CHECK_RETURN Err MtPrimitiveInstance::sanity_check() {
  return MtInstance::sanity_check();
}

//------------------------------------------------------------------------------

MtArrayInstance::MtArrayInstance(const std::string& name, const std::string& path) : MtInstance(name, path) {
}

//----------------------------------------

MtArrayInstance::~MtArrayInstance() {
}

//----------------------------------------

void MtArrayInstance::dump() {
  LOG_B("Array %s %s @ 0x%04X ", _name.c_str(), _path.c_str(), uint64_t(this) & 0xFFFF);
  dump_state(state_stack.back());
  LOG(" - %s\n", to_string(field_type));
  LOG_INDENT_SCOPE();
  dump_log();
}

//----------------------------------------

CHECK_RETURN Err MtArrayInstance::sanity_check() {
  return MtInstance::sanity_check();
}

//------------------------------------------------------------------------------

MtStructInstance::MtStructInstance(const std::string& name, const std::string& path, MtStruct* _struct)
: MtInstance(name, path), _struct(_struct)
{
  for (auto cf : _struct->fields) {
    _fields.push_back(field_to_inst(cf->_name, path + "." + cf->_name, cf));
  }

}

//----------------------------------------

MtStructInstance::~MtStructInstance() {
}

//----------------------------------------

void MtStructInstance::dump() {
  LOG_B("Struct %s %s @ 0x%04X ", _name.c_str(), _path.c_str(), uint64_t(this) & 0xFFFF);
  dump_state(state_stack.back());
  LOG("\n");
  LOG_INDENT_SCOPE();

  for (const auto& f : _fields) {
    LOG_G("%s: ", f->_name.c_str());
    f->dump();
  }
}

//----------------------------------------

CHECK_RETURN Err MtStructInstance::sanity_check() {
  return MtInstance::sanity_check();
}

//----------------------------------------

MtInstance* MtStructInstance::get_field(const std::string& name) {
  for (auto f : _fields) if (f->_name == name) return f;
  return nullptr;
}

//----------------------------------------

MtInstance* MtStructInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;

  auto result = get_field(path[index]);
  if (result) {
    return result->resolve(path, index + 1);
  }
  return nullptr;
}

//----------------------------------------

void MtStructInstance::reset_state() {
  for (auto f : _fields) f->reset_state();
}

//------------------------------------------------------------------------------


MtMethodInstance::MtMethodInstance(const std::string& name, const std::string& path, MtModuleInstance* module, MtMethod* method)
: MtInstance(name, path), _module(module), _method(method) {
  _name = method->name();
  for (auto c : _method->param_nodes) {
    _params.push_back(node_to_inst(c.name4(), path + "." + c.name4(), c, _method->_lib));
  }
  scope_stack.resize(1);
}

//----------------------------------------

MtMethodInstance::~MtMethodInstance() {
  for (auto p : _params) delete p;
  _params.clear();
}

//----------------------------------------

MtInstance* MtMethodInstance::get_param(const std::string& name) {
  for (auto p : _params) if (p->_name == name) return p;
  return nullptr;
}

//----------------------------------------

void MtMethodInstance::dump() {
  LOG_B("Method '%s' @ 0x%04X", _path.c_str(), uint64_t(this) & 0xFFFF);
  LOG(" - %s\n", to_string(_method_type));
  LOG_INDENT_SCOPE();
  for (auto p : _params) {
    LOG_G("%s: ", p->_name.c_str());
    p->dump();
  }
  if (_retval) _retval->dump();
}

//----------------------------------------

CHECK_RETURN Err MtMethodInstance::sanity_check() {
  Err err = MtInstance::sanity_check();

  LOG_R("MtMethodInstance::sanity_check() %s\n", _name.c_str());

  int sig_writes = 0;
  int reg_writes = 0;

  for (auto w : writes) {
    switch(w->state_stack.back()) {
      case CTX_NONE:     break;
      case CTX_INPUT:    break;
      case CTX_OUTPUT:   break;
      case CTX_MAYBE:    break;
      case CTX_SIGNAL:   break;
      case CTX_REGISTER: break;
      case CTX_INVALID:  break;
      case CTX_PENDING:  break;
    }
    //w->dump();
  }
  return err;
}

//----------------------------------------

MtInstance* MtMethodInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;

  auto param_inst = get_param(path[index]);
  if (param_inst) {
    return param_inst->resolve(path, index + 1);
  }

  return _module->resolve(path, index);
}

//----------------------------------------

void MtMethodInstance::reset_state() {
  for (auto p : _params) p->reset_state();
}

//------------------------------------------------------------------------------

MtModuleInstance::MtModuleInstance(const std::string& name, const std::string& path, MtModule* _mod)
: MtInstance(name, path) {
  assert(_mod);

  this->_mod = _mod;

  for (auto cf : _mod->all_fields) {
    _fields.push_back(field_to_inst(cf->name(), path + "." + cf->name(), cf));
  }

  for (auto m : _mod->all_methods) {
    if (m->is_constructor() || m->is_init_) continue;
    _methods.push_back(new MtMethodInstance(m->name(), path + "." + m->name(), this, m));
  }
}

//----------------------------------------

MtModuleInstance::~MtModuleInstance() {
  for (auto f : _fields) delete f;
  for (auto m : _methods) delete m;
}

//----------------------------------------

void MtModuleInstance::dump() {
  LOG_B("Module '%s' @ 0x%04X\n", _path.c_str(), uint64_t(this) & 0xFFFF);
  LOG_INDENT_SCOPE();
  for (auto f : _fields) {
    LOG_G("%s: ", f->_name.c_str());
    f->dump();
  }
  for (auto m : _methods) {
    LOG_G("%s: ", m->_name.c_str());
    m->dump();
  }
}

//----------------------------------------

CHECK_RETURN Err MtModuleInstance::sanity_check() {
  return MtInstance::sanity_check();
}

//----------------------------------------

void MtModuleInstance::reset_state() {
  for (auto f : _fields)  f->reset_state();
  for (auto m : _methods) m->reset_state();
}

//----------------------------------------

MtMethodInstance* MtModuleInstance::get_method(const std::string& name) {
  for (auto m : _methods) if (m->_name == name) return m;
  return nullptr;
}

//----------------------------------------

MtInstance* MtModuleInstance::get_field(const std::string& name) {
  for (auto f : _fields) if (f->_name == name) return f;
  return nullptr;
}

//----------------------------------------

MtInstance* MtModuleInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;
  if (auto f = get_field(path[index])) return f;
  if (auto m = get_method(path[index])) return m;
  return nullptr;
}

//------------------------------------------------------------------------------
