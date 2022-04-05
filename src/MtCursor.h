#pragma once
#include <map>
#include <stack>

#include "MtNode.h"
#include "Platform.h"

struct MtModule;
struct MtField;
struct MtSourceFile;
struct MtModLibrary;

//------------------------------------------------------------------------------

struct MtCursor {
  MtCursor(MtModLibrary* lib, MtSourceFile* source_file, std::string* out);

  // Debugging

  void dump(const MnNode& n) const;
  void dump_node_line(MnNode n);
  void print_error(MnNode n, const char* fmt, ...);

  // Indentation

  void push_indent(MnNode n);
  void pop_indent(MnNode n);

  void push_indent_of(MnNode n);
  void pop_indent_of(MnNode n);

  void emit_newline();
  void emit_indent();

  // Generic emit()s.

  void emit_char(char c);
  void emit_ws();
  void emit_ws_to_newline();
  void emit_span(const char* a, const char* b);
  void emit_text(MnNode n);
  void emit(const char* fmt, ...);
  void emit_replacement(MnNode n, const char* fmt, ...);
  void skip_over(MnNode n);
  void skip_ws();
  void comment_out(MnNode n);

  // Per-symbol emit()s.

  CHECK_RETURN Err emit_dispatch(MnNode n);
  CHECK_RETURN Err emit_children(MnNode n);

  CHECK_RETURN Err emit(MnArgList n);
  CHECK_RETURN Err emit(MnAssignmentExpr n);
  CHECK_RETURN Err emit(MnBreakStatement n);
  CHECK_RETURN Err emit(MnCallExpr n);
  CHECK_RETURN Err emit(MnCaseStatement n);
  CHECK_RETURN Err emit(MnComment n);
  CHECK_RETURN Err emit(MnCompoundStatement n);
  CHECK_RETURN Err emit(MnCondExpr n);
  CHECK_RETURN Err emit(MnDecl n);
  CHECK_RETURN Err emit(MnEnumeratorList n);
  CHECK_RETURN Err emit(MnEnumSpecifier n);
  CHECK_RETURN Err emit(MnExprStatement n);
  CHECK_RETURN Err emit(MnFieldDecl decl);
  CHECK_RETURN Err emit(MnFieldDeclList n);
  CHECK_RETURN Err emit(MnFieldExpr n);
  CHECK_RETURN Err emit(MnFieldIdentifier n);
  CHECK_RETURN Err emit(MnIdentifier n);
  CHECK_RETURN Err emit(MnIfStatement n);
  CHECK_RETURN Err emit(MnNamespaceDef n);
  CHECK_RETURN Err emit(MnNumberLiteral n, int size_cast = 0);
  CHECK_RETURN Err emit(MnParameterList n);
  CHECK_RETURN Err emit(MnPreprocInclude n);
  CHECK_RETURN Err emit(MnDataType n);
  CHECK_RETURN Err emit(MnQualifiedId n);
  CHECK_RETURN Err emit(MnReturnStatement n);
  CHECK_RETURN Err emit(MnSizedTypeSpec n);
  CHECK_RETURN Err emit(MnStorageSpec n);
  CHECK_RETURN Err emit(MnClassSpecifier n);
  CHECK_RETURN Err emit(MnSwitchStatement n);
  CHECK_RETURN Err emit(MnTemplateArgList n);
  CHECK_RETURN Err emit(MnTemplateDecl n);
  CHECK_RETURN Err emit(MnTemplateParamList n);
  CHECK_RETURN Err emit(MnTemplateType n);
  CHECK_RETURN Err emit(MnTranslationUnit n);
  CHECK_RETURN Err emit(MnTypeIdentifier n);
  CHECK_RETURN Err emit(MnUsingDecl n);

  // Special-purpose emit()s
  CHECK_RETURN Err emit_preproc(MnNode n);
  CHECK_RETURN Err emit_field_as_localparam(MnFieldDecl field_decl);
  CHECK_RETURN Err emit_func_decl(MnFuncDeclarator n);
  CHECK_RETURN Err emit(MnFuncDefinition n);
  CHECK_RETURN Err emit_field_as_enum_class(MnFieldDecl n);
  CHECK_RETURN Err emit_static_bit_extract(MnCallExpr n, int bx_width);
  CHECK_RETURN Err emit_dynamic_bit_extract(MnCallExpr n, MnNode bx_node);
  CHECK_RETURN Err emit_hoisted_decls(MnCompoundStatement n);
  CHECK_RETURN Err emit_init_declarator_as_decl(MnDecl n);
  CHECK_RETURN Err emit_init_declarator_as_assign(MnDecl n);
  CHECK_RETURN Err emit_output_ports(MnFieldDecl n);
  CHECK_RETURN Err emit_field_as_submod(MnFieldDecl field_decl);
  CHECK_RETURN Err emit_submod_input_port_bindings(MnNode n);

  //----------

  MtModLibrary* lib = nullptr;
  MtSourceFile* source_file = nullptr;
  MtModule* current_mod = nullptr;
  const char* cursor = nullptr;
  MtMethod* current_method = nullptr;
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
