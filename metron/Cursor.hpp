#pragma once

#include <map>
#include <stack>
#include <vector>
#include <functional>
#include <string>
#include <string_view>

#include "metron/CSourceFile.hpp"

#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"

struct CNode;
struct CNodeCall;

struct Cursor {

  Cursor(CSourceFile* source, std::string* out);

  //----------------------------------------

  Err emit_token(const CToken* a);
  Err skip_token(const CToken* a);

  Err emit_gap();
  Err skip_gap();

  Err emit_span(const CToken* a, const CToken* b);
  Err skip_span(const CToken* a, const CToken* b);

  Err emit_lexeme(Lexeme* l);

  Err start_line();
  Err emit_backspace();
  Err emit_vprint(const char* fmt, va_list args);
  Err emit_print(const char* fmt, ...);
  Err emit_replacement(const CToken* a, const CToken* b, const std::string& r);

  //----------------------------------------

  CSourceFile* source_file = nullptr;
  const CToken* tok_cursor = nullptr;
  std::string* str_out;

  int indent_level = 0;
  bool at_comma = false;
  bool line_dirty = false;
  bool line_elided = false;
  bool echo = false;

  //----------------------------------------

private:

  Err emit_char(char c, uint32_t color = 0);
  Err skip_char(char c);
  Err emit_span(const char* a, const char* b);
};
