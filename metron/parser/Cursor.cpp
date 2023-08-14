#include "Cursor.hpp"

#include "metrolib/core/Log.h"

Cursor::Cursor(CSourceRepo* repo, CSourceFile* source, std::string* out) {
  this->repo = repo;
  this->source = source;
  this->out = out;
}

//------------------------------------------------------------------------------

/*
void Cursor::push_indent(MnNode body) {
  assert(body.sym == sym_compound_statement ||
         body.sym == sym_field_declaration_list);

  auto n = body.first_named_child().node;

  if (ts_node_is_null(n)) {
    indent.push("");
    return;
  }

  if (ts_node_symbol(n) == sym_access_specifier) {
    n = ts_node_next_sibling(n);
  }

  if (ts_node_symbol(n) == anon_sym_COLON) {
    n = ts_node_next_sibling(n);
  }

  const char* begin = &current_source.top()->source[ts_node_start_byte(n)] - 1;
  const char* end = &current_source.top()->source[ts_node_start_byte(n)];

  std::string new_indent;

  while (*begin != '\n' && *begin != '{') begin--;
  if (*begin == '{') {
    new_indent = "";
  } else {
    new_indent = std::string(begin + 1, end);
  }

  for (auto& c : new_indent) {
    assert(isspace(c));
  }

  indent.push(new_indent);
}

void Cursor::pop_indent(MnNode class_body) { indent.pop(); }
*/

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

CHECK_RETURN Err Cursor::emit_backspace() {
  Err err;
  if (echo) {
    LOG_C(0x8080FF, "^H");
  }
  str_out->pop_back();
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_indent() {
  Err err;
  for (auto c : indent.top()) {
    err << emit_char(c);
  }
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
    if (color && d == ' ') d = '_';
    LOG_C(color, "%c", d);
  }

  at_newline = c == '\n';
  at_comma = c == ',';
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_char(char c) {
  Err err;
  if (echo) {
    if (c == ' ') c = '_';
    LOG_C(0x8080FF, "%c", c);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_span(const char* a, const char* b) {
  Err err;
  for (auto c = a; c < b; c++) {
    err << emit_char(*c);
  }
  cursor = b;
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_span(const char* a, const char* b) {
  Err err;
  for (auto c = a; c < b; c++) {
    err << skip_char(*c);
  }
  cursor = b;
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

CHECK_RETURN Err Cursor::emit_line(const char* fmt, ...) {
  Err err = start_line();
  va_list args;
  va_start(args, fmt);
  err << emit_vprint(fmt, args);
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


//------------------------------------------------------------------------------

Err Cursor::emit_everything() {
  return Err();
}

//------------------------------------------------------------------------------
