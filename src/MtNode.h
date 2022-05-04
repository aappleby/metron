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

  bool is_identifier() const {
    switch (sym) {
      case alias_sym_field_identifier:
      case alias_sym_namespace_identifier:
      case alias_sym_statement_identifier:
      case alias_sym_type_identifier:
      case sym_dependent_field_identifier:
      case sym_dependent_identifier:
      case sym_dependent_type_identifier:
      case sym_identifier:
      case sym_qualified_field_identifier:
      case sym_qualified_identifier:
      case sym_qualified_operator_cast_identifier:
      case sym_qualified_type_identifier:
        return true;
      default:
        return false;
    }
  }

  bool is_expression() const {
    switch (sym) {
      case sym_conditional_expression:
      case sym_assignment_expression:
      case sym_unary_expression:
      case sym_binary_expression:
      case sym_update_expression:
      case sym_subscript_expression:
      case sym_call_expression:
      case sym_field_expression:
      case sym_compound_literal_expression:
      case sym_parenthesized_expression:
        return true;
      default:
        return false;
    }
  }

  bool is_statement() const {
    switch (sym) {
      case sym_break_statement:
      case sym_case_statement:
      case sym_compound_statement:
      case sym_continue_statement:
      case sym_do_statement:
      case sym_expression_statement:
      case sym_for_statement:
      case sym_goto_statement:
      case sym_if_statement:
      case sym_return_statement:
      case sym_switch_statement:
      case sym_while_statement:
        return true;
      default:
        return false;
    }
  }

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
  MnFunc(const MnNode& n) : MnNode(n) {}
};

struct MnCallExpr : public MnNode {
  MnCallExpr(){};
  MnCallExpr(const MnNode& n) : MnNode(n) {}
};

struct MnInitDecl : public MnNode {
  MnInitDecl(){};
  MnInitDecl(const MnNode& n) : MnNode(n) {}
};

struct MnTemplateType : public MnNode {
  MnTemplateType(){};
  MnTemplateType(const MnNode& n) : MnNode(n) {}
};

struct MnDecl : public MnNode {
  MnDecl(){};
  MnDecl(const MnNode& n) : MnNode(n) {}
};

struct MnEnumSpecifier : public MnNode {
  MnEnumSpecifier(){};
  MnEnumSpecifier(const MnNode& n) : MnNode(n) {}
};

struct MnType : public MnNode {
  MnType() {}
  MnType(const MnNode& n) : MnNode(n) {}
};

struct MnFieldDecl : public MnNode {
  MnFieldDecl(){};
  MnFieldDecl(const MnNode& n) : MnNode(n) {}
};

struct MnPreprocInclude : public MnNode {
  MnPreprocInclude(){};
  MnPreprocInclude(const MnNode& n) : MnNode(n) {}
};

struct MnNumberLiteral : public MnNode {
  MnNumberLiteral(){};
  MnNumberLiteral(const MnNode& n) : MnNode(n) {}
};

struct MnReturnStatement : public MnNode {
  MnReturnStatement(){};
  MnReturnStatement(const MnNode& n) : MnNode(n) {}
};

struct MnTemplateDecl : public MnNode {
  MnTemplateDecl(){};
  MnTemplateDecl(const MnNode& n) : MnNode(n) {}
};

struct MnTranslationUnit : public MnNode {
  MnTranslationUnit(){};
  MnTranslationUnit(const MnNode& n) : MnNode(n) {}
};

struct MnTemplateParamList : public MnNode {
  MnTemplateParamList(){};
  MnTemplateParamList(const MnNode& n) : MnNode(n) {}
};

struct MnEnumeratorList : public MnNode {
  MnEnumeratorList(){};
  MnEnumeratorList(const MnNode& n) : MnNode(n) {}
};

struct MnComment : public MnNode {
  MnComment(){};
  MnComment(const MnNode& n) : MnNode(n) {}
};

struct MnCaseStatement : public MnNode {
  MnCaseStatement(){};
  MnCaseStatement(const MnNode& n) : MnNode(n) {}
};

struct MnSwitchStatement : public MnNode {
  MnSwitchStatement(){};
  MnSwitchStatement(const MnNode& n) : MnNode(n) {}
};

struct MnUsingDecl : public MnNode {
  MnUsingDecl(){};
  MnUsingDecl(const MnNode& n) : MnNode(n) {}
};

struct MnBreakStatement : public MnNode {
  MnBreakStatement(){};
  MnBreakStatement(const MnNode& n) : MnNode(n) {}
};

struct MnCondExpr : public MnNode {
  MnCondExpr(){};
  MnCondExpr(const MnNode& n) : MnNode(n) {}
};

struct MnStorageSpec : public MnNode {
  MnStorageSpec(){};
  MnStorageSpec(const MnNode& n) : MnNode(n) {}
};

struct MnQualifiedId : public MnNode {
  MnQualifiedId(){};
  MnQualifiedId(const MnNode& n) : MnNode(n) {}
};

struct MnIfStatement : public MnNode {
  MnIfStatement(){};
  MnIfStatement(const MnNode& n) : MnNode(n) {}
};

struct MnSizedTypeSpec : public MnNode {
  MnSizedTypeSpec(){};
  MnSizedTypeSpec(const MnNode& n) : MnNode(n) {}
};

struct MnNamespaceDef : public MnNode {
  MnNamespaceDef(){};
  MnNamespaceDef(const MnNode& n) : MnNode(n) {}
};

//------------------------------------------------------------------------------
