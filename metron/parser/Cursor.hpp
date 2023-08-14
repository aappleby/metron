#pragma once

#include <map>
#include <stack>
#include <vector>
#include <functional>
#include <string>

#include "CSourceRepo.hpp"
#include "CSourceFile.hpp"

#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"

struct CNode;

struct Cursor {

  Cursor(CSourceRepo* repo, CSourceFile* source, std::string* out);

  //----------------------------------------

  //void push_indent(MnNode n);
  //void pop_indent(MnNode n);
  CHECK_RETURN Err start_line();
  CHECK_RETURN Err emit_backspace();
  CHECK_RETURN Err emit_indent();
  CHECK_RETURN Err emit_char(char c, uint32_t color = 0);
  CHECK_RETURN Err skip_char(char c);
  CHECK_RETURN Err emit_span(const char* a, const char* b);
  CHECK_RETURN Err skip_span(const char* a, const char* b);
  CHECK_RETURN Err emit_vprint(const char* fmt, va_list args);
  CHECK_RETURN Err emit_line(const char* fmt, ...);
  CHECK_RETURN Err emit_print(const char* fmt, ...);

  //----------------------------------------
  // Top-level emit function

  CHECK_RETURN Err emit_everything();

  //----------------------------------------

  const char* cursor = nullptr;
  std::stack<const char*> cursor_stack;

  void push_cursor(const char* new_cursor) {
    cursor_stack.push(cursor);
    cursor = new_cursor;
  }

  void pop_cursor() {
    cursor = cursor_stack.top();
    cursor_stack.pop();
  }

  //----------------------------------------

  CSourceRepo* repo = nullptr;
  CSourceFile* source = nullptr;
  std::string* out = nullptr;

  //----------------------------------------

  using string_to_node = std::map<std::string, CNode*>;
  using string_to_string = std::map<std::string, std::string>;

  std::stack<std::string> block_prefix;
  std::stack<std::string> block_suffix;
  std::stack<int> override_size;
  std::stack<string_to_node> preproc_vars;
  std::stack<string_to_string> id_map;
  std::stack<string_to_string> token_map;
  std::stack<bool> elide_type;
  std::stack<bool> elide_value;

  //----------------------------------------
  // Output state

  std::string* str_out;
  std::stack<std::string> indent;
  bool at_newline = true;
  bool at_comma = false;
  bool line_dirty = false;
  bool line_elided = false;
  bool echo = false;
};
