// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "CToken.hpp"
#include "matcheroni/Matcheroni.hpp"

#include <string>
#include <vector>

//------------------------------------------------------------------------------

struct CLexer {
  bool lex(const std::string& source, std::vector<Lexeme>& out_lexemes);

  Lexeme next_lexeme(matcheroni::TextMatchContext& ctx, matcheroni::TextSpan body);

  int current_row = 0;
  int current_col = 0;
  int current_indent = 0;
  bool in_indent = true;
};


//------------------------------------------------------------------------------
