// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Parseroni.hpp"
#include "matcheroni/Utilities.hpp"

#include "CConstants.hpp"
#include "CToken.hpp"
#include "CLexer.hpp"
#include "CNode.hpp"
#include "CScope.hpp"
#include "SST.hpp"

#include <array>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <filesystem>

struct CToken;
struct CNode;
struct CContext;
struct CScope;
struct CSourceRepo;

using TokenSpan = matcheroni::Span<CToken>;

//------------------------------------------------------------------------------

class CContext : public parseroni::NodeContext<CNode> {
 public:

  using AtomType = CToken;
  using SpanType = matcheroni::Span<CToken>;
  using NodeType = CNode;

  CContext();

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
    type_scope->add_struct(*this, body.begin);
    return body;
  }


  void push_scope();
  void pop_scope();

  void append_node(CNode* node);
  void enclose_nodes(CNode* start, CNode* node);

  //----------------------------------------

  matcheroni::TextSpan handle_include(matcheroni::TextSpan body);

  //----------------------------------------

  CSourceRepo* repo;

  std::string source;
  std::vector<Lexeme> lexemes;
  std::vector<CToken> tokens;

  CScope* type_scope;

  CNode* root_node = nullptr;
};

//------------------------------------------------------------------------------
