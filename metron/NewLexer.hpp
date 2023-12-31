// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include <string>
#include <vector>
#include <map>

#include "matcheroni/Matcheroni.hpp"
#include "metron/CToken.hpp"

class NewLexer {
  using Span = matcheroni::TextSpan;
  using Context = matcheroni::TextMatchContext;

  bool lex(const std::string& source, std::vector<Lexeme>& out_lexemes);

  Lexeme next_lexeme();

  Context ctx;
  std::map<std::string, std::string> defines;

  Span source_span;
  int  current_row = 0;
  int  current_col = 0;
  int  current_indent = 0;
  bool in_indent = true;
};
