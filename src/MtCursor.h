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
  MtCursor(
    MtModLibrary* lib,
    MtSourceFile* source,
    MtModule* mod,
    std::string* out
  );

  // Indentation

  void push_indent(MnNode n);
  void pop_indent(MnNode n);

  CHECK_RETURN Err emit_newline();
  CHECK_RETURN Err emit_indent();

  // Generic emit()s.

  CHECK_RETURN Err emit_char(char c);
  CHECK_RETURN Err emit_ws();
  CHECK_RETURN Err emit_ws_to_newline();
  CHECK_RETURN Err emit_span(const char* a, const char* b);
  CHECK_RETURN Err emit_text(MnNode n);
  CHECK_RETURN Err emit_printf(const char* fmt, ...);
  CHECK_RETURN Err emit_replacement(MnNode n, const char* fmt, ...);
  CHECK_RETURN Err emit_splice(MnNode n);
  CHECK_RETURN Err skip_over(MnNode n);
  CHECK_RETURN Err skip_ws();
  CHECK_RETURN Err prune_trailing_ws();
  CHECK_RETURN Err comment_out(MnNode n);

  // Per-symbol emit()s.

  CHECK_RETURN Err emit_dispatch(MnNode n);
  CHECK_RETURN Err emit_children(MnNode n);

  CHECK_RETURN Err emit_arg_list(MnArgList n);
  CHECK_RETURN Err emit_assignment(MnAssignmentExpr n);
  CHECK_RETURN Err emit_break(MnBreakStatement n);
  CHECK_RETURN Err emit_call(MnCallExpr n);
  CHECK_RETURN Err emit_case_statement(MnCaseStatement n);
  CHECK_RETURN Err emit_comment(MnComment n);
  CHECK_RETURN Err emit_compound(MnCompoundStatement n);
  CHECK_RETURN Err emit_condition(MnCondExpr n);
  CHECK_RETURN Err emit_decl(MnDecl n);
  CHECK_RETURN Err emit_enum_list(MnEnumeratorList n);
  CHECK_RETURN Err emit_enum_specifier(MnEnumSpecifier n);
  CHECK_RETURN Err emit_expression(MnExprStatement n);
  CHECK_RETURN Err emit_field_decl(MnFieldDecl decl);
  CHECK_RETURN Err emit_field_expr(MnFieldExpr n);
  CHECK_RETURN Err emit_field_id(MnFieldIdentifier n);
  CHECK_RETURN Err emit_identifier(MnIdentifier n);
  CHECK_RETURN Err emit_if_statement(MnIfStatement n);
  CHECK_RETURN Err emit_namespace_def(MnNamespaceDef n);
  CHECK_RETURN Err emit_number_literal(MnNumberLiteral n, int size_cast = 0);
  CHECK_RETURN Err emit_param_list(MnParameterList n);
  CHECK_RETURN Err emit_preproc_include(MnPreprocInclude n);
  CHECK_RETURN Err emit_data_type(MnDataType n);
  CHECK_RETURN Err emit_qualified_id(MnQualifiedId n);
  CHECK_RETURN Err emit_return(MnReturnStatement n);
  CHECK_RETURN Err emit_sized_type_spec(MnSizedTypeSpec n);
  CHECK_RETURN Err emit_storage_spec(MnStorageSpec n);
  CHECK_RETURN Err emit_class(MnClassSpecifier n);
  CHECK_RETURN Err emit_switch(MnSwitchStatement n);
  CHECK_RETURN Err emit_template_arg_list(MnTemplateArgList n);
  CHECK_RETURN Err emit_template_decl(MnTemplateDecl n);
  CHECK_RETURN Err emit_template_type(MnTemplateType n);
  CHECK_RETURN Err emit_translation_unit(MnTranslationUnit n);
  CHECK_RETURN Err emit_type_id(MnTypeIdentifier n);
  CHECK_RETURN Err emit_using_decl(MnUsingDecl n);

  CHECK_RETURN Err emit_struct(MnNode n);
  CHECK_RETURN Err emit_field_decl_list(MnNode n);

  // Special-purpose emit()s
  CHECK_RETURN Err emit_enum(MnFieldDecl n);
  
  CHECK_RETURN Err emit_broken_enum(MnFieldDecl n, MnNode node_bitfield);
  CHECK_RETURN Err emit_simple_enum(MnFieldDecl n);
  CHECK_RETURN Err emit_anonymous_enum(MnFieldDecl n);


  CHECK_RETURN Err emit_preproc(MnNode n);
  CHECK_RETURN Err emit_func_decl(MnFuncDeclarator n);
  CHECK_RETURN Err emit_func_def(MnFuncDefinition n);
  CHECK_RETURN Err emit_static_bit_extract(MnCallExpr n, int bx_width);
  CHECK_RETURN Err emit_dynamic_bit_extract(MnCallExpr n, MnNode bx_node);
  CHECK_RETURN Err emit_hoisted_decls(MnCompoundStatement n);
  CHECK_RETURN Err emit_init_declarator_as_decl(MnDecl n);
  CHECK_RETURN Err emit_init_declarator_as_assign(MnDecl n);
  CHECK_RETURN Err emit_port_decls(MnFieldDecl n);
  CHECK_RETURN Err emit_field_as_submod(MnFieldDecl field_decl);
  CHECK_RETURN Err emit_input_port_bindings(MnNode n);

  bool branch_contains_submod_call(MnNode n);

  CHECK_RETURN Err check_done(MnNode n);

  //----------

  MtModLibrary* lib = nullptr;
  MtSourceFile* current_source = nullptr;
  MtModule*     current_mod = nullptr;
  MtMethod*     current_method = nullptr;

  const char* cursor = nullptr;


  std::vector<std::string> indent_stack;
  bool at_newline = true;
  bool line_dirty = false;
  bool line_elided = false;

  std::string* str_out;

  std::map<std::string, std::string> id_replacements;
  std::map<std::string, MnNode> preproc_vars;

  std::vector<MnNode> node_stack;

  bool echo = false;

  bool in_public = false;

  bool trim_namespaces = true;
  bool in_ports = false;

  int override_size = 0;
};

//------------------------------------------------------------------------------
