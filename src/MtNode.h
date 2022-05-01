#pragma once
#include <assert.h>

#include <functional>
#include <string>
#include <vector>

#include "MtUtils.h"
#include "Platform.h"
#include "TreeSymbols.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

struct MtModule;
struct MtSourceFile;
struct MtMethod;

//------------------------------------------------------------------------------

struct MnNode {
  MnNode();
  MnNode(TSNode node, int sym, int field, MtSourceFile* source);

  //----------

  SourceRange get_source() const;
  void dump_source_lines() const;
  void dump_tree(int index = 0, int depth = 0, int maxdepth = 255) const;

  void error() const {
    dump_tree(0, 0, 255);
    debugbreak();
  }

  operator bool() const { return !ts_node_is_null(node); }

  MnNode& check_null() {
    if (is_null()) debugbreak();
    return *this;
  }

  const char* ts_node_type() const { return ::ts_node_type(node); }
  uint32_t start_byte() const { return ts_node_start_byte(node); }
  uint32_t end_byte() const { return ts_node_end_byte(node); }

  const char* start() const;
  const char* end() const;
  std::string text() const;
  bool match(const char* s);

  //----------

  int child_count() const { return (int)ts_node_child_count(node); }
  int named_child_count() const { return (int)ts_node_named_child_count(node); }

  MnNode child(int i) const;
  MnNode named_child(int i) const;
  MnNode first_named_child() const;

  bool is_static() const;
  bool is_const() const;

  bool is_null() const { return ts_node_is_null(node); }
  bool is_named() const { return !is_null() && ts_node_is_named(node); }

  //----------

  MnNode get_field(int field_id) const;

  //----------

  std::string name4() const;
  std::string type5() const;

  typedef std::function<void(MnNode)> NodeVisitor;

  void visit_tree(NodeVisitor cv);

  void check_sym(TSSymbol sym) {
    if (!is_null() && this->sym != sym) {
      debugbreak();
    }
    assert(is_null() || this->sym == sym);
  }

  //----------

  TSNode node;
  TSSymbol sym;
  int field;
  MtSourceFile* source;

  static const MnNode null;
};

//------------------------------------------------------------------------------

struct MnConstIterator {
  MnConstIterator(MnNode parent) {
    if (ts_node_is_null(parent.node)) {
      cursor = {0};
    } else {
      cursor = ts_tree_cursor_new(parent.node);

      if (!ts_tree_cursor_goto_first_child(&cursor)) {
        ts_tree_cursor_delete(&cursor);
        cursor = {0};
      }
    }
    this->source = parent.source;
  }

  ~MnConstIterator() {
    if (cursor.tree) {
      ts_tree_cursor_delete(&cursor);
    }
  }

  MnConstIterator& operator++() {
    if (!ts_tree_cursor_goto_next_sibling(&cursor)) {
      ts_tree_cursor_delete(&cursor);
      cursor = {0};
    }
    return *this;
  }

  // bool operator<(const MnConstIterator& b) const;
  bool operator!=(const MnConstIterator& b) const;

  const MnNode operator*() const {
    auto child = ts_tree_cursor_current_node(&cursor);
    auto sym = ts_node_symbol(child);
    auto field = ts_tree_cursor_current_field_id(&cursor);

    return {child, sym, field, source};
  }

  TSTreeCursor cursor;
  MtSourceFile* source;
};

//------------------------------------------------------------------------------

struct MnIterator {
  MnIterator(MnNode parent) {
    if (ts_node_is_null(parent.node)) {
      cursor = {0};
    } else {
      cursor = ts_tree_cursor_new(parent.node);

      if (!ts_tree_cursor_goto_first_child(&cursor)) {
        ts_tree_cursor_delete(&cursor);
        cursor = {0};
      }
    }
    this->source = parent.source;
  }

  ~MnIterator() {
    if (cursor.tree) {
      ts_tree_cursor_delete(&cursor);
    }
  }

  MnIterator& operator++() {
    if (!ts_tree_cursor_goto_next_sibling(&cursor)) {
      ts_tree_cursor_delete(&cursor);
      cursor = {0};
    }
    return *this;
  }

  // bool operator<(const MnIterator& b) const;
  bool operator!=(const MnIterator& b) const;

  MnNode operator*() const {
    auto child = ts_tree_cursor_current_node(&cursor);
    auto sym = ts_node_symbol(child);
    auto field = ts_tree_cursor_current_field_id(&cursor);

    return {child, sym, field, source};
  }

  TSTreeCursor cursor;
  MtSourceFile* source;
};

//------------------------------------------------------------------------------

inline MnConstIterator begin(const MnNode& parent) {
  return MnConstIterator(parent);
}

inline MnConstIterator end(const MnNode& parent) {
  return MnConstIterator(MnNode::null);
}

inline MnIterator begin(MnNode& parent) { return MnIterator(parent); }

inline MnIterator end(MnNode& parent) { return MnIterator(MnNode::null); }

//------------------------------------------------------------------------------

struct MnIdentifier : public MnNode {
  MnIdentifier(){};
  MnIdentifier(const MnNode& n) : MnNode(n) { check_sym(sym_identifier); }
};

//------------------------------------------------------------------------------

struct MnLiteral : public MnNode {
  MnLiteral(){};
  MnLiteral(const MnNode& n) : MnNode(n) { assert(!is_named()); }
};

//------------------------------------------------------------------------------

struct MnAssignmentExpr : public MnNode {
  MnAssignmentExpr(){};
  MnAssignmentExpr(const MnNode& n) : MnNode(n) {
    check_sym(sym_assignment_expression);
  }
  MnNode lhs() { return get_field(field_left); }
  MnLiteral op() { return MnLiteral(get_field(field_operator)); }
  MnNode rhs() { return get_field(field_right); }
};

//------------------------------------------------------------------------------

struct MnTypeIdentifier : public MnNode {
  MnTypeIdentifier(){};
  MnTypeIdentifier(const MnNode& n) : MnNode(n) {
    check_sym(alias_sym_type_identifier);
  }
};

//------------------------------------------------------------------------------

struct MnFieldDeclList : public MnNode {
  MnFieldDeclList(){};
  MnFieldDeclList(const MnNode& n) : MnNode(n) {
    check_sym(sym_field_declaration_list);
  }
};

//------------------------------------------------------------------------------

struct MnClassSpecifier : public MnNode {
  MnClassSpecifier(){};
  MnClassSpecifier(const MnNode& n) : MnNode(n) {
    check_sym(sym_class_specifier);
  }
  MnTypeIdentifier name() { return MnTypeIdentifier(get_field(field_name)); }
  MnFieldDeclList body() { return MnFieldDeclList(get_field(field_body)); }
};

//------------------------------------------------------------------------------

struct MnDataType : public MnNode {
  MnDataType(){};
  MnDataType(const MnNode& n) : MnNode(n) {
    assert(!is_null());
    assert(sym == sym_template_type || sym == sym_primitive_type);
  }
};

//------------------------------------------------------------------------------

struct MnExprStatement : public MnNode {
  MnExprStatement(){};
  MnExprStatement(const MnNode& n) : MnNode(n) {
    check_sym(sym_expression_statement);
  }
};

//------------------------------------------------------------------------------

struct MnFieldName : public MnNode {
  MnFieldName(){};
  MnFieldName(const MnNode& n) : MnNode(n) {
    check_sym(alias_sym_field_identifier);
  }
};

//------------------------------------------------------------------------------

struct MnParameterList : public MnNode {
  MnParameterList(){};
  MnParameterList(const MnNode& n) : MnNode(n) {
    check_sym(sym_parameter_list);
  }
};

//------------------------------------------------------------------------------

struct MnFuncDeclarator : public MnNode {
  MnFuncDeclarator(){};
  MnFuncDeclarator(const MnNode& n) : MnNode(n) {
    check_sym(sym_function_declarator);
  }

  MnFieldName decl() { return MnFieldName(get_field(field_declarator)); }
  MnParameterList params() {
    return MnParameterList(get_field(field_parameters));
  }
};

//------------------------------------------------------------------------------

struct MnCompoundStatement : public MnNode {
  MnCompoundStatement(){};
  MnCompoundStatement(const MnNode& n) : MnNode(n) {
    check_sym(sym_compound_statement);
  }
};

//------------------------------------------------------------------------------

struct MnFuncDefinition : public MnNode {
  MnFuncDefinition(){};
  MnFuncDefinition(const MnNode& n) : MnNode(n) {
    check_sym(sym_function_definition);
  }

  /*
  bool has_type() const {
    auto node_type = get_field(field_type);
    return !node_type.is_null();
  }

  MnDataType type() {
    auto node_type = get_field(field_type);
    return node_type.is_null() ? MnNode::null : node_type;
  }
  */

  MnFuncDeclarator decl() {
    return MnFuncDeclarator(get_field(field_declarator));
  }
  MnCompoundStatement body() {
    return MnCompoundStatement(get_field(field_body));
  }
};

//------------------------------------------------------------------------------

struct MnArgList : public MnNode {
  MnArgList(){};
  MnArgList(const MnNode& n) : MnNode(n) { check_sym(sym_argument_list); }
};

//------------------------------------------------------------------------------

struct MnFieldExpr : public MnNode {
  MnFieldExpr(){};
  MnFieldExpr(const MnNode& n) : MnNode(n) { check_sym(sym_field_expression); }
};

//------------------------------------------------------------------------------

struct MnFieldIdentifier : public MnNode {
  MnFieldIdentifier(){};
  MnFieldIdentifier(const MnNode& n) : MnNode(n) {
    check_sym(alias_sym_field_identifier);
  }
};

//------------------------------------------------------------------------------

struct MnTemplateArgList : public MnNode {
  MnTemplateArgList(){};
  MnTemplateArgList(const MnNode& n) : MnNode(n) {
    check_sym(sym_template_argument_list);
  }
};

//------------------------------------------------------------------------------

struct MnTemplateFunc : public MnNode {
  MnTemplateFunc() {}
  MnTemplateFunc(const MnNode& n) : MnNode(n) {
    check_sym(sym_template_function);
  }

  MnTemplateArgList args() { return get_field(field_arguments); }
};

//------------------------------------------------------------------------------
// MtIdentifier || MtTemplateFunc

// primitive type because int(x) style casts

struct MnFunc : public MnNode {
  MnFunc() {}
  MnFunc(const MnNode& n) : MnNode(n) {
    if (is_null() || is_id() || is_templ() || is_field() || is_prim()) {
    } else {
      debugbreak();
    }
  }

  std::string name() {
    if (sym == sym_field_expression) {
      return as_field().name4();
    } else {
      return name4();
    }
  }

  bool is_id() { return sym == sym_identifier; }
  bool is_templ() { return sym == sym_template_function; }
  bool is_field() { return sym == sym_field_expression; }
  bool is_prim() { return sym == sym_primitive_type; }

  MnIdentifier as_id() { return MnIdentifier(*this); }
  MnTemplateFunc as_templ() { return MnTemplateFunc(*this); }
  MnFieldExpr as_field() { return MnFieldExpr(*this); }
};

//------------------------------------------------------------------------------

struct MnCallExpr : public MnNode {
  MnCallExpr(){};
  MnCallExpr(const MnNode& n) : MnNode(n) { check_sym(sym_call_expression); }

  // identifier or field expression or template function?
  MnFunc func() { return MnFunc(get_field(field_function)); }
  MnArgList args() { return MnArgList(get_field(field_arguments)); }
};

//------------------------------------------------------------------------------

struct MnInitDecl : public MnNode {
  MnInitDecl(){};
  MnInitDecl(const MnNode& n) : MnNode(n) { check_sym(sym_init_declarator); }
  MnIdentifier decl() { return MnIdentifier(get_field(field_declarator)); }
};

//------------------------------------------------------------------------------

struct MnTemplateType : public MnNode {
  MnTemplateType(){};
  MnTemplateType(const MnNode& n) : MnNode(n) { check_sym(sym_template_type); }

  MnTypeIdentifier name() { return MnTypeIdentifier(get_field(field_name)); }
  MnTemplateArgList args() {
    return MnTemplateArgList(get_field(field_arguments));
  }
};

//------------------------------------------------------------------------------

/*
========== tree dump begin
[00:000:187] declaration =
[00:032:321] |  type: template_type =
[00:022:395] |  |  name: type_identifier = "logic"
[01:003:324] |  |  arguments: template_argument_list =
[00:000:036] |  |  |  lit = "<"
[01:000:112] |  |  |  number_literal = "5"
[02:000:033] |  |  |  lit = ">"
[01:009:001] |  declarator: identifier = "default_funct"
[02:000:039] |  lit = ";"
========== tree dump end

========== tree dump begin
[00:000:187] declaration =
[00:032:321] |  type: template_type =
[00:022:395] |  |  name: type_identifier = "logic"
[01:003:324] |  |  arguments: template_argument_list =
[00:000:036] |  |  |  lit = "<"
[01:000:112] |  |  |  number_literal = "5"
[02:000:033] |  |  |  lit = ">"
[01:009:224] |  declarator: init_declarator =
[00:009:001] |  |  declarator: identifier = "default_funct"
[01:000:063] |  |  lit = "="
[02:034:112] |  |  value: number_literal = "22"
[02:000:039] |  lit = ";"
========== tree dump end
*/

struct MnDecl : public MnNode {
  MnDecl(){};
  MnDecl(const MnNode& n) : MnNode(n) { check_sym(sym_declaration); }

  bool is_init_decl() {
    assert(!is_null());
    return get_field(field_declarator).sym == sym_init_declarator;
  }

  MnTemplateType _type() { return MnTemplateType(get_field(field_type)); }
  MnIdentifier _decl() { return MnIdentifier(get_field(field_declarator)); }
  MnInitDecl _init_decl() { return MnInitDecl(get_field(field_declarator)); }
};

//------------------------------------------------------------------------------

struct MnEnumSpecifier : public MnNode {
  MnEnumSpecifier(){};
  MnEnumSpecifier(const MnNode& n) : MnNode(n) {
    check_sym(sym_enum_specifier);
  }
};

//------------------------------------------------------------------------------
// MtIdentifier || MtTemplateType || MtEnumSpecifier || MtPrimitiveType

struct MnType : public MnNode {
  MnType() {}
  MnType(const MnNode& n) : MnNode(n) {
    assert(is_id() || is_templ() || is_enum() || is_prim());
  }

  bool is_id() { return sym == alias_sym_type_identifier; }
  bool is_templ() { return sym == sym_template_type; }
  bool is_enum() { return sym == sym_enum_specifier; }
  bool is_prim() { return sym == sym_primitive_type; }

  MnIdentifier as_id() { return MnIdentifier(*this); }
  MnTemplateType as_templ() { return MnTemplateType(*this); }
  MnEnumSpecifier as_enum() { return MnEnumSpecifier(*this); }
  MnDataType as_prim() { return MnDataType(*this); }
};

//------------------------------------------------------------------------------

struct MnFieldDecl : public MnNode {
  MnFieldDecl(){};
  MnFieldDecl(const MnNode& n) : MnNode(n) { check_sym(sym_field_declaration); }

  bool is_primitive() {
    // Primitive types are primitive types.
    if (type().sym == sym_primitive_type) return true;

    // Logic arrays are primitive types.
    if (type().sym == sym_template_type) {
      auto templ_name = type().get_field(field_name);
      if (templ_name.match("logic")) return true;
    }

    // Bits are primitive types.
    if (type().match("bit")) return true;

    return false;
  }

  bool is_enum() const { return type().sym == sym_enum_specifier; }

  bool is_param() const { return is_static() && is_const(); }

  MnType type() const { return MnType(get_field(field_type)); }
  MnFieldName name() {
    auto decl = get_field(field_declarator);
    if (decl.sym == sym_array_declarator) {
      decl = decl.get_field(field_declarator);
    }
    return MnFieldName(decl);
  }
  MnCallExpr value() { return MnCallExpr(get_field(field_default_value)); }
};

//------------------------------------------------------------------------------

struct MnPreprocInclude : public MnNode {
  MnPreprocInclude(){};
  MnPreprocInclude(const MnNode& n) : MnNode(n) {
    check_sym(sym_preproc_include);
  }

  MnNode path_node() { return get_field(field_path); }

  std::string path() { return get_field(field_path).text(); }
};

//------------------------------------------------------------------------------

struct MnNumberLiteral : public MnNode {
  MnNumberLiteral(){};
  MnNumberLiteral(const MnNode& n) : MnNode(n) {
    check_sym(sym_number_literal);
  }
};

struct MnReturnStatement : public MnNode {
  MnReturnStatement(){};
  MnReturnStatement(const MnNode& n) : MnNode(n) {
    check_sym(sym_return_statement);
  }
};

struct MnTemplateDecl : public MnNode {
  MnTemplateDecl(){};
  MnTemplateDecl(const MnNode& n) : MnNode(n) {
    check_sym(sym_template_declaration);
  }
};

struct MnTranslationUnit : public MnNode {
  MnTranslationUnit(){};
  MnTranslationUnit(const MnNode& n) : MnNode(n) {
    check_sym(sym_translation_unit);
  }
};

struct MnTemplateParamList : public MnNode {
  MnTemplateParamList(){};
  MnTemplateParamList(const MnNode& n) : MnNode(n) {
    check_sym(sym_template_parameter_list);
  }
};

struct MnEnumeratorList : public MnNode {
  MnEnumeratorList(){};
  MnEnumeratorList(const MnNode& n) : MnNode(n) {
    check_sym(sym_enumerator_list);
  }
};

struct MnComment : public MnNode {
  MnComment(){};
  MnComment(const MnNode& n) : MnNode(n) { check_sym(sym_comment); }
};

struct MnCaseStatement : public MnNode {
  MnCaseStatement(){};
  MnCaseStatement(const MnNode& n) : MnNode(n) {
    check_sym(sym_case_statement);
  }
};

struct MnSwitchStatement : public MnNode {
  MnSwitchStatement(){};
  MnSwitchStatement(const MnNode& n) : MnNode(n) {
    check_sym(sym_switch_statement);
  }
};

struct MnUsingDecl : public MnNode {
  MnUsingDecl(){};
  MnUsingDecl(const MnNode& n) : MnNode(n) { check_sym(sym_using_declaration); }
};

struct MnBreakStatement : public MnNode {
  MnBreakStatement(){};
  MnBreakStatement(const MnNode& n) : MnNode(n) {
    check_sym(sym_break_statement);
  }
};

struct MnCondExpr : public MnNode {
  MnCondExpr(){};
  MnCondExpr(const MnNode& n) : MnNode(n) {
    check_sym(sym_conditional_expression);
  }
};

struct MnStorageSpec : public MnNode {
  MnStorageSpec(){};
  MnStorageSpec(const MnNode& n) : MnNode(n) {
    check_sym(sym_storage_class_specifier);
  }
};

struct MnQualifiedId : public MnNode {
  MnQualifiedId(){};
  MnQualifiedId(const MnNode& n) : MnNode(n) {
    check_sym(sym_qualified_identifier);
  }
};

struct MnIfStatement : public MnNode {
  MnIfStatement(){};
  MnIfStatement(const MnNode& n) : MnNode(n) { check_sym(sym_if_statement); }
};

struct MnSizedTypeSpec : public MnNode {
  MnSizedTypeSpec(){};
  MnSizedTypeSpec(const MnNode& n) : MnNode(n) {
    check_sym(sym_sized_type_specifier);
  }
};

struct MnNamespaceDef : public MnNode {
  MnNamespaceDef(){};
  MnNamespaceDef(const MnNode& n) : MnNode(n) {
    check_sym(sym_namespace_definition);
  }
};

//------------------------------------------------------------------------------
