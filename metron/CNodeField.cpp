#include "CNodeField.hpp"

#include "NodeTypes.hpp"
#include "CInstance.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

void CNodeField::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_decl = child("decl")->req<CNodeDeclaration>();
  node_semi = child("semi")->req<CNodePunct>();
}

//------------------------------------------------------------------------------

uint32_t CNodeField::debug_color() const {
  return COL_PINK;
}

std::string_view CNodeField::get_name() const {
  return node_decl->get_name();
}

std::string_view CNodeField::get_type_name() const {
  return node_decl->get_type_name();
}

//------------------------------------------------------------------------------

bool CNodeField::is_component() const {
  return node_decl->_type_class != nullptr;
}

bool CNodeField::is_struct() const {
  return node_decl->_type_struct != nullptr;
}

bool CNodeField::is_array() const {
  return node_decl->node_array != nullptr;
}

bool CNodeField::is_const_char() const {
  if (node_decl->node_static && node_decl->node_const) {
    auto builtin = node_decl->node_type->child("name");
    auto star    = node_decl->node_type->child("star");
    if (builtin && star && builtin->get_text() == "char") {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

Err CNodeField::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  //----------------------------------------
  // Ports don't go in the class body.
  // FIXME should we disallow public components?

  if (is_public && !is_component() && !node_decl->is_localparam()) {
    return err << cursor.skip_over(this);
  }

  //----------------------------------------

  if (is_const_char()) {
    err << cursor.emit_print("localparam string ");
    err << cursor.skip_to(node_decl->node_name);
    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap();

    if (node_decl->node_array) {
      err << cursor.emit(node_decl->node_array);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node_decl->node_eq);
    err << cursor.emit_gap();
    err << cursor.emit(node_decl->node_value);
    err << cursor.emit_gap();

     err << cursor.emit(node_semi);

    return err << cursor.check_done(this);
  }

  //----------------------------------------

  if (node_decl->node_const) {
    // Localparam
    bool in_namespace = ancestor<CNodeNamespace>() != nullptr;
    err << cursor.emit_print(in_namespace ? "parameter " : "localparam ");

    if (node_decl->node_static) {
      err << cursor.comment_out(node_decl->node_static);
      err << cursor.emit_gap();
    }

    if (node_decl->node_const) {
      err << cursor.comment_out(node_decl->node_const);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node_decl->node_type);
    err << cursor.emit_gap();

    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap();

    if (node_decl->node_array) {
      err << cursor.emit(node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node_decl->node_eq) {
      err << cursor.emit(node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node_decl->node_value) {
      err << cursor.emit(node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node_semi);
    return err << cursor.check_done(this);
  }

  //----------------------------------------

  auto node_builtin = node_decl->node_type->as<CNodeBuiltinType>();
  auto node_targs   = node_decl->node_type->child("template_args");

  if (node_builtin && node_targs) {
    err << cursor.skip_to(node_decl->node_type);
    err << cursor.emit(node_decl->node_type);
    err << cursor.emit_gap();

    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap();

    if (node_decl->node_array) {
      err << cursor.emit(node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node_decl->node_eq) {
      err << cursor.emit(node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node_decl->node_value) {
      err << cursor.emit(node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node_semi);

    return err << cursor.check_done(this);
  }

  //----------------------------------------

  if (node_builtin) {
    err << cursor.skip_to(node_decl->node_type);
    err << cursor.emit(node_decl->node_type);
    err << cursor.emit_gap();

    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap();

    if (node_decl->node_array) {
      err << cursor.emit(node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node_decl->node_eq) {
      err << cursor.emit(node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node_decl->node_value) {
      err << cursor.emit(node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node_semi);
    return err << cursor.check_done(this);
  }

  //----------------------------------------

  if (is_struct()) {
    return cursor.emit_raw(this);
  }

  //----------------------------------------

  if (is_component()) {
    return emit_component(cursor);
  }

  //----------------------------------------

  err << CNode::emit(cursor);
  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeField::emit_component(Cursor& cursor) {
  Err err;

  err << cursor.skip_to(node_decl->node_type);
  err << cursor.emit(node_decl->node_type);
  err << cursor.emit_gap();
  err << cursor.skip_over(node_decl->node_name);
  err << cursor.skip_gap();

  auto parent_class = ancestor<CNodeClass>();
  auto repo = parent_class->repo;
  auto component_class = repo->get_class(node_decl->node_type->get_name());
  auto component_template = component_class->node_parent->as<CNodeTemplate>();

  //----------------------------------------
  // Component modparams

  bool has_constructor_params = component_class->constructor && component_class->constructor->params.size();
  bool has_template_params = component_template != nullptr;

  if (has_template_params || has_constructor_params) {
    err << cursor.emit_print("#(");
    cursor.indent_level++;

    // Emit template arguments as module parameters
    if (has_template_params) {
      err << cursor.start_line();
      err << cursor.emit_print("// Template Parameters");

      auto args = node_decl->node_type->child("template_args")->as<CNodeList>();

      int param_count = component_template->params.size();
      int arg_count = args->items.size();
      assert(param_count == arg_count);

      for (int i = 0; i < param_count; i++) {
        auto param = component_template->params[i];
        auto arg = args->items[i];

        auto param_name = param->get_name();

        err << cursor.start_line();
        err << cursor.emit_print(".%.*s(", param_name.size(), param_name.data());
        err << cursor.emit_splice(arg);
        err << cursor.emit_print("),");
      }
    }

    // Emit constructor arguments as module parameters
    if (has_constructor_params) {
      err << cursor.start_line();
      err << cursor.emit_print("// Constructor Parameters");

      auto params = component_class->constructor->params;

      // Find the initializer node IN THE PARENT INIT LIST for the component
      // and extract arguments
      assert(parent_class->constructor->node_init);
      CNodeList* args = nullptr;
      for (auto init : parent_class->constructor->node_init->items) {
        if (init->child("name")->get_text() == node_decl->node_name->get_text()) {
          args = init->child("value")->as<CNodeList>();
        }
      }

      assert(params.size() == args->items.size());

      for (auto i = 0; i < params.size(); i++) {
        auto param = params[i];
        auto arg = args->items[i];
        auto param_name = param->get_name();

        err << cursor.start_line();
        err << cursor.emit_print(".%.*s(", param_name.size(), param_name.data());
        err << cursor.emit_splice(arg);
        err << cursor.emit_print("),");
      }
    }

    // Remove trailing comma from port list
    if (cursor.at_comma) {
      err << cursor.emit_backspace();
    }

    cursor.indent_level--;
    err << cursor.start_line();
    err << cursor.emit_print(") ");
  }

  //----------------------------------------
  // Component name

  //err << cursor.emit_print(" ");
  err << cursor.emit_splice(node_decl->node_name);

  //----------------------------------------
  // Port list

  err << cursor.emit_print("(");
  cursor.indent_level++;

  auto field_name = get_namestr();

  if (component_class->needs_tick()) {
    err << cursor.start_line();
    err << cursor.emit_print("// Global clock");
    err << cursor.start_line();
    err << cursor.emit_print(".clock(clock),");
  }

  if (component_class->input_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// Input signals");
    for (auto f : component_class->input_signals) {
      auto port_name = f->get_namestr();

      err << cursor.start_line();
      err << cursor.emit_print(".%s", port_name.c_str());
      err << cursor.emit_print("(");
      err << cursor.emit_print("%s_%s", field_name.c_str(), port_name.c_str());
      err << cursor.emit_print("),");
    }
  }

  if (component_class->output_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// Output signals");
    for (auto f : component_class->output_signals) {
      auto port_name = f->get_namestr();
      err << cursor.start_line();
      err << cursor.emit_print(".%s(%s_%s),", port_name.c_str(), field_name.c_str(), port_name.c_str());
    }
  }

  if (component_class->output_registers.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// Output registers");
    for (auto f : component_class->output_registers) {
      auto port_name = f->get_namestr();
      err << cursor.start_line();
      err << cursor.emit_print(".%s(%s_%s),", port_name.c_str(), field_name.c_str(), port_name.c_str());
    }
  }

  for (auto m : component_class->all_functions) {
    //if (m->is_constructor()) continue;
    if (!m->is_public) continue;
    if (m->method_type == MT_INIT) continue;
    if (m->internal_callers.size()) continue;
    if (m->params.empty() && !m->has_return()) continue;

    auto func_name = m->get_namestr();

    err << cursor.start_line();
    err << cursor.emit_print("// %s() ports", func_name.c_str());

    for (auto param : m->params) {
      auto param_name = param->get_namestr();

      err << cursor.start_line();
      err << cursor.emit_print(".%s_%s(%s_%s_%s),",
        func_name.c_str(), param_name.c_str(),
        field_name.c_str(), func_name.c_str(), param_name.c_str());
    }

    if (m->has_return()) {
      err << cursor.start_line();
      err << cursor.emit_print(".%s_ret(%s_%s_ret),",
        func_name.c_str(),
        field_name.c_str(), func_name.c_str());
    }
  }

  // Remove trailing comma from port list
  if (cursor.at_comma) {
    err << cursor.emit_backspace();
  }

  cursor.indent_level--;
  err << cursor.start_line();
  err << cursor.emit_print(")");
  err << cursor.emit(node_semi);

  //----------------------------------------
  // Binding variables

  // Swap template arguments with the values from the template
  // instantiation.
  std::map<std::string, std::string> replacements;
  cursor.id_map.push(cursor.id_map.top());

  if (has_template_params) {
    auto args = node_decl->node_type->child("template_args")->as<CNodeList>();

    int param_count = component_template->params.size();
    int arg_count = args->items.size();
    assert(param_count == arg_count);

    for (int i = 0; i < param_count; i++) {
      auto param = component_template->params[i];
      auto arg = args->items[i];

      auto param_name = param->get_namestr();
      cursor.id_map.top()[param_name] = arg->get_textstr();
    }

  }

  for (auto m : component_class->all_functions) {
    //if (m->is_constructor()) continue;
    if (!m->is_public) continue;
    if (m->method_type == MT_INIT) continue;
    if (m->internal_callers.size()) continue;
    if (m->params.empty() && !m->has_return()) continue;

    auto func_name = m->get_namestr();

    for (auto param : m->params) {
      auto param_name = param->get_namestr();
      err << cursor.start_line();
      err << cursor.emit_splice(param->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s_%s;",
        field_name.c_str(), func_name.c_str(), param_name.c_str());
    }

    if (m->has_return()) {
      err << cursor.start_line();
      err << cursor.emit_splice(m->child("return_type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s_ret;",
        field_name.c_str(), func_name.c_str());
    }
  }

  if (component_class->input_signals.size()) {
    for (auto f : component_class->input_signals) {
      auto port_name = f->get_namestr();
      err << cursor.start_line();
      err << cursor.emit_splice(f->child("decl")->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s;", field_name.c_str(), port_name.c_str());
    }
  }

  if (component_class->output_signals.size()) {
    for (auto f : component_class->output_signals) {
      auto port_name = f->get_namestr();
      err << cursor.start_line();
      err << cursor.emit_splice(f->child("decl")->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s;", field_name.c_str(), port_name.c_str());
    }
  }

  if (component_class->output_registers.size()) {
    for (auto f : component_class->output_registers) {
      auto port_name = f->get_namestr();
      err << cursor.start_line();
      err << cursor.emit_splice(f->child("decl")->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s;", field_name.c_str(), port_name.c_str());
    }
  }

  //----------------------------------------
  // Done

  cursor.id_map.pop();

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeField::trace(CInstance* inst, call_stack& stack) {
  return node_decl->child("value")->trace(inst, stack);
}

//------------------------------------------------------------------------------

void CNodeField::dump() const {
  auto name = get_name();
  LOG_A("Field %.*s : ", name.size(), name.data());

  if (node_decl->node_static) LOG_A("static ");
  if (node_decl->node_const)  LOG_A("const ");
  if (is_public)              LOG_A("public ");
  if (node_decl->is_array())  LOG_A("array ");

  if (parent_class) {
    LOG_A("parent class %s ", parent_class->get_namestr().c_str());
  }

  if (parent_struct) {
    LOG_A("parent struct %s ", parent_struct->get_namestr().c_str());
  }

  if (node_decl->_type_class) {
    LOG_A("type class %s ", node_decl->_type_class->get_namestr().c_str());
  }

  if (node_decl->_type_struct) {
    LOG_A("type struct %s ", node_decl->_type_struct->get_namestr().c_str());
  }

  LOG_A("\n");
}

//------------------------------------------------------------------------------
