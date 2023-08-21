// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "CToken.hpp"
#include "matcheroni/Matcheroni.hpp"

#include <vector>

//------------------------------------------------------------------------------

struct CLexer {
  CLexer();
  void reset();
  bool lex(matcheroni::TextSpan text);

  CToken next_lexeme(matcheroni::TextMatchContext& ctx, matcheroni::TextSpan body);

  std::vector<CToken> tokens;
  int current_row = 0;
  int current_col = 0;
  int current_indent = 0;
  bool in_indent = true;
};


//------------------------------------------------------------------------------
