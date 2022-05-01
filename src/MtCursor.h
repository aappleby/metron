#pragma once
#include <map>
#include <stack>

#include "Err.h"
#include "MtNode.h"
#include "Platform.h"

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
  CHECK_RETURN Err emit_ws_to_newline();
  CHECK_RETURN Err emit_span(const char* a, const char* b);
  CHECK_RETURN Err emit_text(MnNode n);
  CHECK_RETURN Err emit_print(const char* fmt, ...);
  CHECK_RETURN Err emit_replacement(MnNode n, const char* fmt, ...);
  CHECK_RETURN Err emit_splice(MnNode n);
  CHECK_RETURN Err skip_over(MnNode n);
  CHECK_RETURN Err skip_ws();
  CHECK_RETURN Err prune_trailing_ws();
  CHECK_RETURN Err comment_out(MnNode n);

  // Generic emit()s.
  CHECK_RETURN Err emit_statement(MnNode n);
  CHECK_RETURN Err emit_expression(MnNode n);
  CHECK_RETURN Err emit_identifier(MnNode n);
  CHECK_RETURN Err emit_child_expressions(MnNode n);

  CHECK_RETURN Err emit_preproc(MnNode n);
  CHECK_RETURN Err emit_type(MnNode n);
  CHECK_RETURN Err emit_declarator(MnNode n);
  CHECK_RETURN Err emit_declaration(MnNode n);
  CHECK_RETURN Err emit_template_argument(MnNode n);
  CHECK_RETURN Err emit_sym_translation_unit(MnTranslationUnit n);
  CHECK_RETURN Err emit_enum(MnFieldDecl n);

  // Special-purpose emit()s
  CHECK_RETURN Err emit_broken_enum(MnFieldDecl n, MnNode node_bitfield);
  CHECK_RETURN Err emit_simple_enum(MnFieldDecl n);
  CHECK_RETURN Err emit_anonymous_enum(MnFieldDecl n);
  CHECK_RETURN Err emit_static_bit_extract(MnCallExpr n, int bx_width);
  CHECK_RETURN Err emit_dynamic_bit_extract(MnCallExpr n, MnNode bx_node);
  CHECK_RETURN Err emit_hoisted_decls(MnCompoundStatement n);
  CHECK_RETURN Err emit_init_declarator_as_decl(MnDecl n);
  CHECK_RETURN Err emit_init_declarator_as_assign(MnDecl n);
  CHECK_RETURN Err emit_port_decls(MnFieldDecl n);
  CHECK_RETURN Err emit_field_as_component(MnFieldDecl field_decl);
  CHECK_RETURN Err emit_input_port_bindings(MnNode n);

  // Per-symbol emit()s.
  CHECK_RETURN Err emit_sym_compound_statement(MnNode n,
                                               const std::string& delim_begin,
                                               const std::string& delim_end);
  CHECK_RETURN Err emit_sym_field_declaration_list(MnNode n);
  CHECK_RETURN Err emit_sym_update_expression(MnNode n);
  CHECK_RETURN Err emit_sym_argument_list(MnArgList n);
  CHECK_RETURN Err emit_sym_assignment_expression(MnAssignmentExpr n);
  CHECK_RETURN Err emit_sym_break_statement(MnBreakStatement n);
  CHECK_RETURN Err emit_sym_call_expression(MnCallExpr n);
  CHECK_RETURN Err emit_sym_case_statement(MnCaseStatement n);
  CHECK_RETURN Err emit_sym_comment(MnComment n);
  CHECK_RETURN Err emit_sym_condition_clause(MnNode n);
  CHECK_RETURN Err emit_sym_conditional_expression(MnCondExpr n);
  CHECK_RETURN Err emit_sym_declaration(MnDecl n);
  CHECK_RETURN Err emit_sym_enumerator_list(MnEnumeratorList n);
  CHECK_RETURN Err emit_sym_enum_specifier(MnEnumSpecifier n);
  CHECK_RETURN Err emit_sym_expression_statement(MnExprStatement n);
  CHECK_RETURN Err emit_sym_field_decl(MnFieldDecl decl);
  CHECK_RETURN Err emit_sym_field_expression(MnFieldExpr n);
  CHECK_RETURN Err emit_sym_field_identifier(MnFieldIdentifier n);
  CHECK_RETURN Err emit_sym_identifier(MnIdentifier n);
  CHECK_RETURN Err emit_sym_if_statement(MnIfStatement n);
  CHECK_RETURN Err emit_sym_namespace_definition(MnNamespaceDef n);
  CHECK_RETURN Err emit_sym_number_literal(MnNumberLiteral n,
                                           int size_cast = 0);
  CHECK_RETURN Err emit_sym_parameter_list(MnParameterList n);
  CHECK_RETURN Err emit_sym_preproc_include(MnPreprocInclude n);
  CHECK_RETURN Err emit_sym_primitive_type(MnDataType n);
  CHECK_RETURN Err emit_sym_qualified_identifier(MnQualifiedId n);
  CHECK_RETURN Err emit_sym_return(MnReturnStatement n);
  CHECK_RETURN Err emit_sym_sized_type_specifier(MnSizedTypeSpec n);
  CHECK_RETURN Err emit_sym_storage_class_specifier(MnStorageSpec n);
  CHECK_RETURN Err emit_sym_class_specifier(MnClassSpecifier n);
  CHECK_RETURN Err emit_sym_switch(MnNode n);
  CHECK_RETURN Err emit_sym_switch_statement(MnSwitchStatement n);
  CHECK_RETURN Err emit_sym_template_argument_list(MnTemplateArgList n);
  CHECK_RETURN Err emit_sym_template_declaration(MnTemplateDecl n);
  CHECK_RETURN Err emit_sym_template_type(MnTemplateType n);
  CHECK_RETURN Err emit_sym_type_identifier(MnTypeIdentifier n);
  CHECK_RETURN Err emit_sym_using_declaration(MnUsingDecl n);
  CHECK_RETURN Err emit_sym_struct_specifier(MnNode n);
  CHECK_RETURN Err emit_sym_function_declarator(MnFuncDeclarator n);
  CHECK_RETURN Err emit_sym_function_definition(MnFuncDefinition n);

  bool branch_contains_component_call(MnNode n);

  CHECK_RETURN Err check_done(MnNode n);

  //----------

  MtModLibrary* lib = nullptr;
  MtSourceFile* current_source = nullptr;
  MtModule* current_mod = nullptr;
  MtMethod* current_method = nullptr;

  const char* cursor = nullptr;

  std::vector<std::string> indent_stack;
  bool at_newline = true;
  bool line_dirty = false;
  bool line_elided = false;

  std::string* str_out;

  std::map<std::string, std::string> id_replacements;
  std::map<std::string, MnNode> preproc_vars;

  bool echo = false;

  bool trim_namespaces = true;
  bool in_ports = false;

  int override_size = 0;
};

//------------------------------------------------------------------------------
