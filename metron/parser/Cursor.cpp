#include "Cursor.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

Cursor::Cursor(CSourceRepo* repo, CSourceFile* source, std::string* str_out) {
  this->repo = repo;
  this->source_file = source;
  this->str_out = str_out;

  block_prefix.push("begin");
  block_suffix.push("end");
  override_size.push(0);
  indent.push("");
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit(CNode* n) {
  Err err = check_at(n);
  err << n->emit(*this);
  return err << check_done(n);
}

CHECK_RETURN Err Cursor::skip_over(CNode* n) {
  Err err = check_at(n);
  err << skip_span(n->text_begin(), n->text_end());
  return err << check_done(n);
}

CHECK_RETURN Err Cursor::comment_out(CNode* n) {
  Err err = check_at(n);

  err << emit_print("/*");
  err << emit_span(n->text_begin(), n->text_end());
  err << emit_print("*/");

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_default(CNode* n) {
  Err err = check_at(n);

  if (n->child_head) {
    err << emit_span(n->text_begin(), n->child_head->text_begin());
    err << emit_children(n);
    err << emit_span(n->child_tail->text_end(), n->text_end());
  }
  else {
    err << emit_span(n->text_begin(), n->text_end());
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_children(CNode* n) {
  Err err;
  for (auto c = n->child_head; c; c = c->node_next) {
    err << emit(c);
    if (c->node_next) err << emit_gap(c, c->node_next);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_rest(CNode* n) {
  return emit_span(text_cursor, n->text_end());
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_gap_after(CNode* n) {
  Err err;
  if (n->node_next) err << emit_gap(n, n->node_next);
  return err;
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

CHECK_RETURN Err Cursor::check_at(CNode* n) {
  Err err;

  if (text_cursor != n->text_begin()) {
    char buf1[10];
    char buf2[10];
    memcpy(buf1, n->text_begin(), 9);
    buf1[9] = 0;
    memcpy(buf2, text_cursor, 9);
    buf2[9] = 0;

    err << ERR("check_at - bad cursor\nwant @%10s@\ngot  @%10s@\n", buf1, buf2);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::check_done(CNode* n) {
  Err err;

  auto text_end = n->text_end();

  if (text_cursor < text_end) {
    err << ERR("Cursor was left inside the current node\n");
  }

  if (text_cursor > text_end) {
    //n.dump_tree();
    err << ERR("Cursor was left past the end of the current node\n");
  }

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

CHECK_RETURN Err Cursor::emit_to(const char* b) {
  Err err;
  for (auto c = text_cursor; c < b; c++) {
    err << emit_char(*c);
  }
  text_cursor = b;
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_span(const char* a, const char* b) {
  Err err;
  for (auto c = a; c < b; c++) {
    err << emit_char(*c);
  }
  text_cursor = b;
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::skip_span(const char* a, const char* b) {
  Err err;
  for (auto c = a; c < b; c++) {
    err << skip_char(*c);
  }
  text_cursor = b;
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

//----------------------------------------

CHECK_RETURN Err Cursor::emit_string(const std::string_view& s) {
  Err err;
  for (auto c : s) {
    err << emit_char(c, 0x80FF80);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_replacement(CNode* n, const std::string& s) {
  Err err = check_at(n);
  for (auto c : s) {
    err << emit_char(c, 0x80FFFF);
  }
  text_cursor = n->text_end();
  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_everything() {
  Err err;

  text_cursor = source_file->source_code.data();
  err << source_file->context.root_node->emit(*this);

  return err;
}

//------------------------------------------------------------------------------
// Emit all whitespace and comments between node A and D. Error if we see
// anything that isn't comment or whitespace.

CHECK_RETURN Err Cursor::emit_gap(CNode* a, CNode* b) {
  Err err;
  if (text_cursor != a->text_end()) {
    err << ERR("skip_gap() - Did not start on a.end()");
    return err;
  }

  if (a->node_next != b) return ERR("a->node_next != b");
  if (b->node_prev != a) return ERR("b->node_prev != a");

  err << emit_span(a->text_end(), b->text_begin());

  return err;

  /*
  Err err;
  if (cursor != a.end()) {
    err << ERR("emit_gap() - Did not start on a.end()");
    return err;
  }

  auto source = a.source->source;
  auto pair_lhs = a.node;
  auto pair_rhs = ts_node_next_sibling(a.node);

  while(1) {
    err << emit_span(
      &source[ts_node_end_byte(pair_lhs)],
      &source[ts_node_start_byte(pair_rhs)]
    );

    pair_lhs = pair_rhs;
    pair_rhs = ts_node_next_sibling(pair_rhs);

    if (ts_node_eq(pair_lhs, d.node)) {
      break;
    }
    else if (ts_node_symbol(pair_lhs) != sym_comment) {
      err << ERR("Non-comment found in gap between nodes");
      return err;
    }
    else {
      err << emit_span(
        &source[ts_node_start_byte(pair_lhs)],
        &source[ts_node_end_byte(pair_lhs)]
      );
    }
  }

  if (cursor != d.start()) {
    err << ERR("emit_gap() - Did not end on d.end()");
    return err;
  }
  return err;
  */
}

//----------------------------------------
// Skip all whitespace and comments between node A and B. Error if we see
// anything that isn't comment or whitespace.

CHECK_RETURN Err Cursor::skip_gap(CNode* a, CNode* b) {
  Err err;
  if (text_cursor != a->text_end()) {
    err << ERR("skip_gap() - Did not start on a.end()");
    return err;
  }

  if (a->node_next != b) return ERR("a->node_next != b");
  if (b->node_prev != a) return ERR("b->node_prev != a");

  text_cursor = b->text_begin();

  return err;

  /*
  auto source = a.source->source;
  auto pair_lhs = a.node;
  auto pair_rhs = ts_node_next_sibling(a.node);

  while(1) {
    err << skip_span(
      &source[ts_node_end_byte(pair_lhs)],
      &source[ts_node_start_byte(pair_rhs)]
    );

    pair_lhs = pair_rhs;
    pair_rhs = ts_node_next_sibling(pair_rhs);

    if (ts_node_eq(pair_lhs, d.node)) {
      break;
    }
    else if (ts_node_symbol(pair_lhs) != sym_comment) {
      err << ERR("Non-comment found in gap between nodes");
      return err;
    }
    else {
      err << skip_span(
        &source[ts_node_start_byte(pair_lhs)],
        &source[ts_node_end_byte(pair_lhs)]
      );
    }
  }

  if (cursor != d.start()) {
    err << ERR("skip_gap() - Did not end on d.end()");
    return err;
  }

  return err;
  */
}

//------------------------------------------------------------------------------
