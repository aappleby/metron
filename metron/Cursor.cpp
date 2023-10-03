#include "metron/Cursor.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

Cursor::Cursor(CSourceFile* source, std::string* str_out) {
  this->source_file = source;
  this->str_out = str_out;

  // Skip LEX_BOF
  this->tok_cursor = source->context.root_node->span.begin;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_gap() {
  Err err;

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

  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_gap() {
  Err err;

  if (tok_cursor->lex_type() == LEX_EOF) return err;

  auto ta = tok_cursor - 1;
  auto tb = tok_cursor;

  for (auto c = ta->text_end(); c < tb->text_begin(); c++) {
    err << skip_char(*c);
  }
  line_elided = true;
  return err;
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
  return err;
}

//------------------------------------------------------------------------------
// Generic emit() methods

CHECK_RETURN Err Cursor::start_line() {
  Err err;
  if (line_dirty) {
    err << emit_char('\n');
    for (int i = 0; i < indent_level; i++) {
      err << emit_print("  ");
    }
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
    assert(c);
    str_out->push_back(c);
  }

  if (echo) {
    auto d = c;
    //if (color && d == ' ') d = '_';
    if (d == ' ') d = '_';
    LOG_C(color, "%c", d);
  }

  at_comma = c == ',';
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_char(char c) {
  Err err;
  // We ditch all '\r'.
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

CHECK_RETURN Err Cursor::emit_print(const char* fmt, ...) {
  Err err;
  va_list args;
  va_start(args, fmt);

  va_list args2;
  va_copy(args2, args);
  int size = vsnprintf(nullptr, 0, fmt, args2);
  va_end(args2);

  auto buf = new char[size + 1];
  vsnprintf(buf, size_t(size) + 1, fmt, args);
  va_end(args);

  for (int i = 0; i < size; i++) {
    err << emit_char(buf[i]);
  }
  delete[] buf;

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

//------------------------------------------------------------------------------
