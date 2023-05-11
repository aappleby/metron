#pragma once
#include "metron/nodes/MtNode.h"

#include "metrolib/core/Err.h"

#include <map>
#include <stack>
#include <vector>
#include <functional>

struct MtMethod;
struct MtModule;
struct MtField;
struct MtSourceFile;
struct MtModLibrary;
struct MtCursor;

typedef std::function<Err(MtCursor*, MnNode node)> emit_cb;
typedef std::map<TSSymbol, emit_cb> emit_map;

typedef std::map<TSSymbol, std::string> sym_to_string;
typedef std::map<std::string, MnNode> string_to_node;
typedef std::map<std::string, std::string> string_to_string;

//------------------------------------------------------------------------------

struct MtCursor {
  MtCursor(MtModLibrary* lib, MtSourceFile* source, MtModule* mod,
           std::string* out);

  CHECK_RETURN Err check_at(const MnNode& n);
  CHECK_RETURN Err check_at(TSSymbol sym, const MnNode& n);
  CHECK_RETURN Err check_done(MnNode n);

  // Top-level emit function
  CHECK_RETURN Err emit_everything();

  // Indentation
  void push_indent(MnNode n);
  void pop_indent(MnNode n);
  CHECK_RETURN Err emit_backspace();
  CHECK_RETURN Err emit_indent();
  CHECK_RETURN Err start_line();

  CHECK_RETURN Err emit_gap(MnNode a, MnNode b);
  CHECK_RETURN Err skip_gap(MnNode a, MnNode b);

  CHECK_RETURN Err emit_tail(MnNode a);
  CHECK_RETURN Err skip_tail(MnNode a);

  // Generic emit()s.
  CHECK_RETURN Err emit_char(char c, uint32_t color = 0);
  CHECK_RETURN Err emit_ws();
  CHECK_RETURN Err emit_ws_to(const MnNode& n);
  CHECK_RETURN Err emit_ws_to(TSSymbol sym, const MnNode& n);
  CHECK_RETURN Err emit_span(const char* a, const char* b);
  CHECK_RETURN Err skip_span(const char* a, const char* b);
  CHECK_RETURN Err emit_text(MnNode n);
  CHECK_RETURN Err emit_vprint(const char* fmt, va_list args);
  CHECK_RETURN Err emit_line(const char* fmt, ...);
  CHECK_RETURN Err emit_print(const char* fmt, ...);
  CHECK_RETURN Err emit_replacement(MnNode n, const char* fmt, ...);
  CHECK_RETURN Err skip_over(MnNode n);
  CHECK_RETURN Err skip_ws();
  CHECK_RETURN Err skip_ws_inside(const MnNode& n);
  CHECK_RETURN Err prune_trailing_ws();
  CHECK_RETURN Err comment_out(MnNode n);

  CHECK_RETURN bool can_omit_call(MnNode n);

  // Generic emit()s.
  CHECK_RETURN Err emit_dispatch(MnNode n);
  CHECK_RETURN Err emit_block(MnNode n, const char* prefix, const char* suffix);
  CHECK_RETURN Err emit_splice(MnNode n);
  CHECK_RETURN Err emit_children(MnNode n);
  CHECK_RETURN Err emit_toplevel_node(MnNode n);

  // Special-purpose emit()s
  CHECK_RETURN Err emit_static_bit_extract(MnNode n, int bx_width);
  CHECK_RETURN Err emit_dynamic_bit_extract(MnNode n, MnNode bx_node);
  CHECK_RETURN Err emit_hoisted_decls(MnNode n);
  CHECK_RETURN Err emit_submod_binding_fields(MnNode n);
  CHECK_RETURN Err emit_component(MnNode n);

  CHECK_RETURN Err emit_local_call_arg_binding(MtMethod* method, MnNode param, MnNode val);
  CHECK_RETURN Err emit_component_call_arg_binding(MnNode inst, MtMethod* method, MnNode param, MnNode val);

  CHECK_RETURN Err emit_call_arg_bindings(MnNode n);
  CHECK_RETURN Err emit_method_ports(MtMethod* m);
  CHECK_RETURN Err emit_func_binding_vars(MtMethod* m);
  CHECK_RETURN Err emit_field_port(MtField* f);
  CHECK_RETURN Err emit_param_port(MtMethod* m, MnNode node_type, MnNode node_name);
  CHECK_RETURN Err emit_param_binding(MtMethod* m, MnNode node_type, MnNode node_name);
  CHECK_RETURN Err emit_return_port(MtMethod* m, MnNode node_type, MnNode node_name);
  CHECK_RETURN Err emit_return_binding(MtMethod* m, MnNode node_type, MnNode node_name);
  CHECK_RETURN Err emit_module_ports(MnNode class_body);

  CHECK_RETURN Err emit_func_as_init(MnNode n);
  CHECK_RETURN Err emit_func_as_func(MnNode n);
  CHECK_RETURN Err emit_func_as_task(MnNode n);
  CHECK_RETURN Err emit_func_as_always_comb(MnNode n);
  CHECK_RETURN Err emit_func_as_always_ff(MnNode n);

  CHECK_RETURN Err emit_optional_param_as_modparam(MnNode n);
  CHECK_RETURN Err emit_param_list_as_modparams(MnNode n);
  CHECK_RETURN Err emit_template_params_as_modparams(MnNode n);

  // Per-symbol emit()s.
  // clang-format off
  CHECK_RETURN Err emit_sym_assignment_expression(MnNode n);
  CHECK_RETURN Err emit_sym_call_expression(MnNode n);
  CHECK_RETURN Err emit_sym_case_statement(MnNode n);
  CHECK_RETURN Err emit_sym_class_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_comment(MnNode n);
  CHECK_RETURN Err emit_sym_compound_statement(MnNode n);
  CHECK_RETURN Err emit_sym_declaration_list(MnNode n);
  CHECK_RETURN Err emit_sym_declaration(MnNode n);
  CHECK_RETURN Err emit_sym_enum_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_expression_statement(MnNode n);
  CHECK_RETURN Err emit_sym_field_declaration_list(MnNode n);
  CHECK_RETURN Err emit_sym_field_declaration(MnNode decl);
  CHECK_RETURN Err emit_sym_field_expression(MnNode n);
  CHECK_RETURN Err emit_sym_for_statement(MnNode n);
  CHECK_RETURN Err emit_sym_function_declarator(MnNode n);
  CHECK_RETURN Err emit_sym_function_definition(MnNode n);
  CHECK_RETURN Err emit_sym_identifier(MnNode n);
  CHECK_RETURN Err emit_sym_if_statement(MnNode n);
  CHECK_RETURN Err emit_sym_initializer_list(MnNode n);
  CHECK_RETURN Err emit_sym_namespace_definition(MnNode n);
  CHECK_RETURN Err emit_sym_nullptr(MnNode n);
  CHECK_RETURN Err emit_sym_number_literal(MnNode n);
  CHECK_RETURN Err emit_sym_pointer_declarator(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_arg(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_def(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_ifdef(MnNode n);
  CHECK_RETURN Err emit_sym_preproc_include(MnNode n);
  CHECK_RETURN Err emit_sym_primitive_type(MnNode n);
  CHECK_RETURN Err emit_sym_qualified_identifier(MnNode n);
  CHECK_RETURN Err emit_sym_return_statement(MnNode n);
  CHECK_RETURN Err emit_sym_sized_type_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_struct_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_switch_statement(MnNode n);
  CHECK_RETURN Err emit_sym_template_argument_list(MnNode n);
  CHECK_RETURN Err emit_sym_template_declaration(MnNode n);
  CHECK_RETURN Err emit_sym_template_type(MnNode n);
  CHECK_RETURN Err emit_sym_translation_unit(MnNode n);
  CHECK_RETURN Err emit_sym_type_identifier(MnNode n);
  CHECK_RETURN Err emit_sym_update_expression(MnNode n);
  CHECK_RETURN Err emit_sym_using_declaration(MnNode n);
  // clang-format on

  bool branch_contains_component_call(MnNode n);

  //----------------------------------------

  const char* cursor = nullptr;
  std::stack<const char*> cursor_stack;

  void push_cursor(const MnNode& node) {
    cursor_stack.push(cursor);
    cursor = node.start();
  }

  void pop_cursor() {
    cursor = cursor_stack.top();
    cursor_stack.pop();
  }

  //----------------------------------------

  MtModLibrary* lib = nullptr;

  std::stack<MtSourceFile*> current_source;
  std::stack<MtModule*> current_mod;
  std::stack<MtMethod*> current_method;

  std::stack<std::string> block_prefix;
  std::stack<std::string> block_suffix;
  std::stack<int> override_size;
  std::stack<string_to_node> preproc_vars;
  std::stack<string_to_string> id_map;
  std::stack<string_to_string> token_map;
  std::stack<bool> elide_type;
  std::stack<bool> elide_value;

  //----------------------------------------
  // Output state

  std::string* str_out;
  std::stack<std::string> indent;
  bool at_newline = true;
  bool at_comma = false;
  bool line_dirty = false;
  bool line_elided = false;
  bool echo = false;
};

//------------------------------------------------------------------------------
