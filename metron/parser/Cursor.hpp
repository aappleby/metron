#pragma once

#include <map>
#include <stack>
#include <vector>
#include <functional>
#include <string>
#include <string_view>

#include "CSourceRepo.hpp"
#include "CSourceFile.hpp"

#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"

struct CNode;

struct Cursor {

  Cursor(CSourceRepo* repo, CSourceFile* source, std::string* out);

  //----------------------------------------

  CHECK_RETURN Err check_at(CNode* n);
  CHECK_RETURN Err check_done(CNode* n);

  //----------------------------------------

  //void push_indent(MnNode n);
  //void pop_indent(MnNode n);
  CHECK_RETURN Err start_line();
  CHECK_RETURN Err emit_backspace();
  CHECK_RETURN Err emit_indent();
  CHECK_RETURN Err emit_char(char c, uint32_t color = 0);
  CHECK_RETURN Err skip_char(char c);
  CHECK_RETURN Err emit_to(const char* b);
  CHECK_RETURN Err emit_span(const char* a, const char* b);
  CHECK_RETURN Err skip_span(const char* a, const char* b);
  CHECK_RETURN Err emit_vprint(const char* fmt, va_list args);
  CHECK_RETURN Err emit_line(const char* fmt, ...);
  CHECK_RETURN Err emit_print(const char* fmt, ...);
  CHECK_RETURN Err emit_string(const std::string_view& s);
  CHECK_RETURN Err emit_replacement(CNode* n, const std::string& s);

#if 0

  void blah() {
    emit_replacements(this, "#include", "`include", ".h", ".sv");
  }

  CHECK_RETURN Err emit_replacement(std::string_view text, std::string_view s_old, std::string_view s_new) {
    while (text.size()) {
      if (text.starts_with(s_old)) {
        for (auto c : s_new) err << emit_char(c, 0x80FF80);
        text.remove_prefix(s_old.size());
        continue;
      }

      err << emit_replacement_step(text);
    }
  }

  CHECK_RETURN Err emit_replacement_step(std::string_view& text, std::string_view s_old, std::string_view s_new) {
    Err err;
    if (text.starts_with(s_old)) {
      for (auto c : s_new) err << emit_char(c, 0x80FF80);
      text.remove_prefix(s_old.size());
    }
    return err;
  }

  CHECK_RETURN Err emit_replacement(std::string_view& text) {
    Err err;
    err << emit_char(text[0]);
    text.remove_prefix(1);
    return err;
  }

#endif

  //----------------------------------------
  // Top-level emit function

  CHECK_RETURN Err emit_everything();

  CHECK_RETURN Err emit_gap(CNode* a, CNode* b);
  CHECK_RETURN Err skip_gap(CNode* a, CNode* b);
  CHECK_RETURN Err emit_dispatch(CNode* node);

  CHECK_RETURN Err emit_trailing_whitespace() {
    Err err;
    auto& text = source_file->source_code;
    auto source_span = matcheroni::utils::to_span(text);
    while(text_cursor < source_span.end) {
      err << emit_char(*text_cursor++);
    }
    return err;
  }

  //----------------------------------------

  const char* text_cursor = nullptr;
  std::stack<const char*> cursor_stack;

  void push_cursor(const char* new_cursor) {
    cursor_stack.push(text_cursor);
    text_cursor = new_cursor;
  }

  void pop_cursor() {
    text_cursor = cursor_stack.top();
    cursor_stack.pop();
  }

  //----------------------------------------

  CSourceRepo* repo = nullptr;
  CSourceFile* source_file = nullptr;

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
