// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include <string>
#include <vector>
#include <map>

#include "matcheroni/Matcheroni.hpp"
#include "metron/CToken.hpp"

//------------------------------------------------------------------------------

/*

chunk = (chunk fork chunk fork chunk fork)

*/

struct Chunk {
  Lexeme preproc;
  std::vector<Lexeme> lexemes;

  Chunk* parent = nullptr;
  std::vector<Chunk*> children;
};

//------------------------------------------------------------------------------

struct CLexer {
  using Span = matcheroni::TextSpan;
  using Context = matcheroni::TextMatchContext;

  bool lex(const std::string& source, std::vector<Lexeme>& out_lexemes);
  bool lex2(const std::string& source, std::vector<Chunk>& out_chunks);

  Lexeme next_lexeme();

  Chunk* chunk_root = nullptr;

  Context ctx;
  std::map<std::string, std::string> defines;

  Span source_span;

  int current_row = 0;
  int current_col = 0;
  int current_indent = 0;
  bool in_indent = true;
};


//------------------------------------------------------------------------------
