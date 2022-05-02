#include "MtContext.h"

#include "Log.h"
#include "MtMethod.h"
#include "MtModule.h"

//------------------------------------------------------------------------------

MtContext::MtContext(MtModule *_top_mod) {
  assert(_top_mod);

  parent = nullptr;
  name = "<top>";
  type = CTX_MODULE;

  field = nullptr;
  method = nullptr;
  mod = _top_mod;

  log_top = {CTX_NONE};
  log_next = {CTX_NONE};
}

MtContext::MtContext(MtContext *_parent, MtMethod *_method) {
  assert(_parent);
  assert(_method);

  parent = _parent;
  name = _method->name();
  type = CTX_METHOD;

  field = nullptr;
  method = _method;
  mod = nullptr;

  log_top = {CTX_NONE};
  log_next = {CTX_NONE};
}

MtContext::MtContext(MtContext *_parent, MtField *_field) {
  assert(_parent);
  assert(_field);

  parent = _parent;
  name = _field->_name;

  if (_field->is_component()) {
    type = CTX_COMPONENT;
  } else {
    type = CTX_FIELD;
  }

  field = _field;
  method = nullptr;
  mod = _field->_type_mod;

  log_top = {CTX_NONE};
  log_next = {CTX_NONE};
}

MtContext *MtContext::param(MtContext *_parent, const std::string &_name) {
  assert(_parent);
  assert(_name.size());

  MtContext *param_ctx = new MtContext();
  param_ctx->parent = _parent;
  param_ctx->name = _name;
  param_ctx->type = CTX_PARAM;

  param_ctx->field = nullptr;
  param_ctx->method = nullptr;
  param_ctx->mod = nullptr;

  param_ctx->log_top = {CTX_NONE};
  param_ctx->log_next = {CTX_NONE};

  return param_ctx;
}

MtContext *MtContext::construct_return(MtContext *_parent) {
  assert(_parent);

  MtContext *return_ctx = new MtContext();
  return_ctx->parent = _parent;
  return_ctx->name = "<return>";
  return_ctx->type = CTX_RETURN;

  return_ctx->field = nullptr;
  return_ctx->method = nullptr;
  return_ctx->mod = nullptr;

  return_ctx->log_top = {CTX_NONE};
  return_ctx->log_next = {CTX_NONE};

  return return_ctx;
}

//------------------------------------------------------------------------------

MtContext *MtContext::clone() {
  MtContext *result = new MtContext();
  result->parent = parent;
  result->name = name;
  result->type = type;
  // result->state = state;

  result->field = field;
  result->method = method;
  result->mod = mod;

  for (auto c : children) {
    result->children.push_back(c->clone());
  }

  result->action_log = action_log;

  return result;
}

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

void MtContext::instantiate(MtModule *mod, MtContext *parent) {
  for (auto f : mod->all_fields) {
    MtContext *result = new MtContext(parent, f);

    if (result->mod) {
      instantiate(result->mod, result);
    }

    parent->children.push_back(result);
  }

  for (auto m : mod->all_methods) {
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

//------------------------------------------------------------------------------

void MtContext::assign_state_to_field() {
  if (field) {
    field->state = log_top.state;
  }

  for (auto c : children) c->assign_state_to_field();
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

  if (type == CTX_METHOD) {
    return parent->resolve(_name);
  }

  return result;
}

//------------------------------------------------------------------------------

void MtContext::dump() const {
  if (type == CTX_METHOD) {
    LOG_B("%s", name.c_str());

    LOG(" = ");
    if (log_top.state == CTX_SIGNAL || log_top.state == CTX_OUTPUT) {
      LOG_B("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_NONE) {
      LOG_Y("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_INVALID) {
      LOG_R("%s", to_string(log_top.state));
    } else {
      LOG_G("%s", to_string(log_top.state));
    }

  } else if (type == CTX_COMPONENT) {
    LOG_G("%s", name.c_str());
  } else if (type == CTX_FIELD) {
    LOG("%s", name.c_str());

    LOG(" = ");
    if (log_top.state == CTX_SIGNAL || log_top.state == CTX_OUTPUT) {
      LOG_B("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_NONE) {
      LOG_Y("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_INVALID) {
      LOG_R("%s", to_string(log_top.state));
    } else {
      LOG_G("%s", to_string(log_top.state));
    }
  } else if (type == CTX_PARAM) {
    LOG_W("%s", name.c_str());

    LOG(" = ");
    if (log_top.state == CTX_SIGNAL || log_top.state == CTX_OUTPUT) {
      LOG_B("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_NONE) {
      LOG_Y("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_INVALID) {
      LOG_R("%s", to_string(log_top.state));
    } else {
      LOG_G("%s", to_string(log_top.state));
    }
  } else if (type == CTX_RETURN) {
    LOG_W("%s", name.c_str());

    LOG(" = ");
    if (log_top.state == CTX_SIGNAL || log_top.state == CTX_OUTPUT) {
      LOG_B("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_NONE) {
      LOG_Y("%s", to_string(log_top.state));
    } else if (log_top.state == CTX_INVALID) {
      LOG_R("%s", to_string(log_top.state));
    } else {
      LOG_G("%s", to_string(log_top.state));
    }
  }
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
