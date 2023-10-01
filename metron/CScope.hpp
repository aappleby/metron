// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once
#include <vector>
#include <string>
#include <set>
#include "matcheroni/Matcheroni.hpp"

struct CToken;
struct CContext;
typedef matcheroni::Span<CToken> TokenSpan;

//------------------------------------------------------------------------------

struct CScope {

  using token_list = std::vector<matcheroni::TextSpan>;

  void merge(CScope* other);

  void clear();
  bool has_type(CContext& ctx, TokenSpan body, token_list& types);
  void add_type(CContext& ctx, const CToken* a, token_list& types);

  bool has_class  (CContext& ctx, TokenSpan body);
  bool has_struct (CContext& ctx, TokenSpan body);
  bool has_union  (CContext& ctx, TokenSpan body);
  bool has_enum   (CContext& ctx, TokenSpan body);
  bool has_typedef(CContext& ctx, TokenSpan body);

  void add_class  (CContext& ctx, const CToken* a);
  void add_struct (CContext& ctx, const CToken* a);
  void add_union  (CContext& ctx, const CToken* a);
  void add_enum   (CContext& ctx, const CToken* a);
  void add_typedef(CContext& ctx, const CToken* a);

  void add_typedef(const char* t);

  CScope* parent2;
  token_list class_types;
  token_list struct_types;
  token_list union_types;
  token_list enum_types;
  token_list typedef_types;

  std::set<std::string> local_names;
  std::set<std::string> param_names;
};

//------------------------------------------------------------------------------
