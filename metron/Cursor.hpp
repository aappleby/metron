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

  CHECK_RETURN Err check_at(CNode* n);
  CHECK_RETURN Err check_done(CNode* n);

  //----------------------------------------

  CHECK_RETURN Err emit_gap();
  CHECK_RETURN Err skip_gap();

  CHECK_RETURN Err emit_gap(CNode* n);
  CHECK_RETURN Err skip_gap(CNode* n);

  // Node-level emit()
  CHECK_RETURN Err skip_over(CNode* n);
  CHECK_RETURN Err skip_over2(CNode* n);
  CHECK_RETURN Err skip_to(CNode* n);
  CHECK_RETURN Err comment_out(CNode* n);
  CHECK_RETURN Err comment_out2(CNode* n);
  CHECK_RETURN Err emit_replacement(CNode* n, const std::string& s);
  CHECK_RETURN Err emit_replacement(CNode* n, const char* fmt, ...);
  CHECK_RETURN Err emit_replacement2(CNode* n, const char* fmt, ...);
  CHECK_RETURN Err emit_raw(CNode* n);
  CHECK_RETURN Err emit_raw2(CNode* n);

  // Token-level emit()
  CHECK_RETURN Err emit_token(const CToken* a);
  CHECK_RETURN Err emit_span(const CToken* a, const CToken* b);

  // Char-level emit()
  CHECK_RETURN Err start_line();
  CHECK_RETURN Err emit_backspace();
  CHECK_RETURN Err emit_char(char c, uint32_t color = 0);
  CHECK_RETURN Err skip_char(char c);
  CHECK_RETURN Err emit_vprint(const char* fmt, va_list args);
  CHECK_RETURN Err emit_print(const char* fmt, ...);
  CHECK_RETURN Err emit_span(const char* a, const char* b);

  //----------------------------------------

  CSourceFile* source_file = nullptr;
  const CToken* tok_cursor = nullptr;

  std::string* str_out;
  int indent_level = 0;
  bool at_comma = false;
  bool line_dirty = false;
  bool line_elided = false;
  bool echo = false;
};
