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

  void emit_dispatch(MnNode n);
  void emit_children(MnNode n);

  void emit(MnArgList n);
  void emit(MnAssignmentExpr n);
  void emit(MnBreakStatement n);
  void emit(MnCallExpr n);
  void emit(MnCaseStatement n);
  void emit(MnComment n);
  void emit(MnCompoundStatement n);
  void emit(MnCondExpr n);
  void emit(MnDecl n);
  void emit(MnEnumeratorList n);
  void emit(MnEnumSpecifier n);
  void emit(MnExprStatement n);
  void emit(MnFieldDecl decl);
  void emit(MnFieldDeclList n);
  void emit(MnFieldExpr n);
  void emit(MnFieldIdentifier n);
  void emit(MnIdentifier n);
  void emit(MnIfStatement n);
  void emit(MnNamespaceDef n);
  void emit(MnNumberLiteral n, int size_cast = 0);
  void emit(MnParameterList n);
  void emit(MnPreprocInclude n);
  void emit(MnDataType n);
  void emit(MnQualifiedId n);
  void emit(MnReturnStatement n);
  void emit(MnSizedTypeSpec n);
  void emit(MnStorageSpec n);
  void emit(MnClassSpecifier n);
  void emit(MnSwitchStatement n);
  void emit(MnTemplateArgList n);
  void emit(MnTemplateDecl n);
  void emit(MnTemplateParamList n);
  void emit(MnTemplateType n);
  void emit(MnTranslationUnit n);
  void emit(MnTypeIdentifier n);
  void emit(MnUsingDecl n);

  // Special-purpose emit()s
  void emit_preproc(MnNode n);
  void emit_field_as_localparam(MnFieldDecl field_decl);
  void emit_func_decl(MnFuncDeclarator n);
  void emit(MnFuncDefinition n);
  void emit_field_as_enum_class(MnFieldDecl n);
  void emit_static_bit_extract(MnCallExpr n, int bx_width);
  void emit_dynamic_bit_extract(MnCallExpr n, MnNode bx_node);
  void emit_hoisted_decls(MnCompoundStatement n);
  void emit_init_declarator_as_decl(MnDecl n);
  void emit_init_declarator_as_assign(MnDecl n);
  void emit_output_ports(MnFieldDecl n);
  void emit_field_as_submod(MnFieldDecl field_decl);
  void emit_submod_input_port_bindings(MnNode n);

  //----------

  MtModLibrary* lib = nullptr;
  MtSourceFile* source_file = nullptr;
  MtModule* current_mod = nullptr;
  const char* cursor = nullptr;
  MtMethod* current_method;
  std::vector<std::string> indent_stack;
  bool at_newline = true;
  bool line_dirty = false;
  bool line_elided = false;

  std::string* str_out;

  std::map<std::string, std::string> id_replacements;
  std::map<std::string, MnNode> preproc_vars;

  std::vector<MnNode> node_stack;

  bool quiet = true;

  int in_module_or_package = 0;

  bool in_public = false;

  bool trim_namespaces = true;
  bool in_ports = false;

  int override_size = 0;
};

//------------------------------------------------------------------------------
