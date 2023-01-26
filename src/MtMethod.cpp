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
    if (param.sym == sym_parameter_declaration ||
        param.sym == sym_optional_parameter_declaration) {
      param_nodes.push_back(param);
    }
  }
}

//------------------------------------------------------------------------------

MtMethod::~MtMethod() {
}

//------------------------------------------------------------------------------

const char* MtMethod::cname() const { return _name.c_str(); }

std::string MtMethod::name() const { return _name; }

bool MtMethod::is_constructor() const { return _name == _mod->name(); }

bool MtMethod::categorized() const {
  return is_init_ || is_tick_ || is_tock_ || is_func_;
}

bool MtMethod::is_valid() const {
  // A method must be only 1 of init/tick/tock/func
  return (int(is_init_) + int(is_tick_) + int(is_tock_) + int(is_func_)) == 1;
}

bool MtMethod::is_public() const { return _public; }

//------------------------------------------------------------------------------

void MtMethod::dump() {
  if (is_constructor()) {
    LOG_C(0xFF0088, "@ Constructor %s\n", cname());
  } else {
    if (is_init_) {
      LOG_C(0xBBBBBB, "# Init %s\n", cname());
    }
    else if (is_func_) {
      LOG_C(0xFF8888, "@ Func %s\n", cname());
    }
    else if (is_tick_) {
      LOG_C(0x88FF88, "^ Tick %s\n", cname());
    }
    else if (is_tock_) {
      LOG_C(0x8888FF, "v Tock %s\n", cname());
    }
    else {
      LOG_C(0x0000FF, "X Unknown %s\n", cname());
    }
  }

  for (auto p : param_nodes) {
    LOG_INDENT_SCOPE();
    LOG_R("Param %s\n", p.text().c_str());
  }

  for (auto c : internal_callees) {
    LOG_INDENT_SCOPE();
    LOG_G("Calls this->%s\n", c->cname());
  }

  for (auto c : internal_callers) {
    LOG_INDENT_SCOPE();
    LOG_Y("Called by tick %s\n", c->cname());
  }

  for (auto c : external_callees) {
    LOG_INDENT_SCOPE();
    LOG_G("Calls %s.%s\n", c->_mod->cname(), c->cname());
  }
  for (auto c : external_callers) {
    LOG_INDENT_SCOPE();
    LOG_Y("Called by %s.%s\n", c->_mod->cname(), c->cname());
  }
}

//------------------------------------------------------------------------------

bool MtMethod::has_params() const { return !param_nodes.empty(); }
bool MtMethod::has_return() const { return !_type.is_null() && _type.text() != "void"; }

bool MtMethod::has_param(const std::string& name) {
  for (const auto& p : param_nodes) {
    if (p.name4() == name) return true;
  }
  return false;
}

bool MtMethod::called_in_module() const {
  return !internal_callers.empty();
}

bool MtMethod::called_in_init() const {
  for (auto m : internal_callers) {
    if (m->is_constructor()) return true;
    if (m->called_in_init()) return true;
  }
  return false;
}

bool MtMethod::called_in_tick() const {
  for (auto m : internal_callers) {
    if (m->is_tick_) return true;
    if (m->called_in_tick()) return true;
  }
  return false;
}

bool MtMethod::called_in_tock() const {
  for (auto m : internal_callers) {
    if (m->is_tock_) return true;
    if (m->called_in_tock()) return true;
  }
  return false;
}

bool MtMethod::called_by_tock() const {
  for (auto m : internal_callers) {
    if (m->is_tock_) return true;
  }
  return false;
}

bool MtMethod::called_in_func() const {
  for (auto m : internal_callers) {
    if (m->is_tock_) return true;
    if (m->called_in_tock()) return true;
  }
  return false;
}
