// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "metrolib/core/Log.h"
#include "CToken.hpp"

#include "matcheroni/Parseroni.hpp"
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"
#include "metron/tools/MtUtils.h"

#include <string>
#include <typeinfo>
#include <string_view>
#include <functional>

struct Cursor;
struct CNodeClass;
struct CSourceRepo;
struct CInstance;

typedef matcheroni::Span<CToken> TokenSpan;

//------------------------------------------------------------------------------

struct CNode : public parseroni::NodeBase<CNode, CToken> {

  using AtomType = CToken;
  using SpanType = matcheroni::Span<CToken>;

  CNode() {}
  virtual ~CNode() {}

  //----------------------------------------
  // Methods to be implemented by subclasses.

  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
  virtual Err trace_children(CInstance* instance, TraceAction action);

  //----------------------------------------

  const CToken* tok_begin() const { return span.begin; }
  const CToken* tok_end() const   { return span.end; }

  const char* text_begin() const { return span.begin->lex->text_begin; }
  const char* text_end() const   { return (span.end - 1)->lex->text_end; }

  matcheroni::TextSpan as_text_span() const {
    return matcheroni::TextSpan(text_begin(), text_end());
  }

  const std::string_view get_text() const {
    return std::string_view(text_begin(), text_end());
  }

  const std::string get_string() const {
    return std::string(text_begin(), text_end());
  }


  //----------------------------------------

  void dump_tree(int max_depth = 0) const;

  //----------------------------------------

  template <typename P>
  P* as_a() {
    return dynamic_cast<P*>(this);
  }

  //----------------------------------------

  CNode* child(const char* tag) {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (cursor->tag_is(tag)) return cursor;
    }
    return nullptr;
  }

  const CNode* child(const char* tag) const {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (cursor->tag_is(tag)) return cursor;
    }
    return nullptr;
  }

  template<typename P>
  P* ancestor() {
    if (auto p = dynamic_cast<P*>(this)) return p;
    return node_parent ? node_parent->ancestor<P>() : nullptr;
  }

  template<typename P>
  P* child_as(const char* tag) {
    return child(tag)->as_a<P>();
  }

  template<typename P>
  P* child_is(const char* tag) {
    return child(tag)->as_a<P>();
  }

  template<typename P>
  P* child() {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (auto result = dynamic_cast<P*>(cursor)) return result;
    }
    return nullptr;
  }

  template<typename P>
  const P* child() const {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (auto result = dynamic_cast<const P*>(cursor)) return result;
    }
    return nullptr;
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
