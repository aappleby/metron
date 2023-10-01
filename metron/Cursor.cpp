#include "metron/Cursor.hpp"

#include "metrolib/core/Log.h"
#include "metron/Emitter.hpp"

//------------------------------------------------------------------------------

Cursor::Cursor(CSourceRepo* repo, CSourceFile* source, std::string* str_out) {
  this->repo = repo;
  this->source_file = source;
  this->str_out = str_out;

  this->lex_begin = source->context.lexemes.data();
  this->lex_end = this->lex_begin + source->context.lexemes.size();

  const CToken* actual_tok_begin = source->context.root_node->span.begin;
  const CToken* actual_tok_end   = source->context.root_node->span.end;

  //this->tok_begin  = source->context.tokens.data();
  //this->tok_end    = this->tok_begin + source->context.tokens.size();
  this->tok_begin = actual_tok_begin;
  this->tok_end = actual_tok_end;

  // Skip LEX_BOF
  this->tok_cursor = this->tok_begin;
  this->gap_emitted = false;

  id_map.push({
    {"signed",         "$signed"},
    {"unsigned",       "$unsigned"},
    {"clog2",          "$clog2" },
    {"pow2",           "2**" },
    {"readmemh",       "$readmemh" },
    {"value_plusargs", "$value$plusargs" },
    {"write",          "$write" },
    {"sign_extend",    "$signed" },
    {"zero_extend",    "$unsigned" },
    {"DONTCARE",       "'x" },
    {"#include",       "`include"},
    {"#define",        "`define"},
    {"#ifndef",        "`ifndef"},
    {"#endif",         "`endif"},
    {"unsigned int",   "int unsigned"},
  });

  block_prefix.push("begin");
  block_suffix.push("end");
  override_size.push(0);

  elide_type.push(false);
  elide_value.push(false);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_gap() {
  Err err;
  assert(!gap_emitted);

  auto ta = tok_cursor - 1;
  auto tb = tok_cursor;

  auto la = ta->lex + 1;
  auto lb = tb->lex - 1;

  for(auto l = la; l <= lb; l++) {
    auto c_begin = l->text_begin;
    auto c_end = l->text_end;

    // Unwrap magic /*#foo#*/ comments to pass arbitrary text to Verilog.
    if (l->type == LEX_COMMENT) {
      if (l->get_text().starts_with("/*#") && l->get_text().ends_with("#*/")) {
        c_begin += 3;
        c_end -= 3;
      }
    }

    for (auto c = c_begin; c < c_end; c++) {
      err << emit_char(*c);
    }
  }

  gap_emitted = true;
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_gap() {
  Err err;
  assert(!gap_emitted);

  if (tok_cursor->lex_type() == LEX_EOF) return err;

  auto ta = tok_cursor - 1;
  auto tb = tok_cursor;

  for (auto c = ta->text_end(); c < tb->text_begin(); c++) {
    err << skip_char(*c);
  }
  line_elided = true;
  gap_emitted = true;
  return err;
}

//------------------------------------------------------------------------------

/*
CHECK_RETURN Err Cursor::emit(CNode* n) {
  //if (n == nullptr) return Err();
  assert(n);

  Err err = check_at(n);
  err << n->emit(*this);
  return err << check_done(n);
}
*/

CHECK_RETURN Err Cursor::skip_over(CNode* n) {
  //if (n == nullptr) return Err();
  assert(n);
  Err err = check_at(n);
  err << skip_span(n->tok_begin(), n->tok_end());
  return err << check_done(n);
}

CHECK_RETURN Err Cursor::skip_to(CNode* n) {
  assert(n);
  Err err;
  //if (n == nullptr) return err;
  while (tok_cursor < n->tok_begin()) err << skip_current_token();
  return err;
}

CHECK_RETURN Err Cursor::skip_current_token() {
  Err err;
  for (auto c = tok_cursor->text_begin(); c < tok_cursor->text_end(); c++) {
    err << skip_char(*c);
  }
  tok_cursor++;
  this->gap_emitted = false;
  return err;
}

CHECK_RETURN Err Cursor::comment_out(CNode* n) {
  //if (n == nullptr) return Err();
  assert(n);
  Err err = check_at(n);
  err << emit_print("/*");
  err << emit_span(n->tok_begin(), n->tok_end());
  err << emit_print("*/");
  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_rest(CNode* n) {
  return emit_span(tok_cursor, n->tok_end());
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::check_at(CNode* n) {

  if (tok_cursor != n->tok_begin()) {
    LOG_R("check_at - bad tok_cursor\n");
    LOG_R("  want @%.10s@\n", n->text_begin());
    LOG_R("  got  @%.10s@\n", tok_cursor->text_begin());
    assert(false);
  }

  return Err();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::check_done(CNode* n) {
  assert(!gap_emitted);

  auto tok_end = n->tok_end();

  if (tok_cursor < tok_end) {
    LOG_R("Token cursor was left inside the current node\n");
    assert(false);
  }

  if (tok_cursor > tok_end) {
    LOG_R("Token cursor was left past the end of the current node\n");
    assert(false);
  }

  return Err();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_token(const CToken* a) {
  auto begin = a->text_begin();
  auto end   = a->text_end();

  Err err;
  for (auto c = begin; c < end; c++) {
    err << emit_char(*c);
  }
  tok_cursor = a + 1;
  gap_emitted = false;
  return err;
}

CHECK_RETURN Err Cursor::emit_span(const CToken* a, const CToken* b) {
  Err err;
  assert(a != b);
  for (auto c = a; c < b; c++) {
    err << emit_token(c);
    if (c < b - 1) err << emit_gap();
  }
  tok_cursor = b;
  gap_emitted = false;
  return err;
}

//------------------------------------------------------------------------------
// Generic emit() methods

CHECK_RETURN Err Cursor::start_line() {
  Err err;
  if (line_dirty) {
    err << emit_char('\n');
    err << emit_indent();
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_indent() {
  Err err;
  for (int i = 0; i < indent_level; i++) {
    err << emit_print("  ");
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_backspace() {
  Err err;
  if (echo) {
    LOG_C(0x8080FF, "^H");
  }
  str_out->pop_back();
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_char(char c, uint32_t color) {
  Err err;
  // We ditch all '\r'.
  if (c == '\r') return err;

  if (c < 0 || !isspace(c)) {
    line_dirty = true;
  }

  if (c == '\n') {
    // Strip trailing whitespace
    while (str_out->size() && str_out->back() == ' ') {
      err << emit_backspace();
    }

    // Discard the line if it contained only whitespace after elisions
    if (!line_dirty && line_elided) {
      if (echo) {
        LOG_C(0xFF8080, " (Line elided)");
      }
      while (str_out->size() && str_out->back() != '\n') {
        str_out->pop_back();
      }
    } else {
      if (echo) {
        LOG_C(0xFFFFFF, "\\n");
      }
      str_out->push_back(c);
    }

    line_dirty = false;
    line_elided = false;
  } else {
    str_out->push_back(c);
  }

  if (echo) {
    auto d = c;
    //if (color && d == ' ') d = '_';
    if (d == ' ') d = '_';
    LOG_C(color, "%c", d);
  }

  at_newline = c == '\n';
  at_comma = c == ',';
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_char(char c) {
  Err err;
  if (c == '\r') return err;

  if (echo) {
    if (c == ' ') {
      LOG_C(0x8080FF, "_", c);
    }
    else if (c == '\n') {
      LOG_C(0x8080FF, "\\n\n", c);
    }
    else {
      LOG_C(0x8080FF, "%c", c);
    }
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_to(const CToken* b) {
  Err err;
  for (auto c = tok_cursor->text_begin(); c < b->text_end(); c++) {
    err << emit_char(*c);
  }
  tok_cursor = b;
  this->gap_emitted = false;
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_span(const CToken* a, const CToken* b) {
  auto begin = a->text_begin();
  auto end   = (b-1)->text_end();

  Err err;
  for (auto c = begin; c < end; c++) {
    err << skip_char(*c);
  }
  tok_cursor = b;
  this->gap_emitted = false;
  line_elided = true;
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_vprint(const char* fmt, va_list args) {
  Err err;

  va_list args2;
  va_copy(args2, args);
  int size = vsnprintf(nullptr, 0, fmt, args2);
  va_end(args2);

  auto buf = new char[size + 1];
  vsnprintf(buf, size_t(size) + 1, fmt, args);
  va_end(args);

  for (int i = 0; i < size; i++) {
    err << emit_char(buf[i], 0x80FF80);
  }
  delete[] buf;
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_print(const char* fmt, ...) {
  Err err;
  va_list args;
  va_start(args, fmt);
  err << emit_vprint(fmt, args);
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_string(const std::string_view& s) {
  Err err;
  for (auto c : s) {
    err << emit_char(c, 0x80FF80);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_span(const char* a, const char* b) {
  Err err;
  for (auto c = a; c < b; c++) {
    err << emit_char(*c);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_replacement(CNode* n, const std::string& s) {
  Err err = check_at(n);
  for (auto c : s) {
    err << emit_char(c, 0x80FFFF);
  }
  tok_cursor = n->tok_end();
  gap_emitted = false;
  return err << check_done(n);
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_replacement(CNode* n, const char* fmt, ...) {
  Err err = check_at(n);
  va_list args;
  va_start(args, fmt);
  err << emit_vprint(fmt, args);
  tok_cursor = n->tok_end();
  gap_emitted = false;
  return err << check_done(n);
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_splice(CNode* n) {
  Err err;
  Emitter emitter(*this);

  auto old_cursor = tok_cursor;
  tok_cursor = n->tok_begin();
  err << emitter.emit_dispatch(n);
  tok_cursor = old_cursor;
  return err;
}

CHECK_RETURN Err Cursor::emit_raw(CNode* n) {
  Err err;
  err << emit_span(n->tok_begin(), n->tok_end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_everything() {
  Err err;

  // Emit header

  for (auto lex_cursor = lex_begin; lex_cursor < tok_begin->lex; lex_cursor++) {
    for (auto c = lex_cursor->text_begin; c < lex_cursor->text_end; c++) {
      err << emit_char(*c);
    }
  }

  //err << source_file->context.root_node->emit(*this);

  err << Emitter(*this).emit_dispatch(source_file->context.root_node);

  // Emit footer (everything in the gap after the translation unit)
  err << emit_gap();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_trailing_whitespace() {
  Err err;
  while(tok_cursor < tok_end) {
    err << emit_token(tok_cursor);
    tok_cursor++;
    gap_emitted = false;
  }
  return err;
}

//------------------------------------------------------------------------------