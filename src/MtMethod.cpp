#include "MtMethod.h"

#include "Log.h"
#include "MtModule.h"

//------------------------------------------------------------------------------

MtMethod::MtMethod(MtModule* mod, MnNode n, bool is_public) {
  _mod = mod;
  _node = n;
  _public = is_public;
  _lib = _mod->lib;
  _name = n.name4();
  _type = n.get_field(field_type);

  auto params = n.get_field(field_declarator).get_field(field_parameters);
  for (const auto& param : params) {
    if (param.sym != sym_parameter_declaration) {
      has_params = true;
      break;
    }
  }

  has_return = _type.text() != "void";
}

//------------------------------------------------------------------------------

const char* MtMethod::cname() const { return _name.c_str(); }

std::string MtMethod::name() const { return _name; }

bool MtMethod::is_constructor() const { return _name == _mod->name(); }

bool MtMethod::categorized() const {
  return in_init || in_tick || in_tock || in_func;
}

bool MtMethod::is_valid() const {
  // A method must be only 1 of init/tick/tock/func
  return (int(in_init) + int(in_tick) + int(in_tock) + int(in_func)) == 1;
}

bool MtMethod::is_root() const { return callers.empty(); }

bool MtMethod::is_leaf() const {
  for (auto& m : callees) {
    if (!m->in_func) return false;
  }
  return true;
}

bool MtMethod::is_branch() const {
  for (auto& m : callees) {
    if (!m->in_func) return true;
  }
  return false;
}

//------------------------------------------------------------------------------

void MtMethod::dump() {
  if (is_constructor()) {
    LOG_B("Constructor %s\n", cname());
  } else {
    LOG_B("Method %s\n", cname());
  }

  for (auto p : param_nodes) {
    LOG_INDENT_SCOPE();
    LOG_R("Param %s\n", p.text().c_str());
  }

  for (auto c : callees) {
    LOG_INDENT_SCOPE();
    LOG_G("Calls %s.%s\n", c->_mod->cname(), c->cname());
  }
  for (auto c : callers) {
    LOG_INDENT_SCOPE();
    LOG_Y("Called by %s.%s\n", c->_mod->cname(), c->cname());
  }
}

//------------------------------------------------------------------------------
