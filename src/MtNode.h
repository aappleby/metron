#pragma once
#include <assert.h>

#include <functional>
#include <string>

#include "Err.h"
#include "MtUtils.h"
#include "Platform.h"
#include "TreeSymbols.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

struct MtSourceFile;

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
    Err err;
    err << ERR("Dumping tree for %s\n", ts_node_type());
    dump_tree(0, 0, 255);
  }

  operator bool() const { return !ts_node_is_null(node); }

  const char* ts_node_type() const { return ::ts_node_type(node); }
  uint32_t start_byte() const { return ts_node_start_byte(node); }
  uint32_t end_byte() const { return ts_node_end_byte(node); }

  const char* start() const;
  const char* end() const;
  std::string text() const;
  bool match(const char* s);
  bool contains(const char* s) const;

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

inline MnConstIterator begin(const MnNode& parent) {
  return MnConstIterator(parent);
}

inline MnConstIterator end(const MnNode& parent) {
  return MnConstIterator(MnNode::null);
}

//------------------------------------------------------------------------------
