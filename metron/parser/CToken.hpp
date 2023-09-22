// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once
#include <stdint.h>
#include <string_view>

#include "CConstants.hpp"
#include "matcheroni/Matcheroni.hpp"

//------------------------------------------------------------------------------

enum LexemeType {
  LEX_INVALID = 0,
  LEX_SPACE,
  LEX_NEWLINE,
  LEX_STRING,
  LEX_KEYWORD,
  LEX_IDENTIFIER,
  LEX_COMMENT,
  LEX_PREPROC,
  LEX_FLOAT,
  LEX_INT,
  LEX_PUNCT,
  LEX_CHAR,
  LEX_SPLICE,
  LEX_FORMFEED,
  LEX_BOF,
  LEX_EOF,
  LEX_LAST
};

//------------------------------------------------------------------------------

struct Lexeme {
  Lexeme(LexemeType type, const char* text_begin, const char* text_end) {
    this->type = type;
    this->text_begin = text_begin;
    this->text_end = text_end;
    this->row = -1;
    this->col = -1;
    this->indent = -1;
  }

  int len() const { return int(text_end - text_begin); }

  bool is_bof() const;
  bool is_eof() const;
  bool is_gap() const;

  const char* type_to_str() const;
  uint32_t type_to_color() const;
  void dump_lexeme() const;

  std::string_view get_text() const { return std::string_view(text_begin, text_end); }

  LexemeType type;
  const char* text_begin;
  const char* text_end;
  int row;
  int col;
  int indent;
};

//------------------------------------------------------------------------------

struct CToken {
  CToken(Lexeme* lex) : lex(lex) {}

  LexemeType lex_type() const { return lex->type; }

  const char* text_begin() const { return lex->text_begin; }
  const char* text_end() const   { return lex->text_end; }

  void dump_lexeme() {
    lex->dump_lexeme();
  }

  matcheroni::TextSpan as_text_span() const {
    return matcheroni::TextSpan(lex->text_begin, lex->text_end);
  }

  Lexeme* lex;
};

//------------------------------------------------------------------------------
