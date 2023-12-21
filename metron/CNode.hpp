// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <typeinfo>
#include <assert.h>

#include "matcheroni/Parseroni.hpp"
#include "metrolib/core/Err.h"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metron/CToken.hpp"
#include "metron/MtUtils.h"

struct Cursor;
struct CSourceRepo;
struct CNode;

struct CInstCall;
struct CInstance;
struct CInstClass;

typedef matcheroni::Span<CToken> TokenSpan;

#define NODE_ERR(A) { LOG_R("bad bad %s : %s\n", typeid(*this).name(), A); assert(false); exit(-1); }

//------------------------------------------------------------------------------

struct CNode : public parseroni::NodeBase<CNode, CToken> {

  CNode() {}
  virtual ~CNode() {}

  //----------------------------------------

  virtual CSourceRepo* get_repo() {
    return node_parent ? node_parent->get_repo() : nullptr;
  }

  std::string get_textstr() const {
    auto n = get_text();
    return std::string(n.begin(), n.end());
  }

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

  const std::string_view get_gap_prev() const {
    const char* a = (span.begin - 1)->lex->text_end;
    const char* b = (span.begin - 0)->lex->text_begin;
    return std::string_view(a, b);
  }

  const std::string_view get_gap_next() const {
    const char* a = (span.end - 1)->lex->text_end;
    const char* b = (span.end - 0)->lex->text_begin;
    return std::string_view(a, b);
  }

  bool tag_noconvert() const {
    return get_gap_prev().find("metron_noconvert") != std::string_view::npos;
  }

  bool tag_internal() const {
    return get_gap_prev().find("metron_internal") != std::string_view::npos;
  }

  bool tag_noemit() const {
    return get_gap_prev().find("metron_noemit") != std::string_view::npos;
  }

  //----------------------------------------

  template <typename P>
  P* as() {
    return dynamic_cast<P*>(this);
  }

  template <typename P>
  P* opt() {
    auto result1 = dynamic_cast<CNode*>(this);
    if (result1 == nullptr) return nullptr;

    auto result2 = dynamic_cast<P*>(this);
    assert(result2);
    return result2;
  }

  template <typename P>
  P* req() {
    auto result2 = dynamic_cast<P*>(this);
    assert(result2);
    return result2;
  }

  //----------------------------------------

  template<typename P>
  P* ancestor() {
    if (auto p = dynamic_cast<P*>(this)) return p;
    return node_parent ? node_parent->ancestor<P>() : nullptr;
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

  std::string name = "<CNode>";
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

inline void visit_children(CNode* n, node_visitor visit) {
  for (auto child = n->child_head; child; child = child->node_next) {
    visit(child);
    visit_children(child, visit);
  }
}

inline void visit(CNode* n, node_visitor visit_cb) {
  visit_cb(n);
  for (auto child = n->child_head; child; child = child->node_next) {
    visit(child, visit_cb);
  }
}

//------------------------------------------------------------------------------
