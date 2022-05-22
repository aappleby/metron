#include "MtContext.h"

#include "Log.h"
#include "MtMethod.h"
#include "MtModule.h"
#include "MtStruct.h"

//------------------------------------------------------------------------------

MtContext::MtContext(MtModule *_top_mod) {
  assert(_top_mod);

  name = "<top>";
  context_type = CTX_MODULE;
  parent = nullptr;

  field = nullptr;
  method = nullptr;

  parent_mod = _top_mod;
  parent_struct = nullptr;

  type_mod = _top_mod;
  type_struct = nullptr;

  log_top = {CTX_NONE};
  log_next = {CTX_NONE};
}

MtContext::MtContext(MtContext *_parent, MtMethod *_method) {
  assert(_parent);
  assert(_method);

  name = _method->name();
  context_type = CTX_METHOD;
  parent = _parent;

  field = nullptr;
  method = _method;

  parent_mod = _method->_mod;
  parent_struct = nullptr;

  type_mod = nullptr;
  type_struct = nullptr;

  log_top = {CTX_NONE};
  log_next = {CTX_NONE};
}

MtContext::MtContext(MtContext *_parent, MtField *_field) {
  assert(_parent);
  assert(_field);

  name = _field->_name;

  if (_field->is_component()) {
    context_type = CTX_COMPONENT;
  } else if (_field->is_struct()) {
    context_type = CTX_FIELD;
  } else {
    context_type = CTX_FIELD;
  }

  parent = _parent;

  field = _field;
  method = nullptr;

  parent_mod = _field->_parent_mod;
  parent_struct = _field->_parent_struct;

  type_mod = _field->_type_mod;
  type_struct = _field->_type_struct;

  log_top = {CTX_NONE};
  log_next = {CTX_NONE};
}

//------------------------------------------------------------------------------

MtContext::~MtContext() {
  for (auto c : children) delete c;
  children.clear();
}

//------------------------------------------------------------------------------

MtContext *MtContext::param(MtContext *_parent, const std::string &_name) {
  assert(_parent);
  assert(_name.size());

  MtContext *param_ctx = new MtContext();

  param_ctx->name = _name;
  param_ctx->context_type = CTX_PARAM;
  param_ctx->parent = _parent;

  param_ctx->field = nullptr;
  param_ctx->method = nullptr;

  param_ctx->parent_mod = nullptr;
  param_ctx->parent_struct = nullptr;

  param_ctx->type_mod = nullptr;
  param_ctx->type_struct = nullptr;

  param_ctx->log_top = {CTX_NONE};
  param_ctx->log_next = {CTX_NONE};

  return param_ctx;
}

MtContext *MtContext::construct_return(MtContext *_parent) {
  assert(_parent);

  MtContext *return_ctx = new MtContext();

  return_ctx->name = "<return>";
  return_ctx->context_type = CTX_RETURN;
  return_ctx->parent = _parent;

  return_ctx->field = nullptr;
  return_ctx->method = nullptr;

  return_ctx->parent_mod = nullptr;
  return_ctx->parent_struct = nullptr;

  return_ctx->type_mod = nullptr;
  return_ctx->type_struct = nullptr;

  return_ctx->log_top = {CTX_NONE};
  return_ctx->log_next = {CTX_NONE};

  return return_ctx;
}

//------------------------------------------------------------------------------

/*
MtContext *MtContext::clone() {
  MtContext *result = new MtContext();

  result->name = name;
  result->context_type = context_type;
  result->parent = parent;

  result->field = field;
  result->method = method;

  result->parent_mod = parent_mod;
  result->parent_struct = parent_struct;

  result->type_mod = type_mod;
  result->type_struct = type_struct;

  for (auto c : children) {
    result->children.push_back(c->clone());
  }

  result->action_log = action_log;

  return result;
}
*/

//------------------------------------------------------------------------------

std::string MtContext::get_path() const {
  if (parent) {
    return parent->get_path() + "." + name;
  } else {
    return name;
  }
}

MtContext *MtContext::get_child(const std::string &name) const {
  for (auto c : children)
    if (c->name == name) return c;
  return nullptr;
}

//------------------------------------------------------------------------------

void MtContext::instantiate() {
  if (type_mod) {
    for (auto f : type_mod->all_fields) {
      MtContext *result = new MtContext(this, f);
      children.push_back(result);
    }

    for (auto m : type_mod->all_methods) {
      MtContext *method_ctx = new MtContext(this, m);
      children.push_back(method_ctx);
    }
  }

  if (method) {
    if (method->has_params()) {
      auto params = method->_node.get_field(field_declarator).get_field(field_parameters);
      for (const auto &param : params) {
        if (param.sym == sym_parameter_declaration) {
          MtContext *param_ctx = MtContext::param(this, param.get_field(field_declarator).text());
          children.push_back(param_ctx);
        }
      }
    }

    if (method->has_return()) {
      MtContext *return_ctx = MtContext::construct_return(this);
      children.push_back(return_ctx);
    }
  }

  if (type_struct) {
    for (auto f : type_struct->fields) {
      MtContext *result = new MtContext(this, f);
      children.push_back(result);
    }
  }

  for (auto c : children) {
    c->instantiate();
  }
}

/*
void MtContext::instantiate(MtModule *_mod, MtContext *parent) {
  for (auto f : _mod->all_fields) {
    MtContext *result = new MtContext(parent, f);

    if (result->mod) {
      instantiate(result->mod, result);
    }

    if (result->_struct) {
      instantiate(result->_struct, result);
    }

    parent->children.push_back(result);
  }

  for (auto m : _mod->all_methods) {
    MtContext *method_ctx = new MtContext(parent, m);

    parent->children.push_back(method_ctx);

    if (m->has_params()) {
      auto params =
          m->_node.get_field(field_declarator).get_field(field_parameters);
      for (const auto &param : params) {
        if (param.sym == sym_parameter_declaration) {
          MtContext *param_ctx = MtContext::param(
              method_ctx, param.get_field(field_declarator).text());
          method_ctx->children.push_back(param_ctx);
        }
      }
    }

    if (m->has_return()) {
      MtContext *return_ctx = MtContext::construct_return(method_ctx);
      method_ctx->children.push_back(return_ctx);
    }
  }
}
*/

//------------------------------------------------------------------------------

/*
void MtContext::instantiate(MtStruct *_struct, MtContext *parent) {
  for (auto f : _struct->fields) {
    MtContext *result = new MtContext(parent, f);

    if (result->_type_struct) {
      instantiate(result->mod, result);
    }

    parent->children.push_back(result);
  }
}
*/

//------------------------------------------------------------------------------

void MtContext::assign_struct_states() {
  if (type_struct) {
    log_top.state = children[0]->log_top.state;
    for (auto c : children) {
      c->assign_struct_states();
      auto result = merge_branch(log_top.state, c->log_top.state);
      log_top.state = result;
    }
  }

  for (auto c : children) c->assign_struct_states();
}

//------------------------------------------------------------------------------

void MtContext::assign_state_to_field(MtModule* current_module) {
  if (parent_mod != current_module) {
    return;
  }
  if (field) {
    field->_state = log_top.state;
  }
  for (auto c : children) {
    c->assign_state_to_field(current_module);
  }
}

//------------------------------------------------------------------------------

MtContext *MtContext::resolve(const std::string &_name) {
  MtContext *result = nullptr;
  if (name == _name) {
    assert(false);
    return this;
  }

  for (auto c : children) {
    if (c->name == _name) {
      return c;
    }
  }

  if (context_type == CTX_METHOD) {
    return parent->resolve(_name);
  }

  return result;
}

MtContext *MtContext::resolve(MnNode node) {

  if (node.sym == sym_identifier || node.sym == alias_sym_field_identifier) {
    return resolve(node.text());
  }
  else if (node.sym == sym_field_expression) {
    auto lhs = node.get_field(field_argument);
    auto lhs_context = resolve(lhs);
    if (lhs_context) {
      auto rhs = node.get_field(field_field);
      return lhs_context->resolve(rhs);
    }
    else {
      return nullptr;
    }
  }
  else {
    return nullptr;
  }
}

//------------------------------------------------------------------------------

void log_state(ContextState s) {
  if (s == CTX_SIGNAL || s == CTX_OUTPUT) {
    LOG_B("%s", to_string(s));
  } else if (s == CTX_NONE) {
    LOG_Y("%s", to_string(s));
  } else if (s == CTX_INVALID) {
    LOG_R("%s", to_string(s));
  } else {
    LOG_G("%s", to_string(s));
  }
}

void MtContext::dump() const {
  if (context_type == CTX_METHOD) {
    LOG("{Method} %s = ", name.c_str());
  } else if (context_type == CTX_COMPONENT) {
    LOG("{Component} %s = ", name.c_str());
  } else if (context_type == CTX_FIELD) {
    LOG("{Field} %s = ", name.c_str());
  } else if (context_type == CTX_PARAM) {
    LOG("{Param} %s = ", name.c_str());
  } else if (context_type == CTX_RETURN) {
    LOG("{Return} %s = ", name.c_str());
  } else if (context_type == CTX_MODULE) {
    LOG("{Module} %s = ", name.c_str());
  }
  log_state(state());
  LOG("\n");
}

void MtContext::dump_ctx_tree() const {
  dump();
  for (auto inst : children) {
    LOG_INDENT_SCOPE();
    inst->dump_ctx_tree();
  }
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtContext::check_done() {
  Err err;
  if (action_log.size()) {
    err << ERR("Had leftover contexts in action_log\n");
  }

  if (log_next.state != CTX_NONE) {
    err << ERR("Had leftover context in log_next\n");
  }

  if (log_top.state == CTX_INVALID) {
    err << ERR("Context %s had invalid state after trace\n", name.c_str());
  }

  if (log_top.state == CTX_PENDING) {
    err << ERR("Context %s had pending state after trace\n", name.c_str());
  }

  for (auto c : children) err << c->check_done();
  return err;
}
