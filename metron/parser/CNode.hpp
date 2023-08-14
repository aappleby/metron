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

  bool starts_with(const char* lit) {
    auto s = as_text_span();

    while (1) {
      auto ca = s.begin == s.end ? 0 : *s.begin;
      auto cb = *lit;
      if (cb == 0)  return true;
      if (ca != cb) return false;
      s.begin++;
      lit++;
    }
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

  CHECK_RETURN Err emit_children(Cursor& cursor);

  CHECK_RETURN Err emit_rest(Cursor& cursor);

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
