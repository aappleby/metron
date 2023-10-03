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

  CHECK_RETURN Err emit_gap();
  CHECK_RETURN Err skip_gap();

  // Token-level emit()
  CHECK_RETURN Err emit_token(const CToken* a);
  CHECK_RETURN Err emit_span(const CToken* a, const CToken* b);

  Err emit_lexeme(Lexeme* l) {
    Err err;
    for (auto c = l->text_begin; c < l->text_end; c++) {
      err << emit_char(*c);
    }
    return err;
  }

  // Char-level emit()
  CHECK_RETURN Err start_line();
  CHECK_RETURN Err emit_backspace();
  CHECK_RETURN Err emit_print(const char* fmt, ...);

  Err emit_replacement(const CToken* a, const CToken* b, const std::string& r) {
    Err err;
    for (auto c : r) {
      err << emit_char(c, 0x80FFFF);
    }
    tok_cursor = b;
    return err;
  }

  Err skip_span(const CToken* a, const CToken* b) {
    Err err;
    auto begin = a->text_begin();
    auto end   = (b-1)->text_end();

    for (auto c = begin; c < end; c++) {
      err << skip_char(*c);
    }
    tok_cursor = b;
    return err;
  }

  //----------------------------------------

  CSourceFile* source_file = nullptr;
  const CToken* tok_cursor = nullptr;

  std::string* str_out;
  int indent_level = 0;
  bool at_comma = false;
  bool line_dirty = false;
  bool line_elided = false;
  bool echo = false;

private:

  CHECK_RETURN Err emit_char(char c, uint32_t color = 0);
  CHECK_RETURN Err skip_char(char c);
  CHECK_RETURN Err emit_span(const char* a, const char* b);
};
