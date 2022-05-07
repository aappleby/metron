#pragma once
#include <map>
#include <stack>

#include "Err.h"
#include "MtNode.h"

struct MtMethod;
struct MtModule;
struct MtField;
struct MtSourceFile;
struct MtModLibrary;

//------------------------------------------------------------------------------

struct MtCursor {
  MtCursor(MtModLibrary* lib, MtSourceFile* source, MtModule* mod,
           std::string* out);

  // Top-level emit function
  CHECK_RETURN Err emit_everything();

  // Indentation
  void push_indent(MnNode n);
  void pop_indent(MnNode n);
  CHECK_RETURN Err emit_newline();
  CHECK_RETURN Err emit_indent();

  // Generic emit()s.
  CHECK_RETURN Err emit_char(char c, uint32_t color = 0);
  CHECK_RETURN Err emit_ws();
  CHECK_RETURN Err emit_ws_to(const MnNode& n);
  CHECK_RETURN Err emit_ws_to(TSSymbol sym, const MnNode& n);
  CHECK_RETURN Err emit_ws_to_newline();
  CHECK_RETURN Err emit_span(const char* a, const char* b);
  CHECK_RETURN Err emit_text(MnNode n);
  CHECK_RETURN Err emit_print(const char* fmt, ...);
  CHECK_RETURN Err emit_replacement(MnNode n, const char* fmt, ...);
  CHECK_RETURN Err skip_over(MnNode n);
  CHECK_RETURN Err skip_ws();
  CHECK_RETURN Err prune_trailing_ws();
  CHECK_RETURN Err comment_out(MnNode n);

  // Generic emit()s.
  CHECK_RETURN Err emit_statement(MnNode n);
  CHECK_RETURN Err emit_expression(MnNode n);
  CHECK_RETURN Err emit_identifier(MnNode n);
  CHECK_RETURN Err emit_child_expressions(MnNode n);
  CHECK_RETURN Err emit_default(MnNode n);
  CHECK_RETURN Err emit_toplevel_block(MnNode n);
  CHECK_RETURN Err emit_toplevel_node(MnNode n);
  CHECK_RETURN Err emit_preproc(MnNode n);
  CHECK_RETURN Err emit_type(MnNode n);
  CHECK_RETURN Err emit_declarator(MnNode n, bool elide_value = false);
  CHECK_RETURN Err emit_declaration(MnNode n);
  CHECK_RETURN Err emit_template_argument(MnNode n);
  CHECK_RETURN Err emit_enum(MnNode n);

  // Special-purpose emit()s
  CHECK_RETURN Err emit_broken_enum(MnNode n);
  CHECK_RETURN Err emit_simple_enum(MnNode n);
  CHECK_RETURN Err emit_anonymous_enum(MnNode n);
  CHECK_RETURN Err emit_static_bit_extract(MnNode n, int bx_width);
  CHECK_RETURN Err emit_dynamic_bit_extract(MnNode n, MnNode bx_node);
  CHECK_RETURN Err emit_hoisted_decls(MnNode n);
  CHECK_RETURN Err emit_init_declarator_as_decl(MnNode n);
  CHECK_RETURN Err emit_init_declarator_as_assign(MnNode n);
  CHECK_RETURN Err emit_port_decls(MnNode n);
  CHECK_RETURN Err emit_field_as_component(MnNode field_decl);
  CHECK_RETURN Err emit_input_port_bindings(MnNode n);
  CHECK_RETURN Err emit_method_ports(MtMethod* m);
  CHECK_RETURN Err emit_field_port(MtField* f);
  CHECK_RETURN Err emit_param_port(MtMethod* m, MnNode node_type, MnNode node_name);
  CHECK_RETURN Err emit_return_port(MtMethod* m, MnNode node_type, MnNode node_name);
  CHECK_RETURN Err emit_port_list(MnNode class_body);
  CHECK_RETURN Err emit_trigger_call(MtMethod* m);
  CHECK_RETURN Err emit_trigger_calls();
  CHECK_RETURN Err emit_param_as_field(MtMethod* method, MnNode n);
  CHECK_RETURN Err emit_modparam_list();
  CHECK_RETURN Err emit_call_binding(MnNode n);
  CHECK_RETURN Err emit_simple_call(MnNode n);

  // Per-symbol emit()s.
  // clang-format off
  CHECK_RETURN Err emit_sym_translation_unit(MnNode n);
  CHECK_RETURN Err emit_sym_argument_list(MnNode n);
  CHECK_RETURN Err emit_sym_assignment_expression(MnNode n);
  CHECK_RETURN Err emit_sym_break_statement(MnNode n);
  CHECK_RETURN Err emit_sym_call_expression(MnNode n);
  CHECK_RETURN Err emit_sym_case_statement(MnNode n);
  CHECK_RETURN Err emit_sym_class_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_comment(MnNode n);
  CHECK_RETURN Err emit_sym_compound_statement(MnNode n, const std::string& delim_begin, const std::string& delim_end);
  CHECK_RETURN Err emit_sym_condition_clause(MnNode n);
  CHECK_RETURN Err emit_sym_conditional_expression(MnNode n);
  CHECK_RETURN Err emit_sym_declaration(MnNode n, bool elide_type, bool elide_value);
  CHECK_RETURN Err emit_sym_enum_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_enumerator(MnNode n);
  CHECK_RETURN Err emit_sym_enumerator_list(MnNode n);
  CHECK_RETURN Err emit_sym_expression_statement(MnNode n);
  CHECK_RETURN Err emit_sym_field_declaration(MnNode decl);
  CHECK_RETURN Err emit_sym_field_declaration_list(MnNode n, bool is_struct);
  CHECK_RETURN Err emit_sym_field_expression(MnNode n);
  CHECK_RETURN Err emit_sym_field_identifier(MnNode n);
  CHECK_RETURN Err emit_sym_function_definition(MnNode n);
  CHECK_RETURN Err emit_sym_identifier(MnNode n);
  CHECK_RETURN Err emit_sym_if_statement(MnNode n);
  CHECK_RETURN Err emit_sym_init_declarator(MnNode n, bool elide_value);
  CHECK_RETURN Err emit_sym_initializer_list(MnNode n);
  CHECK_RETURN Err emit_sym_namespace_definition(MnNode n);
  CHECK_RETURN Err emit_sym_number_literal(MnNode n, int size_cast = 0);
  CHECK_RETURN Err emit_sym_parameter_list(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_arg(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_def(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_ifdef(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_include(MnNode n);
  CHECK_RETURN Err emit_sym_primitive_type(MnNode n);
  CHECK_RETURN Err emit_sym_qualified_identifier(MnNode n);
  CHECK_RETURN Err emit_sym_return(MnNode n);
  CHECK_RETURN Err emit_sym_sized_type_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_storage_class_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_struct_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_switch_statement(MnNode n);
  CHECK_RETURN Err emit_sym_switch(MnNode n);
  CHECK_RETURN Err emit_sym_template_argument_list(MnNode n);
  CHECK_RETURN Err emit_sym_template_declaration(MnNode n);
  CHECK_RETURN Err emit_sym_template_type(MnNode n);
  CHECK_RETURN Err emit_sym_type_definition(MnNode node);
  CHECK_RETURN Err emit_sym_type_identifier(MnNode n);
  CHECK_RETURN Err emit_sym_update_expression(MnNode n);
  CHECK_RETURN Err emit_sym_using_declaration(MnNode n);
  // clang-format on

  bool branch_contains_component_call(MnNode n);

  CHECK_RETURN Err check_done(MnNode n);

  //----------

  MtModLibrary* lib = nullptr;
  MtSourceFile* current_source = nullptr;
  MtModule* current_mod = nullptr;
  MtMethod* current_method = nullptr;

  const char* cursor = nullptr;
  std::stack<const char*> cursor_stack;

  void push_cursor(const MnNode& node) {
    cursor_stack.push(cursor);
    cursor = node.start();
  }

  void pop_cursor(const MnNode& node) {
    assert(cursor == node.end());
    cursor = cursor_stack.top();
    cursor_stack.pop();
  }

  std::vector<std::string> indent_stack;
  bool at_newline = true;
  bool line_dirty = false;
  bool line_elided = false;

  std::string* str_out;

  std::map<std::string, std::string> id_replacements;
  std::map<std::string, MnNode> preproc_vars;

  bool echo = false;

  int override_size = 0;
};

//------------------------------------------------------------------------------
