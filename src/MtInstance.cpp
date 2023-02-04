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

MtFieldInstance* field_to_inst(const std::string& name, const std::string& path, MtField* field) {

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






//==============================================================================
// MtInstance
//==============================================================================

MtInstance::MtInstance(const std::string& name, const std::string& path)
: _name(name), _path(path) {
  state_stack = {CTX_NONE};
}

//----------------------------------------

MtInstance::~MtInstance() {
}

//----------------------------------------

CHECK_RETURN Err MtInstance::sanity_check() {
  //LOG_B("MtInstance::sanity_check() %s\n", _path.c_str());
  Err err;
  if (state_stack.size() > 1) {
    err << ERR("Had more than one item in the state stack after trace\n");
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtInstance::assign_types() {
  return Err::ok;
}

//----------------------------------------

CHECK_RETURN Err MtInstance::log_action(MnNode node, TraceAction action) {
  Err err;
  auto old_state = state_stack.back();
  auto new_state = merge_action(old_state, action);

  if (new_state == CTX_INVALID) {
    LOG_R("Trace error: state went from %s to %s\n", to_string(old_state), to_string(new_state));
    dump();
    err << ERR("Invalid context state\n");
  }

  if (old_state != new_state) {
    action_log.push_back({old_state, new_state, action, node});
  }

  state_stack.back() = new_state;

  return err;
}

//----------------------------------------

void MtInstance::dump() {
  LOG_R("MtInstance::dump()\n");
  dump_log();
}

void MtInstance::dump_log() {
  //::dump_log(action_log);
}

//----------------------------------------

void MtInstance::reset_state() {
  state_stack = {CTX_NONE};
  action_log.clear();
}

//----------------------------------------

void MtInstance::visit(const inst_visitor& v) {
  v(this);
}

//----------------------------------------

MtInstance* MtInstance::resolve(const std::vector<std::string>& path, int index) {
  return (index == path.size()) ? this : nullptr;
}

//----------------------------------------

CHECK_RETURN Err MtInstance::merge_with_source() {
  return ERR("Should never get here");
}





//==============================================================================
// MtFieldInstance
//==============================================================================

MtFieldInstance::MtFieldInstance(const std::string& name, const std::string& path) : MtInstance(name, path)
{
}

//----------------------------------------

MtFieldInstance::~MtFieldInstance() {
}

//----------------------------------------

CHECK_RETURN Err MtFieldInstance::sanity_check() {
  //LOG_B("MtFieldInstance::sanity_check() %s\n", _path.c_str());
  if (_field_type == FT_UNKNOWN) return ERR("MtFieldInstance::sanity_check() - Field type is unknown");
  return Err::ok;
}

//----------------------------------------

void MtFieldInstance::reset_state() {
  _field_type = FT_UNKNOWN;
}

//----------------------------------------

FieldType MtFieldInstance::get_field_type() const {
  return _field_type;
}

//----------------------------------------

CHECK_RETURN Err MtFieldInstance::set_field_type(FieldType f) {
  Err err;

  if (_field_type != FT_UNKNOWN && _field_type != f) {
    return ERR("MtFieldInstance::set_field_type - mismatched types old %s new %s\n", to_string(_field_type), to_string(f));
  }

  _field_type = f;
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtFieldInstance::merge_with_source() {
  Err err;
  return err;
}





//==============================================================================
// MtPrimitiveInstance
//==============================================================================

MtPrimitiveInstance::MtPrimitiveInstance(const std::string& name, const std::string& path)
: MtFieldInstance(name, path) {
}

//----------------------------------------

MtPrimitiveInstance::~MtPrimitiveInstance() {
}

//----------------------------------------

CHECK_RETURN Err MtPrimitiveInstance::sanity_check() {
  LOG_B("MtPrimitiveInstance::sanity_check() %s\n", _path.c_str());
  LOG_INDENT_SCOPE();
  return MtInstance::sanity_check();
}

//----------------------------------------

CHECK_RETURN Err MtPrimitiveInstance::assign_types() {
  Err err;
  err << set_field_type(trace_state_to_field_type(state_stack.back()));
  return Err::ok;
}

//----------------------------------------

void MtPrimitiveInstance::dump() {
  LOG_B("Primitive '%s' @ 0x%04X ", _path.c_str(), uint64_t(this) & 0xFFFF);
  //dump_state(state_stack.back());
  LOG(" - %s\n", to_string(get_field_type()));
  LOG_INDENT_SCOPE();
  dump_log();
  assert(state_stack.size() == 1);
}

//----------------------------------------

CHECK_RETURN Err MtPrimitiveInstance::merge_with_source() {
  Err err;
  return err;
}





//==============================================================================
// MtArrayInstance
//==============================================================================

MtArrayInstance::MtArrayInstance(const std::string& name, const std::string& path)
: MtFieldInstance(name, path) {
}

//----------------------------------------

MtArrayInstance::~MtArrayInstance() {
}

//----------------------------------------

void MtArrayInstance::dump() {
  LOG_B("Array %s %s @ 0x%04X ", _name.c_str(), _path.c_str(), uint64_t(this) & 0xFFFF);
  //dump_state(state_stack.back());
  LOG(" - %s\n", to_string(get_field_type()));
  LOG_INDENT_SCOPE();
  dump_log();
}

//----------------------------------------

CHECK_RETURN Err MtArrayInstance::sanity_check() {
  LOG_B("MtArrayInstance::sanity_check() %s\n", _path.c_str());
  LOG_INDENT_SCOPE();
  return MtFieldInstance::sanity_check();
}

//----------------------------------------

CHECK_RETURN Err MtArrayInstance::assign_types() {
  Err err;
  err << set_field_type(trace_state_to_field_type(state_stack.back()));
  return Err::ok;
}

//----------------------------------------

CHECK_RETURN Err MtArrayInstance::merge_with_source() {
  Err err;
  return err;
}





//==============================================================================
// MtStructInstance
//==============================================================================

MtStructInstance::MtStructInstance(const std::string& name, const std::string& path, MtStruct* _struct)
: MtFieldInstance(name, path), _struct(_struct)
{
  for (auto cf : _struct->fields) {
    _fields.push_back(field_to_inst(cf->_name, path + "." + cf->_name, cf));
  }
}

//----------------------------------------

MtStructInstance::~MtStructInstance() {
}

//----------------------------------------

CHECK_RETURN Err MtStructInstance::sanity_check() {
  LOG_B("MtStructInstance::sanity_check() %s\n", _path.c_str());
  LOG_INDENT_SCOPE();
  Err err = MtInstance::sanity_check();
  for (auto f : _fields) err << f->sanity_check();
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtStructInstance::assign_types() {
  /*
  if (_field_type != FT_UNKNOWN) return Err::ok;


  auto state = _fields[0]->state_stack.back();
  for (int i = 1; i < _fields.size(); i++) {
  }

  for (auto f : _fields) {
    err << set_field_type(f->get_field_type());
  }
  */

  Err err;

  for (auto f : _fields) {
    err << f->assign_types();
  }

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtStructInstance::log_action(MnNode node, TraceAction action) {
  Err err;

  for (auto f : _fields) {
    err << f->log_action(node, action);
  }

  return err;
}

//----------------------------------------

void MtStructInstance::dump() {
  LOG_B("Struct %s %s @ 0x%04X ", _name.c_str(), _path.c_str(), uint64_t(this) & 0xFFFF);
  //dump_state(state_stack.back());
  LOG(" - %s\n", to_string(get_field_type()));
  LOG_INDENT_SCOPE();

  for (const auto& f : _fields) {
    LOG_G("%s: ", f->_name.c_str());
    f->dump();
  }
}

//----------------------------------------

void MtStructInstance::reset_state() {
  for (auto f : _fields) f->reset_state();
}

//----------------------------------------

void MtStructInstance::visit(const inst_visitor& v) {
  MtInstance::visit(v);
  for (auto& f : _fields) v(f);
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

FieldType MtStructInstance::get_field_type() const {
  auto state = _fields[0]->state_stack.back();
  for (int i = 1; i < _fields.size(); i++) {
    state = merge_branch(state, _fields[i]->state_stack.back());
  }
  return trace_state_to_field_type(state);
}

//----------------------------------------

CHECK_RETURN Err MtStructInstance::set_field_type(FieldType f) {
  return ERR("Should not get here");
}

//----------------------------------------

MtInstance* MtStructInstance::get_field(const std::string& name) {
  for (auto f : _fields) if (f->_name == name) return f;
  return nullptr;
}

//----------------------------------------

CHECK_RETURN Err MtStructInstance::merge_with_source() {
  Err err;
  for (auto& f : _fields)  err << f->merge_with_source();
  return err;
}







//==============================================================================
// MtMethodInstance
//==============================================================================

MtMethodInstance::MtMethodInstance(const std::string& name, const std::string& path, MtModuleInstance* module, MtMethod* method)
: MtInstance(name, path), _module(module), _method(method) {
  _name = method->name();
  for (auto c : _method->param_nodes) {
    _params.push_back(node_to_inst(c.name4(), path + "." + c.name4(), c, _method->_lib));
  }
  _scope_stack.resize(1);
}

//----------------------------------------

MtMethodInstance::~MtMethodInstance() {
  for (auto p : _params) delete p;
  _params.clear();
}

//----------------------------------------

void MtMethodInstance::visit(const inst_visitor& v) {
  MtInstance::visit(v);
  for (auto p : _params) {
    p->visit(v);
  }
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
  LOG_B("MtMethodInstance::sanity_check() %s\n", _path.c_str());
  LOG_INDENT_SCOPE();

  if (_method_type == MT_UNKNOWN) return ERR("MtMethodInstance::sanity_check() - Method type is unknown - %s", _path.c_str());

  Err err = MtInstance::sanity_check();

  for (auto p : _params) err << p->sanity_check();

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtMethodInstance::assign_types() {
  if (_method_type != MT_UNKNOWN) return Err::ok;
  LOG_G("Assigning type to %s\n", _path.c_str());

  Err err;
  for (auto p : _params) err << p->assign_types();
  for (auto w : _writes) err << w->assign_types();
  for (auto r : _reads)  err << r->assign_types();
  for (auto c : _calls)  err << c->assign_types();

  if (_name.starts_with("tick")) {
    err << set_method_type(MT_TICK);
    return err;
  }
  else if (_name.starts_with("tock")) {
    err << set_method_type(MT_TOCK);
    return err;
  }

  int sig_writes = 0;
  int reg_writes = 0;

  for (auto w : _writes) {
    switch(w->state_stack.back()) {
      case CTX_NONE:     break;
      case CTX_INPUT:    break;
      case CTX_OUTPUT:   break;
      case CTX_MAYBE:    break;
      case CTX_SIGNAL:   sig_writes++; break;
      case CTX_REGISTER: reg_writes++; break;
      case CTX_INVALID:  break;
      case CTX_PENDING:  break;
    }
    //w->dump();
  }

  if (sig_writes != 0 && reg_writes != 0) {
    // Interesting, the old tracer didn't catch this?
    return ERR("Method wrote both signals and registers\n");
  }
  else if (sig_writes) {
    err << set_method_type(MT_TOCK);
    return err;
  }
  else if (reg_writes) {
    err << set_method_type(MT_TICK);
    return err;
  }
  else {
    // If it calls a tock, it has to be a tock.

    for (auto c : _calls) {
      if (c->get_method_type() == MT_TOCK) {
        err << set_method_type(MT_TOCK);
        return err;
      }
    }

    for (auto c : _calls) {
      if (c->get_method_type() == MT_TICK) {
        err << set_method_type(MT_TICK);
        return err;
      }
    }
    //_method_type = MT_FUNC;
  }

  // We didn't write anything, we didn't call anything - must be a function.

  err << set_method_type(MT_FUNC);
  return err;

  //return ERR("Could not assign type to method %s\n", _path.c_str());
}

//----------------------------------------

MtInstance* MtMethodInstance::get_param(const std::string& name) {
  for (auto p : _params) if (p->_name == name) return p;
  return nullptr;
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

//----------------------------------------

CHECK_RETURN Err MtMethodInstance::merge_with_source() {
  Err err;

  /*
  bool is_init_ = false;
  bool is_tick_ = false;
  bool is_tock_ = false;
  bool is_func_ = false;

  bool emit_as_always_comb = false;
  bool emit_as_always_ff = false;
  bool emit_as_init = false;
  bool emit_as_task = false;
  bool emit_as_func = false;
  bool needs_binding = false;
  bool needs_ports = false;
  */

  for (auto& p : _params)  err << p->merge_with_source();

  LOG_B("%s is_init_ %d\n", _name.c_str(), _method->is_init_);
  LOG_B("%s is_tick_ %d\n", _name.c_str(), _method->is_tick_);
  LOG_B("%s is_tock_ %d\n", _name.c_str(), _method->is_tock_);
  LOG_B("%s is_func_ %d\n", _name.c_str(), _method->is_func_);

  if ((_method->is_init_ + _method->is_tick_ + _method->is_tock_ + _method->is_func_) > 1) {
    return ERR("Too many types");
  }

  // This one breaks
  //if (_method_type == MT_TICK && !_method->is_tick_) return ERR("Tick mismatch");

  // This one is ok
  if (_method_type == MT_TOCK && !_method->is_tock_) return ERR("Tock mismatch");

  //if (_method_type == MT_FUNC && !_method->is_func_) return ERR("Func mismatch");


  return err;
}





//==============================================================================
// MtModuleInstance
//==============================================================================

MtModuleInstance::MtModuleInstance(const std::string& name, const std::string& path, MtModule* _mod)
: MtFieldInstance(name, path) {
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

CHECK_RETURN Err MtModuleInstance::sanity_check() {
  LOG_B("MtModuleInstance::sanity_check() %s\n", _path.c_str());
  LOG_INDENT_SCOPE();
  Err err = MtFieldInstance::sanity_check();
  for (auto f : _fields)  err << f->sanity_check();
  for (auto m : _methods) err << m->sanity_check();
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtModuleInstance::assign_types() {
  Err err = MtFieldInstance::set_field_type(FT_MODULE);
  for (auto f : _fields)  err << f->assign_types();
  for (auto m : _methods) err << m->assign_types();
  return err;
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

void MtModuleInstance::reset_state() {
  MtFieldInstance::reset_state();
  for (auto f : _fields)  f->reset_state();
  for (auto m : _methods) m->reset_state();
}

//----------------------------------------

void MtModuleInstance::visit(const inst_visitor& v) {
  MtInstance::visit(v);
  for (auto& f : _fields)  f->visit(v);
  for (auto& m : _methods) m->visit(v);
}

//----------------------------------------

MtInstance* MtModuleInstance::resolve(const std::vector<std::string>& path, int index) {
  if (index == path.size()) return this;
  if (auto f = get_field(path[index])) return f;
  if (auto m = get_method(path[index])) return m;
  return nullptr;
}

//----------------------------------------

CHECK_RETURN Err MtModuleInstance::merge_with_source() {
  Err err;
  for (auto& f : _fields)  err << f->merge_with_source();
  for (auto& m : _methods) err << m->merge_with_source();
  return err;
}

//----------------------------------------

FieldType MtModuleInstance::get_field_type() const {
  return MtFieldInstance::get_field_type();
}

//----------------------------------------

CHECK_RETURN Err MtModuleInstance::set_field_type(FieldType f) {
  return ERR("Should not get here");
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

//------------------------------------------------------------------------------
