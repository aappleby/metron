// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Parseroni.hpp"
#include "matcheroni/Utilities.hpp"

#include "metron/CConstants.hpp"
#include "metron/CToken.hpp"
#include "metron/CLexer.hpp"
#include "metron/CNode.hpp"
#include "metron/CScope.hpp"
#include "metron/SST.hpp"

#include <array>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <filesystem>

struct CToken;
struct CNode;
struct CScope;
class CSourceRepo;
class CSourceFile;

using TokenSpan = matcheroni::Span<CToken>;

//------------------------------------------------------------------------------

class CContext : public parseroni::NodeContext<CNode> {
 public:

  using AtomType = CToken;
  using SpanType = matcheroni::Span<CToken>;
  using NodeType = CNode;

  CContext();

  //----------------------------------------

  static int atom_cmp(char a, int b) {
    return (unsigned char)a - b;
  }

  static int atom_cmp(const CToken& a, const LexemeType& b) {
    return a.lex->type - b;
  }

  static int atom_cmp(const CToken& a, const char& b) {
    if (auto d = a.lex->len() - 1) return d;
    return a.lex->text_begin[0] - b;
  }

  static int atom_cmp(const CToken& a, const matcheroni::TextSpan& b) {
    return strcmp_span(matcheroni::TextSpan(a.lex->text_begin, a.lex->text_end), b);
  }

  //----------------------------------------

  void reset();
  TokenSpan parse();

  TokenSpan match_builtin_type_base  (TokenSpan body);
  TokenSpan match_builtin_type_prefix(TokenSpan body);
  TokenSpan match_builtin_type_suffix(TokenSpan body);

  TokenSpan match_class_name  (TokenSpan body);
  TokenSpan match_struct_name (TokenSpan body);
  TokenSpan match_union_name  (TokenSpan body);
  TokenSpan match_enum_name   (TokenSpan body);
  TokenSpan match_typedef_name(TokenSpan body);

  // matches any of the above
  TokenSpan match_type(TokenSpan body);

  void add_class  (const CToken* a);
  void add_struct (const CToken* a);
  void add_union  (const CToken* a);
  void add_enum   (const CToken* a);
  void add_typedef(const CToken* a);

  TokenSpan add_struct2(TokenSpan body) {
    top_scope->add_struct(*this, body.begin);
    return body;
  }

  void push_scope2(CScope* new_top) {
    new_top->parent2 = top_scope;
    top_scope = new_top;
  }

  void pop_scope2() {
    top_scope = top_scope->parent2;
  }

  //----------------------------------------

  TokenSpan handle_preproc(TokenSpan body);

  //----------------------------------------

  template<typename pattern>
  SpanType take() {
    auto tail = pattern::match(*this, span);
    if (!tail) return tail;
    SpanType head = { span.begin, tail.begin };
    span.begin = tail.begin;
    return head;
  }

  //----------------------------------------

  CSourceRepo* repo;
  CSourceFile* source_file = nullptr;

  CScope global_scope;
  CScope* top_scope = &global_scope;

  CNode* root_node = nullptr;

  std::string source;
  std::vector<Lexeme> lexemes;
  std::vector<CToken> tokens;

  TokenSpan span;

  std::set<std::string> define_names;
};

//------------------------------------------------------------------------------
