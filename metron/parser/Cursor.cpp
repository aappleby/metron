#include "Cursor.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

Cursor::Cursor(CSourceRepo* repo, CSourceFile* source, std::string* str_out) {
  this->repo = repo;
  this->source_file = source;
  this->str_out = str_out;

  this->tok_begin  = source->context.tokens.data();
  this->tok_end    = this->tok_begin + source->context.tokens.size();

  // Skip LEX_BOF
  this->tok_cursor = this->tok_begin + 1;

  id_map.push(string_to_string());

  block_prefix.push("begin");
  block_suffix.push("end");
  override_size.push(0);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit(CNode* n) {
  if (n == nullptr) return Err();
  Err err = check_at(n);
  err << n->emit(*this);
  return err << check_done(n);
}

CHECK_RETURN Err Cursor::skip_over(CNode* n) {
    if (n == nullptr) return Err();
  Err err = check_at(n);
  err << skip_span(n->tok_begin(), n->tok_end());
  return err << check_done(n);
}

CHECK_RETURN Err Cursor::comment_out(CNode* n) {
  if (n == nullptr) return Err();
  Err err = check_at(n);
  err << emit_print("/*");
  err << emit_span(n->tok_begin(), n->tok_end());
  err << emit_print("*/");
  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_default(CNode* n) {
  if (n == nullptr) return Err();
  Err err = check_at(n);

  if (n->child_head) {
    err << emit_span(n->tok_begin(), n->child_head->tok_begin());
    err << emit_children(n);
    err << emit_span(n->child_tail->tok_end(), n->tok_end());
  }
  else {
    err << emit_span(n->tok_begin(), n->tok_end());
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
  return emit_span(tok_cursor, n->tok_end());
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_gap_after(CNode* n) {
  if (n == nullptr) return Err();
  Err err;
  if (n->node_next) err << emit_gap(n, n->node_next);
  return err;
}

CHECK_RETURN Err Cursor::skip_gap_after(CNode* n) {
  auto begin = n->text_end();
  auto end   = n->node_next->text_begin();

  Err err;
  for (auto c = begin; c < end; c++) {
    err << skip_char(*c);
  }
  tok_cursor = n->node_next->tok_begin();
  line_elided = true;
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
  return err;
}

CHECK_RETURN Err Cursor::emit_span(const CToken* a, const CToken* b) {
  Err err;
  for (auto c = a; c < b; c++) {
    err << emit_token(c);
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

CHECK_RETURN Err Cursor::emit_replacement(CNode* n, const std::string& s) {
  Err err = check_at(n);
  for (auto c : s) {
    err << emit_char(c, 0x80FFFF);
  }
  tok_cursor = n->tok_end();
  return err << check_done(n);
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_replacement(CNode* n, const char* fmt, ...) {
  Err err = check_at(n);
  va_list args;
  va_start(args, fmt);
  err << emit_vprint(fmt, args);
  tok_cursor = n->tok_end();
  return err << check_done(n);
}

//----------------------------------------

CHECK_RETURN Err Cursor::emit_splice(CNode* n) {
  Err err;
  push_cursor(n->tok_begin());
  err << emit(n);
  pop_cursor();
  return err;
}

CHECK_RETURN Err Cursor::emit_raw(CNode* n) {
  Err err;
  err << emit_span(n->tok_begin(), n->tok_end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_everything() {
  return source_file->context.root_node->emit(*this);
}

//------------------------------------------------------------------------------
// Emit all whitespace and comments between node A and B.

CHECK_RETURN Err Cursor::emit_gap(CNode* ta, CNode* tb) {
  Err err;
  if (tok_cursor != ta->tok_end()) {
    return ERR("skip_gap() - Did not start on a.end()");
  }

  if (ta->node_next != tb) return ERR("a->node_next != b");
  if (tb->node_prev != ta) return ERR("b->node_prev != a");

  for (auto c = ta->text_end(); c < tb->text_begin(); c++) {
    err << emit_char(*c);
  }
  tok_cursor = tb->tok_begin();
  return err;
}

//----------------------------------------
// Skip all whitespace and comments between node A and B.

CHECK_RETURN Err Cursor::skip_gap(CNode* a, CNode* b) {
  Err err;
  if (tok_cursor != a->tok_end()) {
    err << ERR("skip_gap() - Did not start on a.end()");
    return err;
  }

  if (a->node_next != b) return ERR("a->node_next != b");
  if (b->node_prev != a) return ERR("b->node_prev != a");

  tok_cursor = b->tok_begin();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Cursor::emit_trailing_whitespace() {
  Err err;
  while(tok_cursor < tok_end) {
    err << emit_token(tok_cursor);
    tok_cursor++;
  }
  return err;
}

//------------------------------------------------------------------------------

void Cursor::push_cursor(const CToken* new_cursor) {
  cursor_stack.push(tok_cursor);
  tok_cursor = new_cursor;
}

void Cursor::pop_cursor() {
  tok_cursor = cursor_stack.top();
  cursor_stack.pop();
}

//------------------------------------------------------------------------------
