// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "CToken.hpp"
#include "matcheroni/Parseroni.hpp"
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"
#include <string>
#include <typeinfo>
#include <string_view>
#include <functional>

struct Cursor;

typedef matcheroni::Span<CToken> TokenSpan;

//------------------------------------------------------------------------------

struct CNode : public parseroni::NodeBase<CNode, CToken> {

  using AtomType = CToken;
  using SpanType = matcheroni::Span<CToken>;

  CNode() {}
  virtual ~CNode() {}

  virtual std::string_view get_name() const { return "<CNode>"; }

  const char* text_begin() const { return span.begin->text.begin; }
  const char* text_end() const   { return (span.end - 1)->text.end; }

  matcheroni::TextSpan as_text_span() const {
    return matcheroni::TextSpan(text_begin(), text_end());
  }

  const std::string_view get_text() const {
    return std::string_view(text_begin(), text_end());
  }

  CHECK_RETURN virtual Err emit(Cursor& c);

  //----------------------------------------

  virtual uint32_t debug_color() const { return 0x999999; }
  void dump_tree(int max_depth = 0) const;
  void debug_dump(std::string& out);

  //----------------------------------------

  template <typename P>
  bool is_a() const {
    return typeid(*this) == typeid(P);
  }

  template <typename P>
  P* as_a() {
    return dynamic_cast<P*>(this);
  }

  CNode* child(const char* match_name) {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (strcmp(match_name, cursor->match_name) == 0) return cursor;
    }
    return nullptr;
  }

  const CNode* child(const char* match_name) const {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (strcmp(match_name, cursor->match_name) == 0) return cursor;
    }
    return nullptr;
  }

  template<typename P>
  P* child_as(const char* match_name) {
    return child(match_name)->as_a<P>();
  }

  //----------------------------------------

  // -2 = prefix, -1 = right-to-left, 0 = none, 1 = left-to-right, 2 = suffix
  int assoc = 0;
  int precedence = 0;
};

//------------------------------------------------------------------------------

struct CNodeIterator {
  CNodeIterator(CNode* cursor) : n(cursor) {}

  CNodeIterator& operator++() {
    n = n->node_next;
    return *this;
  }

  bool operator!=(CNodeIterator& b) const { return n != b.n; }

  CNode*       operator*()       { return n; }
  const CNode* operator*() const { return n; }

  CNode* n;
};

inline CNodeIterator begin(CNode* parent) {
  return CNodeIterator(parent->child_head);
}

inline CNodeIterator end(CNode* parent) {
  return CNodeIterator(nullptr);
}

//------------------------------------------------------------------------------

typedef std::function<void(CNode*)> node_visitor;

inline void visit(CNode* n, node_visitor v) {
  v(n);
  for (auto c = n->child_head; c; c = c->node_next) {
    visit(c, v);
  }
}

//------------------------------------------------------------------------------
