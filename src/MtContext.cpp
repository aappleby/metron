#include "MtContext.h"

#include "Log.h"
#include "MtMethod.h"
#include "MtModule.h"

//------------------------------------------------------------------------------

MtContext *MtContext::clone() {
  MtContext *result = new MtContext();
  result->type = type;
  result->parent = parent;
  result->name = name;
  result->field = field;
  result->method = method;
  result->mod = mod;
  result->state = state;

  for (auto c : children) {
    result->children.push_back(c->clone());
  }

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
    MtContext *result = new MtContext();
    result->type = CTX_FIELD;
    result->parent = parent;
    result->name = f->_name;
    result->field = f;
    result->mod = f->_type_mod;

    // printf("instantiate %s\n", f->cname());

    if (result->mod) {
      instantiate(result->mod, result);
    }

    parent->children.push_back(result);
  }

  for (auto m : mod->all_methods) {
    MtContext *method_ctx = new MtContext();
    method_ctx->type = CTX_METHOD;
    method_ctx->parent = parent;
    method_ctx->name = m->name();
    method_ctx->field = nullptr;
    method_ctx->method = m;
    method_ctx->mod = nullptr;

    parent->children.push_back(method_ctx);

    if (m->has_params()) {
      auto params =
          m->_node.get_field(field_declarator).get_field(field_parameters);
      for (const auto &param : params) {
        if (param.sym == sym_parameter_declaration) {
          MtContext *param_ctx = new MtContext();
          param_ctx->type = CTX_PARAM;
          param_ctx->parent = method_ctx;
          param_ctx->name = param.get_field(field_declarator).text();
          param_ctx->field = nullptr;
          param_ctx->method = nullptr;
          param_ctx->mod = nullptr;

          method_ctx->children.push_back(param_ctx);
        }
      }
    }

    if (m->has_return()) {
      MtContext *return_ctx = new MtContext();
      return_ctx->type = CTX_RETURN;
      return_ctx->parent = method_ctx;
      return_ctx->name = "<return>";
      return_ctx->field = nullptr;
      return_ctx->method = nullptr;
      return_ctx->mod = nullptr;

      method_ctx->children.push_back(return_ctx);
    }
  }
}

//------------------------------------------------------------------------------

void MtContext::assign_state_to_field() {
  if (field) {
    if (field->state == CTX_PENDING) {
      field->state = state;
    } else {
      field->state = merge_branch(field->state, state);
    }
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

  /*
  if (parent->type == CTX_MODULE) {
    return parent->resolve(_name);
  }
  */

  if (type == CTX_METHOD) {
    return parent->resolve(_name);
  }

  return result;
}

//------------------------------------------------------------------------------

void MtContext::dump() const {
  LOG_G("%s : ", name.c_str());

  switch (type) {
    case CTX_FIELD:
      if (field->_type_mod) {
        // LOG_Y("Submod %s = %s\n", field->_type_mod->cname(),
        // to_string(state));
        LOG_Y("Submod %s\n", field->_type_mod->cname());
      } else {
        LOG_G("%s = %s\n", field->_type.c_str(), to_string(state));
      }
      return;
    case CTX_METHOD:
      LOG_R("Method\n");
      return;
    case CTX_PARAM:
      LOG_B("Param = %s\n", to_string(state));
      return;
    case CTX_RETURN:
      LOG_G("Return = %s\n", to_string(state));
      return;
    case CTX_MODULE:
      LOG_G("Module\n");
      return;
  }
}

void MtContext::dump_tree() const {
  dump();
  for (auto inst : children) {
    LOG_INDENT_SCOPE();
    inst->dump_tree();
  }
}

//------------------------------------------------------------------------------
