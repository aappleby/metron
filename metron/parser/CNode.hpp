// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "CToken.hpp"
#include "matcheroni/Parseroni.hpp"
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"
#include <cstddef>  // for size_t
#include <string>
#include <typeinfo>
#include <string_view>

struct Cursor;

typedef matcheroni::Span<CToken> TokenSpan;

//------------------------------------------------------------------------------

struct CNode : public parseroni::NodeBase<CNode, CToken> {
  virtual ~CNode() {}

  using AtomType = CToken;
  using SpanType = matcheroni::Span<CToken>;

  const char* text_begin() const {
    return span.begin->text.begin;
  }

  const char* text_end() const {
    return (span.end - 1)->text.end;
  }

  matcheroni::TextSpan as_text_span() const {
    return matcheroni::TextSpan(text_begin(), text_end());
  }

  std::string as_string() {
    return std::string(text_begin(), text_end());
  }

  std::string_view as_string_view() {
    return std::string_view(text_begin(), text_end());
  }

  //----------------------------------------

  virtual uint32_t debug_color() const { return 0x999999; }

  void dump_tree(int max_depth = 0) const;

  void debug_dump(std::string& out) {
    out += "[";
    out += match_name;
    out += ":";
    if (child_head) {
      for (auto c = child_head; c; c = c->node_next) {
        c->debug_dump(out);
      }
    } else {
      out += '`';
      out += as_string();
      out += '`';
    }
    out += "]";
  }

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

  template<typename P>
  P* child_as(const char* match_name) {
    return child(match_name)->as_a<P>();
  }

  //----------------------------------------

  CHECK_RETURN virtual Err emit(Cursor& c);

  CHECK_RETURN Err emit_default(Cursor& cursor);

  CHECK_RETURN Err emit_children(Cursor& cursor);

  CHECK_RETURN Err emit_rest(Cursor& cursor);


  CHECK_RETURN Err emit_gap_after(Cursor& cursor);

  //----------------------------------------

  /*
  template <typename P>
  P* child() {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (cursor->is_a<P>()) {
        return dynamic_cast<P*>(cursor);
      }
    }
    return nullptr;
  }

  template <typename P>
  const P* child() const {
    for (auto cursor = child_head; cursor; cursor = cursor->node_next) {
      if (cursor->is_a<P>()) {
        return dynamic_cast<const P*>(cursor);
      }
    }
    return nullptr;
  }

  template <typename P>
  const P* as_a() const {
    return dynamic_cast<const P*>(this);
  }
  */

  //----------------------------------------

  int precedence = 0;

  // -2 = prefix, -1 = right-to-left, 0 = none, 1 = left-to-right, 2 = suffix
  int assoc = 0;
};

//------------------------------------------------------------------------------

#if 1
template<typename NodeType>
struct NodeIterator {
  NodeIterator(NodeType* cursor) : n(cursor) {}
  NodeIterator& operator++() {
    n = n->node_next;
    return *this;
  }
  bool operator!=(NodeIterator& b) const { return n != b.n; }
  NodeType* operator*() const { return n; }
  NodeType* n;
};

template<typename NodeType>
inline NodeIterator<NodeType> begin(NodeType* parent) {
  return NodeIterator<NodeType>(parent->child_head);
}

template<typename NodeType>
inline NodeIterator<NodeType> end(NodeType* parent) {
  return NodeIterator<NodeType>(nullptr);
}

template<typename NodeType>
struct ConstNodeIterator {
  ConstNodeIterator(const NodeType* cursor) : n(cursor) {}
  ConstNodeIterator& operator++() {
    n = n->node_next;
    return *this;
  }
  bool operator!=(const ConstNodeIterator& b) const { return n != b.n; }
  const NodeType* operator*() const { return n; }
  const NodeType* n;
};

template<typename NodeType>
inline ConstNodeIterator<NodeType> begin(const NodeType* parent) {
  return ConstNodeIterator<NodeType>(parent->child_head);
}

template<typename NodeType>
inline ConstNodeIterator<NodeType> end(const NodeType* parent) {
  return ConstNodeIterator<NodeType>(nullptr);
}
#endif

//------------------------------------------------------------------------------
