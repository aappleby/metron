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

  // Node-level emit()
  CHECK_RETURN Err emit(CNode* n);
  CHECK_RETURN Err skip_over(CNode* n);
  CHECK_RETURN Err comment_out(CNode* n);
  CHECK_RETURN Err emit_default(CNode* n);
  CHECK_RETURN Err emit_children(CNode* n);
  CHECK_RETURN Err emit_rest(CNode* n);
  CHECK_RETURN Err emit_gap_after(CNode* n);
  CHECK_RETURN Err skip_gap_after(CNode* n);
  CHECK_RETURN Err emit_replacement(CNode* n, const std::string& s);
  CHECK_RETURN Err emit_replacement(CNode* n, const char* fmt, ...);
  CHECK_RETURN Err emit_splice(CNode* n);
  CHECK_RETURN Err emit_raw(CNode* n);
  CHECK_RETURN Err emit_indent();

  // Token-level emit()
  CHECK_RETURN Err emit_token(const CToken* a);
  CHECK_RETURN Err emit_span(const CToken* a, const CToken* b);
  CHECK_RETURN Err skip_span(const CToken* a, const CToken* b);
  CHECK_RETURN Err emit_to(const CToken* b);

  // Char-level emit()
  CHECK_RETURN Err start_line();
  CHECK_RETURN Err emit_backspace();
  CHECK_RETURN Err emit_char(char c, uint32_t color = 0);
  CHECK_RETURN Err skip_char(char c);
  CHECK_RETURN Err emit_vprint(const char* fmt, va_list args);
  CHECK_RETURN Err emit_print(const char* fmt, ...);
  CHECK_RETURN Err emit_string(const std::string_view& s);


  //----------------------------------------

  CHECK_RETURN Err emit_replacement_step(std::string_view& text, std::string_view s_old, std::string_view s_new) {
    Err err;
    if (text.starts_with(s_old)) {
      for (auto c : s_new) {
        err << emit_char(c, 0x80FFFF);
      }
      text.remove_prefix(s_old.size());
    }
    else {
      err << emit_char(text[0]);
      text.remove_prefix(1);
    }
    return err;
  }

  template<typename ... Args>
  CHECK_RETURN Err emit_replacement_step(std::string_view& text, std::string_view s_old, std::string_view s_new, Args... args) {
    Err err;
    if (text.starts_with(s_old)) {
      for (auto c : s_new) {
        err << emit_char(c, 0x80FFFF);
      }
      text.remove_prefix(s_old.size());
    }
    else {
      err << emit_replacement_step(text, args...);
    }
    return err;
  }

  template<typename ... Args>
  CHECK_RETURN Err emit_replacements(CNode* n, Args... args) {
    Err err;
    auto text = n->get_text();
    while (text.size()) {
      err << emit_replacement_step(text, args...);
    }
    tok_cursor = n->tok_end();
    return err;
  }

  //----------------------------------------
  // Top-level emit function

  CHECK_RETURN Err emit_everything();

  CHECK_RETURN Err emit_gap(CNode* a, CNode* b);
  CHECK_RETURN Err skip_gap(CNode* a, CNode* b);

  CHECK_RETURN Err emit_trailing_whitespace();

  //----------------------------------------

  void push_cursor(const CToken* new_cursor);
  void pop_cursor();

  //----------------------------------------

  CSourceRepo* repo = nullptr;
  CSourceFile* source_file = nullptr;

  // FIXME need to switch from text cursor to token cursor...

  std::stack<const CToken*> cursor_stack;

  const CToken* tok_begin = nullptr;
  const CToken* tok_end = nullptr;
  const CToken* tok_cursor = nullptr;

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
  int indent_level = 0;
  bool at_newline = true;
  bool at_comma = false;
  bool line_dirty = false;
  bool line_elided = false;
  bool echo = false;
};
