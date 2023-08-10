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

  std::vector<CToken> tokens;
};

CToken next_lexeme(matcheroni::TextMatchContext& ctx, matcheroni::TextSpan body);

//------------------------------------------------------------------------------