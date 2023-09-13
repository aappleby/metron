#include "CNodeField.hpp"

#include "NodeTypes.hpp"
#include "CInstance.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

uint32_t CNodeField::debug_color() const {
  return COL_PINK;
}

std::string_view CNodeField::get_name() const {
  return node_decl->get_name();
}

//------------------------------------------------------------------------------

void CNodeField::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_decl = child("decl")->req<CNodeDeclaration>();
  node_semi = child("semi")->req<CNodePunct>();

  /*
  auto node_static = node_decl->child("static")->as<CNodeKeyword>();
  auto node_const  = node_decl->child("const")->as<CNodeKeyword>();
  auto node_type   = node_decl->child("type")->as<CNodeType>();
  auto node_name   = node_decl->child("name")->as<CNodeIdentifier>();
  auto node_array  = node_decl->child("array")->as<CNodeList>();
  auto node_eq     = node_decl->child("eq")->as<CNodePunct>();
  auto node_value  = node_decl->child("value")->as<CNodeExpression>();
  */
}

//------------------------------------------------------------------------------

std::string_view CNodeField::get_type_name() const {
  return node_decl->get_type_name();

  /*
  auto decl_type = child("type");
  if (auto name = decl_type->child("type_name")) {
    return name->get_text();
  }

  if (auto builtin = decl_type->child("builtin_name")) {
    return builtin->get_text();
  }

  assert(false);
  return "<could not get type of field>";
  */
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
    auto builtin = node_decl->node_type->child("builtin_name");
    auto star    = node_decl->node_type->child("star");
    if (builtin && star && builtin->get_text() == "char") {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

#if 0
//------------------------------------------------------------------------------
// Emit field declarations. For components, also emit glue declarations and
// append the glue parameter list to the field.

// + field_declaration (259) =
// |--# type: type_identifier (444) = "example_data_memory"
// |--# declarator: field_identifier (440) = "data_memory"
// |--# lit (39) = ";"

CHECK_RETURN Err MtCursor::emit_sym_field_declaration(MnNode n) {
  Err err = check_at(n);
  assert(n.sym == sym_field_declaration);

  // Struct outside of class
  if (current_mod.top() == nullptr) {
    // sym_field_declaration
    //   field_type : sym_template_type
    //   field_declarator : sym_field_identifier
    //   lit ;

    return err << emit_children(n);
  }

  // Const local variable
  if (n.is_const()) {
    err << emit_ws_to(n);
    err << emit_print("localparam ");
    err << emit_children(n);
    return err << check_done(n);
  }

  // Enum
  if (n.get_field(field_type).sym == sym_enum_specifier) {
    return emit_children(n);
  }

  //----------
  // Actual fields

  auto field = current_mod.top()->get_field(n.name4());
  assert(field);

  if (field->is_component()) {
    // Component
    err << emit_component(n);
    err << emit_submod_binding_fields(n);
  }
  else if (field->is_port()) {
    // Ports don't go in the class body.
    err << skip_over(n);
  }
  else if (field->is_dead()) {
    err << comment_out(n);
  }
  else {
    err << emit_children(n);
  }

  return err << check_done(n);
}

#endif


Err CNodeField::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  bool in_namespace = ancestor<CNodeNamespace>() != nullptr;

  if (is_const_char()) {
    err << cursor.skip_over(node_decl->node_static);
    err << cursor.skip_gap_after(node_decl->node_static);
    err << cursor.skip_over(node_decl->node_const);
    err << cursor.skip_gap_after(node_decl->node_const);
    err << cursor.skip_over(node_decl->node_type);
    err << cursor.skip_gap_after(node_decl->node_type);
    err << cursor.emit_print("localparam string ");

    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap_after(node_decl->node_name);

    err << cursor.emit(node_decl->node_array);
    err << cursor.emit_gap_after(node_decl->node_array);

    err << cursor.emit(node_decl->node_eq);
    err << cursor.emit_gap_after(node_decl->node_eq);
    err << cursor.emit(node_decl->node_value);
    err << cursor.emit_gap_after(node_decl->node_value);

     err << cursor.emit(node_semi);

    return err << cursor.check_done(this);
  }

  if (node_decl->node_static && node_decl->node_const) {
    // Localparam
    err << cursor.emit_print(in_namespace ? "parameter " : "localparam ");

    err << cursor.comment_out(node_decl->node_static);
    err << cursor.emit_gap_after(node_decl->node_static);

    err << cursor.comment_out(node_decl->node_const);
    err << cursor.emit_gap_after(node_decl->node_const);

    err << cursor.emit(node_decl->node_type);
    err << cursor.emit_gap_after(node_decl->node_type);

    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap_after(node_decl->node_name);

    err << cursor.emit(node_decl->node_array);
    err << cursor.emit_gap_after(node_decl->node_array);

    err << cursor.emit(node_decl->node_eq);
    err << cursor.emit_gap_after(node_decl->node_eq);

    err << cursor.emit(node_decl->node_value);
    err << cursor.emit_gap_after(node_decl->node_value);

    err << cursor.emit(node_semi);
    return err << cursor.check_done(this);
  }

  auto node_builtin = node_decl->node_type->as<CNodeBuiltinType>();
  auto node_targs   = node_decl->node_type->child("template_args");

  if (node_builtin && node_targs) {
    err << cursor.comment_out(node_decl->node_static);
    err << cursor.emit_gap_after(node_decl->node_static);
    err << cursor.comment_out(node_decl->node_const);
    err << cursor.emit_gap_after(node_decl->node_const);
    err << cursor.emit(node_decl->node_type);
    err << cursor.emit_gap_after(node_decl->node_type);

    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap_after(node_decl->node_name);

    err << cursor.emit(node_decl->node_array);
    err << cursor.emit_gap_after(node_decl->node_array);

    err << cursor.emit(node_decl->node_eq);
    err << cursor.emit_gap_after(node_decl->node_eq);
    err << cursor.emit(node_decl->node_value);
    err << cursor.emit_gap_after(node_decl->node_value);

    err << cursor.emit(node_semi);

    return err << cursor.check_done(this);
  }

  if (node_builtin) {
    err << cursor.comment_out(node_decl->node_static);
    err << cursor.emit_gap_after(node_decl->node_static);
    err << cursor.comment_out(node_decl->node_const);
    err << cursor.emit_gap_after(node_decl->node_const);
    err << cursor.emit(node_decl->node_type);
    err << cursor.emit_gap_after(node_decl->node_type);

    err << cursor.emit(node_decl->node_name);
    err << cursor.emit_gap_after(node_decl->node_name);

    err << cursor.emit(node_decl->node_array);
    err << cursor.emit_gap_after(node_decl->node_array);

    err << cursor.emit(node_decl->node_eq);
    err << cursor.emit_gap_after(node_decl->node_eq);
    err << cursor.emit(node_decl->node_value);
    err << cursor.emit_gap_after(node_decl->node_value);

    err << cursor.emit(node_semi);
    return err << cursor.check_done(this);
  }

  if (is_struct()) {
  }


  if (is_component()) {
    return emit_component(cursor);
  }

  //err << cursor.emit_default(this);
  err << CNode::emit(cursor);


  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

#if 0
//------------------------------------------------------------------------------
// Emits the fields that come after a submod declaration

// module my_mod(
//   .foo(my_mod_foo)
// );
// logic my_mod_foo; <-- this part

CHECK_RETURN Err MtCursor::emit_submod_binding_fields(MnNode n) {
  Err err;

  if (current_mod.top()->components.empty()) {
    return err;
  }

  auto node_type = n.get_field(field_type);
  auto node_decl = n.get_field(field_declarator);

  auto component_name = node_decl.text();
  auto component_cname = component_name.c_str();

  std::string type_name = node_type.type5();
  auto component_mod = lib->get_module(type_name);

  // Swap template arguments with the values from the template
  // instantiation.
  std::map<std::string, std::string> replacements;

  auto args = node_type.get_field(field_arguments);
  if (args) {
    int arg_count = args.named_child_count();
    for (int i = 0; i < arg_count; i++) {
      auto key = component_mod->all_modparams[i]->name();
      auto val = args.named_child(i).text();
      replacements[key] = val;
    }
  }

  // FIXME clean this mess up

  current_source.push(component_mod->source_file);
  current_mod.push(component_mod);
  id_map.push(replacements);

  for (auto field : component_mod->input_signals) {
    err << start_line();
    err << emit_splice(field->get_type_node());
    err << emit_print(" %s_", component_cname);
    err << emit_splice(field->get_decl_node());
    err << emit_print(";");
  }

  for (auto param : component_mod->input_method_params) {
    err << start_line();
    err << emit_splice(param->get_type_node());
    err << emit_print(" %s_%s_", component_cname, param->func_name.c_str());
    err << emit_splice(param->get_decl_node());
    err << emit_print(";");
  }

  for (auto field : component_mod->output_signals) {
    err << start_line();
    err << emit_splice(field->get_type_node());
    err << emit_print(" %s_", component_cname);
    err << emit_splice(field->get_decl_node());
    err << emit_print(";");
  }

  for (auto field : component_mod->output_registers) {
    err << start_line();
    err << emit_splice(field->get_type_node());
    err << emit_print(" %s_", component_cname);
    err << emit_splice(field->get_decl_node());
    err << emit_print(";");
  }

  for (auto method : component_mod->output_method_returns) {
    err << start_line();
    err << emit_splice(method->_node.get_field(field_type));
    err << emit_print(" %s_", component_cname);
    err << emit_splice(method->_node.get_field(field_declarator).get_field(field_declarator));
    err << emit_print("_ret;");
  }

  current_source.pop();
  current_mod.pop();
  id_map.pop();

  return err;
}
#endif

/*
[000.029] __ ▆ CNodeField =
[000.029]  ┣━━╸▆ type : CNodeClassType =
[000.029]  ┃   ┗━━╸▆ name : CNodeIdentifier = "Submod"
[000.029]  ┗━━╸▆ name : CNodeIdentifier = "submod"
[000.029] {{10CNodeField}};\n
*/

Err CNodeField::emit_component(Cursor& cursor) {
  Err err;

  err << cursor.skip_to(node_decl->node_type);
  err << cursor.emit(node_decl->node_type);
  err << cursor.emit_gap_after(node_decl->node_type);
  err << cursor.emit(node_decl->node_name);
  err << cursor.emit_gap_after(node_decl->node_name);

  auto src_class = ancestor<CNodeClass>();
  auto repo = src_class->repo;
  auto dst_class = repo->get_class(node_decl->node_type->get_name());

  auto dst_template = dst_class->node_parent->as<CNodeTemplate>();

  //----------------------------------------
  // Component modparams

  bool has_constructor_params = dst_class->constructor && dst_class->constructor->params.size();
  bool has_template_params = dst_template != nullptr;

  if (has_template_params || has_constructor_params) {
    err << cursor.emit_print(" #(");
    cursor.indent_level++;

    // Emit template arguments as module parameters
    if (has_template_params) {
      err << cursor.start_line();
      err << cursor.emit_print("// Template Parameters");

      auto args = node_decl->node_type->child("template_args")->as<CNodeList>();

      int param_count = dst_template->params.size();
      int arg_count = args->items.size();
      assert(param_count == arg_count);

      for (int i = 0; i < param_count; i++) {
        auto param = dst_template->params[i];
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

      auto params = dst_class->constructor->params;

      //for (auto param: dst_class->constructor->params) {
      //}

      // Find the initializer node for the component and extract arguments
      assert(src_class->constructor->node_init);
      CNodeList* args = nullptr;
      for (auto init : src_class->constructor->node_init->items) {
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
    err << cursor.emit_print(")");
  }

#if 0
  //----------------------------------------
  // Component name

  err << emit_gap(node_type, node_decl);
  err << emit_dispatch(node_decl);

  //----------------------------------------
  // Port list

  err << emit_print("(");

  indent.push(indent.top() + "  ");

  if (component_mod->needs_tick()) {
    err << emit_line("// Global clock");
    err << emit_line(".clock(clock),");
  }

  if (component_mod->input_signals.size()) {
    err << emit_line("// Input signals");
    for (auto f : component_mod->input_signals) {
      err << emit_line(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
    }
  }

  if (component_mod->output_signals.size()) {
    err << emit_line("// Output signals");
    for (auto f : component_mod->output_signals) {
      err << emit_line(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
    }
  }

  if (component_mod->output_registers.size()) {
    err << emit_line("// Output registers");
    for (auto f : component_mod->output_registers) {
      err << emit_line(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
    }
  }

  for (auto m : component_mod->all_methods) {
    if (m->is_constructor()) continue;
    if (m->is_public() && m->internal_callers.empty()) {

      if (m->param_nodes.size() || m->has_return()) {
        err << emit_line("// %s() ports", m->cname());
      }

      int param_count = m->param_nodes.size();
      for (int i = 0; i < param_count; i++) {
        auto param = m->param_nodes[i];
        auto node_type = param.get_field(field_type);
        auto node_decl = param.get_field(field_declarator);

        err << emit_line(".%s_%s(%s_%s_%s),", m->cname(), node_decl.text().c_str(), inst_name.c_str(), m->cname(), node_decl.text().c_str());
      }

      if (m->has_return()) {
        auto node_type = m->_node.get_field(field_type);
        auto node_decl = m->_node.get_field(field_declarator);
        auto node_name = node_decl.get_field(field_declarator);

        err << emit_line(".%s_ret(%s_%s_ret),", m->cname(), inst_name.c_str(), m->cname());
      }
    }
  }

  // Remove trailing comma from port list
  if (at_comma) {
    err << emit_backspace();
  }

  indent.pop();

  err << emit_line(")");
  err << emit_dispatch(node_semi);

  return err << check_done(n);
#endif







  //err << emit_submod_binding_fields(n);

  err << cursor.emit(node_semi);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeField::trace(CCall* call) {
  return node_decl->child("value")->trace(call);
}

//------------------------------------------------------------------------------

void CNodeField::dump() {
  //dump_tree();

  auto name = get_name();
  LOG_A("Field %.*s : ", name.size(), name.data());

  if (child("static")) LOG_A("static ");
  if (child("const"))  LOG_A("const ");
  if (_public)    LOG_A("public ");
  if (node_decl->is_array()) LOG_A("array ");
  //if (_enum)      LOG_A("enum ");

  if (_parent_class) {
    auto name = _parent_class->get_name();
    LOG_A("parent class %.*s ", int(name.size()), name.data());
  }

  if (_parent_struct) {
    auto name = _parent_struct->get_name();
    LOG_A("parent struct %.*s ", int(name.size()), name.data());
  }

  if (node_decl->_type_class) {
    auto name = node_decl->_type_class->get_name();
    LOG_A("type class %.*s ", int(name.size()), name.data());
  }

  if (node_decl->_type_struct) {
    auto name = node_decl->_type_struct->get_name();
    LOG_A("type struct %.*s ", int(name.size()), name.data());
  }

  LOG_A("%s", to_string(field_type));

  LOG_A("\n");
}

//------------------------------------------------------------------------------
