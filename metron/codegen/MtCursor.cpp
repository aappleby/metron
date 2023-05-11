#include "metron/codegen/MtCursor.h"

#include "metron/app/MtModLibrary.h"
#include "metron/app/MtSourceFile.h"
#include "metron/nodes/MtField.h"
#include "metron/nodes/MtFuncParam.h"
#include "metron/nodes/MtMethod.h"
#include "metron/nodes/MtModParam.h"
#include "metron/nodes/MtModule.h"
#include "metron/nodes/MtNode.h"

#include "metrolib/core/Log.h"

#include <string.h>

emit_map emit_sym_map = {
  { alias_sym_field_identifier,         &MtCursor::emit_text },
  { alias_sym_namespace_identifier,     &MtCursor::emit_text },
  { alias_sym_type_identifier,          &MtCursor::emit_sym_type_identifier },
  { sym_access_specifier,               &MtCursor::comment_out },
  { sym_argument_list,                  &MtCursor::emit_children },
  { sym_array_declarator,               &MtCursor::emit_children },
  { sym_assignment_expression,          &MtCursor::emit_sym_assignment_expression },
  { sym_binary_expression,              &MtCursor::emit_children },
  { sym_break_statement,                &MtCursor::skip_over }, // Verilog doesn't use "break"
  { sym_call_expression,                &MtCursor::emit_sym_call_expression },
  { sym_case_statement,                 &MtCursor::emit_sym_case_statement },
  { sym_char_literal,                   &MtCursor::emit_text },
  { sym_class_specifier,                &MtCursor::emit_sym_class_specifier },
  { sym_comment,                        &MtCursor::emit_sym_comment },
  { sym_compound_statement,             &MtCursor::emit_sym_compound_statement },
  { sym_condition_clause,               &MtCursor::emit_children },
  { sym_conditional_expression,         &MtCursor::emit_children},
  { sym_declaration,                    &MtCursor::emit_sym_declaration },
  { sym_declaration_list,               &MtCursor::emit_sym_declaration_list },
  { sym_enum_specifier,                 &MtCursor::emit_sym_enum_specifier },
  { sym_enumerator_list,                &MtCursor::emit_children },
  { sym_enumerator,                     &MtCursor::emit_children },
  { sym_expression_statement,           &MtCursor::emit_sym_expression_statement },
  { sym_field_declaration,              &MtCursor::emit_sym_field_declaration },
  { sym_field_declaration_list,         &MtCursor::emit_sym_field_declaration_list },
  { sym_field_expression,               &MtCursor::emit_sym_field_expression },
  { sym_for_statement,                  &MtCursor::emit_sym_for_statement },
  { sym_function_declarator,            &MtCursor::emit_children },
  { sym_function_definition,            &MtCursor::emit_sym_function_definition },
  { sym_identifier,                     &MtCursor::emit_sym_identifier },
  { sym_if_statement,                   &MtCursor::emit_sym_if_statement },
  { sym_init_declarator,                &MtCursor::emit_children },
  { sym_initializer_list,               &MtCursor::emit_sym_initializer_list },
  { sym_namespace_definition,           &MtCursor::emit_sym_namespace_definition },
  { sym_nullptr,                        &MtCursor::emit_sym_nullptr },
  { sym_number_literal,                 &MtCursor::emit_sym_number_literal },
  { sym_parameter_declaration,          &MtCursor::emit_children },
  { sym_parameter_list,                 &MtCursor::emit_children },
  { sym_parenthesized_expression,       &MtCursor::emit_children },
  { sym_pointer_declarator,             &MtCursor::emit_sym_pointer_declarator },

  { sym_preproc_arg,                    &MtCursor::emit_sym_preproc_arg },
  { sym_preproc_call,                   &MtCursor::skip_over },
  { sym_preproc_def,                    &MtCursor::emit_sym_preproc_def },
  { sym_preproc_else,                   &MtCursor::skip_over },
  { sym_preproc_if,                     &MtCursor::skip_over },
  { sym_preproc_ifdef,                  &MtCursor::emit_sym_preproc_ifdef },
  { sym_preproc_include,                &MtCursor::emit_sym_preproc_include },

  { sym_primitive_type,                 &MtCursor::emit_text },
  { sym_qualified_identifier,           &MtCursor::emit_sym_qualified_identifier },
  { sym_return_statement,               &MtCursor::emit_sym_return_statement },
  { sym_sized_type_specifier,           &MtCursor::emit_sym_sized_type_specifier },
  { sym_storage_class_specifier,        &MtCursor::comment_out },
  { sym_string_literal,                 &MtCursor::emit_text },
  { sym_struct_specifier,               &MtCursor::emit_sym_struct_specifier },
  { sym_subscript_expression,           &MtCursor::emit_children },
  { sym_switch_statement,               &MtCursor::emit_sym_switch_statement },
  { sym_template_argument_list,         &MtCursor::emit_sym_template_argument_list },
  { sym_template_declaration,           &MtCursor::emit_sym_template_declaration },
  { sym_template_type,                  &MtCursor::emit_sym_template_type },
  { sym_translation_unit,               &MtCursor::emit_sym_translation_unit },
  { sym_type_definition,                &MtCursor::emit_children },
  { sym_type_descriptor,                &MtCursor::emit_children },
  { sym_type_qualifier,                 &MtCursor::comment_out },
  { sym_unary_expression,               &MtCursor::emit_children },
  { sym_update_expression,              &MtCursor::emit_sym_update_expression },
  { sym_using_declaration,              &MtCursor::emit_sym_using_declaration },
};

//------------------------------------------------------------------------------

MtCursor::MtCursor(MtModLibrary* lib, MtSourceFile* source, MtModule* mod,
                   std::string* out)
    : str_out(out) {
  this->lib = lib;
  current_source.push(source);
  current_mod.push(mod);
  cursor = current_source.top()->source;

  block_prefix.push("begin");
  block_suffix.push("end");
  override_size.push(0);
  indent.push("");

  // FIXME preproc_vars should be a set
  preproc_vars.push(string_to_node());
  preproc_vars.top()["IV_TEST"] = MnNode();

  id_map.push(string_to_string());

  token_map.push({
    {"#include", "`include"},
    {"#ifdef",   "`ifdef"},
    {"#ifndef",  "`ifndef"},
    {"#else",    "`else"},
    {"#elif",    "`elif"},
    {"#endif",   "`endif"},
    {"#define",  "`define"},
    {"#undef",   "`undef"},
  });

  elide_type.push(true);
  elide_value.push(false);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::check_at(const MnNode& n) {
  Err err;
  if (cursor != n.start()){
    char buf1[10];
    char buf2[10];
    memcpy(buf1, n.start(), 9);
    buf1[9] = 0;
    memcpy(buf2, cursor, 9);
    buf2[9] = 0;

    err << ERR("check_at - bad cursor\nwant @%10s@\ngot  @%10s@\n", buf1, buf2);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::check_at(TSSymbol sym, const MnNode& n) {
  Err err;
  if (sym != n.sym) {
    err << ERR("check_at - bad sym, want %d, got %s\n", sym, n.ts_node_type());
  }
  if (cursor != n.start()){
    char buf1[10];
    char buf2[10];
    memcpy(buf1, n.start(), 9);
    buf1[9] = 0;
    memcpy(buf2, cursor, 9);
    buf2[9] = 0;

    err << ERR("check_at - bad cursor\nwant @%10s@\ngot  @%10s@\n", buf1, buf2);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::check_done(MnNode n) {
  Err err;

  if (cursor < n.end()) {
    err << ERR("Cursor was left inside the current node\n");
  }

  if (cursor > n.end()) {
    n.dump_tree();
    err << ERR("Cursor was left past the end of the current node\n");
  }

  return err;
}

//------------------------------------------------------------------------------

void MtCursor::push_indent(MnNode body) {
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

void MtCursor::pop_indent(MnNode class_body) { indent.pop(); }

//------------------------------------------------------------------------------
// Generic emit() methods

CHECK_RETURN Err MtCursor::emit_backspace() {
  Err err;
  if (echo) {
    LOG_C(0x8080FF, "^H");
  }
  str_out->pop_back();
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_indent() {
  Err err;
  for (auto c : indent.top()) {
    err << emit_char(c);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::start_line() {
  Err err;
  if (line_dirty) {
    err << emit_char('\n');
    err << emit_indent();
  }
  return err;
}

//----------------------------------------
// Emit all whitespace and comments between node A and D. Error if we see
// anything that isn't comment or whitespace.

CHECK_RETURN Err MtCursor::emit_gap(MnNode a, MnNode d) {
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
}

//----------------------------------------
// Skip all whitespace and comments between node A and B. Error if we see
// anything that isn't comment or whitespace.

CHECK_RETURN Err MtCursor::skip_gap(MnNode a, MnNode d) {
  Err err;
  if (cursor != a.end()) {
    err << ERR("skip_gap() - Did not start on a.end()");
    return err;
  }

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
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_char(char c, uint32_t color) {
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

CHECK_RETURN Err MtCursor::emit_ws() {
  Err err;
  while (cursor < current_source.top()->source_end && isspace(*cursor)) {
    err << emit_char(*cursor++);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_ws_to(const MnNode& n) {
  Err err;
  while (cursor < current_source.top()->source_end && isspace(*cursor) &&
         cursor < n.start()) {
    err << emit_char(*cursor++);
  }

  if (cursor != n.start()) {
    err << ERR("emit_ws_to - did not hit node %s\n", n.text().c_str());
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_ws_to(TSSymbol sym, const MnNode& n) {
  Err err = emit_ws();
  if (n.sym != sym) {
    err << ERR("emit_ws_to - bad sym, %d != node %s\n", sym, n.ts_node_type());
  }

  if (cursor != n.start()) {
    err << ERR("emit_ws_to - did not hit node %s\n", n.text().c_str());
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::skip_over(MnNode n) {
  Err err = check_at(n);
  return skip_span(n.start(), n.end());;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::skip_ws() {
  Err err;

  while (*cursor && isspace(*cursor) && (*cursor != '\n')) {
    if (echo) {
      LOG_C(0x8080FF, "_");
    }
    cursor++;
  }

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::skip_ws_inside(const MnNode& n) {
  Err err;

  while (*cursor && isspace(*cursor) && (*cursor != '\n') && (cursor < n.end())) {
    if (echo) {
      LOG_C(0x8080FF, "_");
    }
    cursor++;
  }

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::prune_trailing_ws() {
  Err err;

  while (isspace(str_out->back())) {
    err << emit_backspace();
  }

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::comment_out(MnNode n) {
  Err err = check_at(n);

  auto start = n.start();
  auto end1 = n.end();
  auto end2 = n.end();

  // Don't include whitespace in the commented-out bit
  while(isspace(end1[-1]) && end1 > start) {
    end1--;
  }

  err << emit_print("/*");
  err << emit_span(start, end1);
  err << emit_print("*/");
  err << emit_span(end1, end2);

  cursor = n.end();

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_span(const char* a, const char* b) {
  Err err;
  for (auto c = a; c < b; c++) {
    err << emit_char(*c);
  }
  cursor = b;
  return err;
}

CHECK_RETURN Err MtCursor::skip_span(const char* a, const char* b) {
  Err err;
  if (echo) {
    for (auto c = a; c < b; c++) {
      LOG_C(0x8080FF, "%c", *c);
    }
  }
  cursor = b;
  line_elided = true;
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_text(MnNode n) {
  Err err = check_at(n);

  err << emit_span(n.start(), n.end());

  return err << check_done(n);
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_vprint(const char* fmt, va_list args) {
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

CHECK_RETURN Err MtCursor::emit_line(const char* fmt, ...) {
  Err err = start_line();
  va_list args;
  va_start(args, fmt);
  err << emit_vprint(fmt, args);
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_print(const char* fmt, ...) {
  Err err;
  va_list args;
  va_start(args, fmt);
  err << emit_vprint(fmt, args);
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_replacement(MnNode n, const char* fmt, ...) {
  Err err = check_at(n);
  va_list args;
  va_start(args, fmt);
  err << emit_vprint(fmt, args);
  cursor = n.end();
  return err << check_done(n);
}

//------------------------------------------------------------------------------
// FIXME we don't seem to be exercising this...

CHECK_RETURN Err MtCursor::emit_sym_initializer_list(MnNode node) {
  exit(1);

  Err err = check_at(sym_initializer_list, node);

  bool has_zero = false;
  bool bad_list = false;
  for (auto child : node) {
    if (!child.is_named()) continue;
    if (child.sym == sym_number_literal) {
      if (child.text() == "0") {
        if (has_zero) bad_list = true;
        has_zero = true;
      }
      else {
        bad_list = true;
      }
    }
    else {
      bad_list = true;
    }
  }

  if (has_zero && !bad_list) {
    err << emit_replacement(node, "'0");
  }
  else {
    err << emit_children(node);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------
// Replace "#include" with "`include" and ".h" with ".sv"

CHECK_RETURN Err MtCursor::emit_sym_preproc_include(MnNode n) {
  Err err = check_at(sym_preproc_include, n);

  for (auto child : n) {
    err << emit_ws_to(child);

    if (child.field == field_path) {
      auto path = child.text();
      if (!path.ends_with(".h\"")) return err << ERR("Include did not end with .h\n");
      path.resize(path.size() - 3);
      path = path + ".sv\"";
      err << emit_replacement(child, path.c_str());
    }
    else {
      err << emit_dispatch(child);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

CHECK_RETURN Err MtCursor::emit_sym_assignment_expression(MnNode node) {
  Err err = check_at(sym_assignment_expression, node);

  MnNode lhs;

  for (auto child : node) {
    err << emit_ws_to(child);
    switch (child.field) {
      case field_left: {
        lhs = child;
        err << emit_dispatch(child);
        break;
      }
      case field_operator: {
        // There may not be a method if we're in an enum initializer list.
        bool left_is_field = current_mod.top()->get_field(lhs.name4()) != nullptr;
        if (current_method.top() && current_method.top()->is_tick_ && left_is_field) {
          err << emit_replacement(child, "<=");
        } else {
          err << emit_replacement(child, "=");
        }
        auto op  = child.text();
        bool is_compound = op != "=";
        if (is_compound) {
          push_cursor(lhs);
          err << emit_print(" ");
          err << emit_dispatch(lhs);
          err << emit_print(" %c", op[0]);
          pop_cursor();
        }
        break;
      }
      default: {
        err << emit_dispatch(child);
        break;
      }
    }
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_static_bit_extract(MnNode call, int bx_width) {
  Err err = check_at(call);

  int arg_count = call.get_field(field_arguments).named_child_count();

  auto arg0 = call.get_field(field_arguments).named_child(0);
  auto arg1 = call.get_field(field_arguments).named_child(1);

  if (arg_count == 1) {
    if (arg0.sym == sym_number_literal) {
      // Explicitly sized literal - 8'd10
      override_size.push(bx_width);
      err << emit_splice(arg0);
      override_size.pop();
    }
    else if (arg0.sym == sym_identifier || arg0.sym == sym_subscript_expression) {
      if (arg0.text() == "DONTCARE") {
        // Size-casting expression
        err << emit_print("%d'bx", bx_width);
      } else {
        // Size-casting expression
        err << emit_print("%d'(", bx_width);
        err << emit_splice(arg0);
        err << emit_print(")");
      }

    } else {
      // Size-casting expression
      err << emit_print("%d'(", bx_width);
      err << emit_splice(arg0);
      err << emit_print(")");
    }
  } else if (arg_count == 2) {
    // Slicing logic array - foo[7:2]

    if (bx_width == 1) {
      // b1(foo, 7) -> foo[7]
      err << emit_splice(arg0);
      err << emit_print("[");
      err << emit_splice(arg1);
      err << emit_print("]");
    }
    else {
      err << emit_splice(arg0);
      err << emit_print("[");

      if (arg1.sym == sym_number_literal) {
        // Static size slice, static offset
        // b6(foo, 2) -> foo[7:2]
        int offset = atoi(arg1.start());
        err << emit_print("%d:%d", bx_width - 1 + offset, offset);
      } else {
        // Static size slice, dynamic offset
        // b6(foo, offset) -> foo[5 + offset:offset]
        err << emit_print("%d + ", bx_width - 1);
        err << emit_splice(arg1);
        err << emit_print(" : ");
        err << emit_splice(arg1);
      }
      err << emit_print("]");

    }
  } else {
    err << ERR("emit_static_bit_extract got > 1 args\n");
  }

  cursor = call.end();
  return err << check_done(call);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_dynamic_bit_extract(MnNode call,
                                                    MnNode bx_node) {
  Err err = check_at(call);

  int arg_count = call.get_field(field_arguments).named_child_count();

  auto arg0 = call.get_field(field_arguments).named_child(0);
  auto arg1 = call.get_field(field_arguments).named_child(1);

  auto arg0_text = arg0.text();

  auto arg0_field = current_mod.top()->get_field(arg0_text);
  if (arg0_field) {
    if (arg0_field->is_input()) {
      printf("INPUT FIELD %s!\n", arg0_field->cname());
    }
  }

  if (arg_count == 1) {
    // Non-literal size-casting expression - bits'(expression)
    if (arg0.text() == "DONTCARE") {
      err << emit_splice(bx_node);
      err << emit_print("'('x)");
    } else {
      err << emit_print("(");
      err << emit_splice(bx_node);
      err << emit_print(")'(");
      err << emit_splice(arg0);
      err << emit_print(")");
    }

  } else if (arg_count == 2) {
    // Non-literal slice expression - expression[bits+offset-1:offset];

    if (arg1.sym != sym_number_literal) {
      err << ERR("emit_dynamic_bit_extract saw a non-literal?\n");
    }
    int offset = atoi(arg1.start());

    err << emit_splice(arg0);
    err << emit_print("[%s+%d:%d]", bx_node.text().c_str(), offset - 1, offset);
  } else {
    err << ERR("emit_dynamic_bit_extract saw too many args?\n");
  }

  cursor = call.end();
  return err << check_done(call);
}

//------------------------------------------------------------------------------
// Call expressions turn into either Verilog calls, or are replaced with binding
// statements.

CHECK_RETURN bool MtCursor::can_omit_call(MnNode n) {
  assert(n.sym == sym_call_expression);
  MnNode func = n.get_field(field_function);
  MnNode args = n.get_field(field_arguments);

  if (func.sym == sym_field_expression) {
    // Calls into submodules always turn into bindings.
    return true;
  }
  else {
    // Calls into methods in the same module turn into bindings if needed.
    auto dst_method = current_mod.top()->get_method(func.name4());
    if (dst_method && dst_method->needs_binding) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
// Replace function names with macro names where needed, comment out explicit
// init/tick/tock calls.

// + call_expression (289) =
// |--# function: identifier (1) = "readmemh"
// |--+ arguments: argument_list (290) =

CHECK_RETURN Err MtCursor::emit_sym_call_expression(MnNode n) {
  Err err = check_at(sym_call_expression, n);

  err << emit_ws_to(n);

  MnNode func = n.get_field(field_function);
  MnNode args = n.get_field(field_arguments);

  std::string func_name = func.name4();

  //----------
  // Handle bN

  if (func_name[0] == 'b' && func_name.size() == 2) {
    auto x = func_name[1];
    if (x >= '0' && x <= '9') {
      return emit_static_bit_extract(n, x - '0');
    }
  }

  if (func_name[0] == 'b' && func_name.size() == 3) {
    auto x = func_name[1];
    auto y = func_name[2];
    if (x >= '0' && x <= '9' && y >= '0' && y <= '9') {
      return emit_static_bit_extract(n, (x - '0') * 10 + (y - '0'));
    }
  }

  //----------

  if (func_name == "bx") {
    // Bit extract.
    auto template_arg = func.get_field(field_arguments).named_child(0);
    err << emit_ws_to(n);
    err << emit_dynamic_bit_extract(n, template_arg);
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Remove "cat" and replace parens with brackets

  if (func_name == "cat") {
    err << skip_over(func);
    for (const auto& arg : (MnNode&)args) {
      err << emit_ws_to(arg);

      if (arg.sym == anon_sym_LPAREN) {
        err << emit_replacement(arg, "{");
      }
      else if(arg.sym == anon_sym_RPAREN) {
        err << emit_replacement(arg, "}");
      }
      else {
        err << emit_dispatch(arg);
      }
    }
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Convert "dup<15>(x)" to "{15 {x}}"

  if (func_name == "dup") {

    // printf("{%d {%s}}", call.func.args[0], call.args[0]); ???

    if (args.named_child_count() != 1) return err << ERR("dup() had too many children\n");

    err << skip_over(func);

    auto template_arg = func.get_field(field_arguments).named_child(0);
    int dup_count = atoi(template_arg.start());
    auto func_arg = args.named_child(0);
    err << emit_print("{%d {", dup_count);
    err << emit_splice(func_arg);
    err << emit_print("}}");
    cursor = n.end();
    return err << check_done(n);
  }

  //----------

  if (func_name == "sra") {
    auto lhs = args.named_child(0);
    auto rhs = args.named_child(1);

    err << emit_print("($signed(");
    err << emit_splice(lhs);
    err << emit_print(") >>> ");
    err << emit_splice(rhs);
    err << emit_print(")");
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Component method call.

  if (func.sym == sym_field_expression) {

    auto node_component = n.get_field(field_function).get_field(field_argument);

    auto dst_component = current_mod.top()->get_field(node_component.text());
    auto dst_mod = lib->get_module(dst_component->type_name());
    if (!dst_mod) return err << ERR("dst_mod null\n");
    auto node_func = n.get_field(field_function).get_field(field_field);
    auto dst_method = dst_mod->get_method(node_func.text());

    if (!dst_method) return err << ERR("dst_method null\n");

    err << emit_print("%s_%s_ret", node_component.text().c_str(), dst_method->cname());
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Builtins

  string_to_string renames = {
    {"signed",         "$signed"},
    {"unsigned",       "$unsigned"},
    {"clog2",          "$clog2" },
    {"pow2",           "2**" },
    {"readmemh",       "$readmemh" },
    {"value_plusargs", "$value$plusargs" },
    {"write",          "$write" },
    {"sign_extend",    "$signed" },
    {"zero_extend",    "$unsigned" },
  };

  if (auto it = renames.find(func_name); it != renames.end()) {
    err << emit_replacement(func, (*it).second.c_str());
    err << emit_dispatch(args);
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Internal method call.

  auto method = current_mod.top()->get_method(func_name);
  if (method->needs_binding) {
    err << emit_replacement(n, "%s_ret", method->cname());
  }
  else {
    err << emit_children(n);
  }

  cursor = n.end();
  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Emit local variable declarations at the top of the block scope.

CHECK_RETURN Err MtCursor::emit_hoisted_decls(MnNode n) {
  Err err;

  elide_type.push(false);
  elide_value.push(true);

  for (const auto& c : (MnNode&)n) {
    if (c.sym == sym_declaration) {
      if (c.is_const()) {
        // Don't emit decls for localparams
        continue;
      } else {
        err << start_line();
        err << emit_splice(c);
      }
    }

    if (c.sym == sym_for_statement) {
      auto init = c.get_field(field_initializer);
      if (init.sym == sym_declaration) {
        err << start_line();
        err << emit_splice(init);
      }
    }
  }

  elide_type.pop();
  elide_value.pop();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_local_call_arg_binding(MtMethod* method, MnNode param, MnNode val) {
  Err err;

  err << emit_line("%s_%s = ",
    method->cname(),
    param.get_field(field_declarator).text().c_str());
  err << emit_splice(val);
  err << prune_trailing_ws();
  err << emit_print(";");

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_component_call_arg_binding(MnNode inst, MtMethod* method, MnNode param, MnNode val) {
  Err err;

  err << emit_line("%s_%s_%s = ",
    inst.text().c_str(),
    method->name().c_str(),
    param.get_field(field_declarator).text().c_str());
  err << emit_splice(val);
  err << prune_trailing_ws();
  err << emit_print(";");

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_call_arg_bindings(MnNode n) {
  Err err;

  // Emit bindings for child nodes first, but do _not_ recurse into compound
  // blocks.

  for (auto c : n) {
    if (c.sym != sym_compound_statement) {
      err << emit_call_arg_bindings(c);
    }
  }

  // OK, now we can emit bindings for the call we're at.

  bool any_bindings = false;

  if (n.sym == sym_call_expression) {
    auto func_node = n.get_field(field_function);
    auto args_node = n.get_field(field_arguments);

    if (func_node.sym == sym_field_expression) {
      if (args_node.named_child_count() != 0) {
        auto inst_id = func_node.get_field(field_argument);
        auto meth_id = func_node.get_field(field_field);

        auto component = current_mod.top()->get_component(inst_id.text());
        assert(component);

        auto component_method = component->_type_mod->get_method(meth_id.text());
        if (!component_method) return err << ERR("Component method missing\n");

        for (int i = 0; i < component_method->param_nodes.size(); i++) {
          err << emit_component_call_arg_binding(
            inst_id,
            component_method,
            component_method->param_nodes[i],
            args_node.named_child(i)
          );
          any_bindings = true;
        }
      }
    }
    else if (func_node.sym == sym_identifier) {
      auto method = current_mod.top()->get_method(func_node.text().c_str());
      if (method && method->needs_binding) {
        for (int i = 0; i < method->param_nodes.size(); i++) {
          err << emit_local_call_arg_binding(
            method,
            method->param_nodes[i],
            args_node.named_child(i)
          );
          any_bindings = true;
        }
      }
    }
  }

  if (any_bindings) {
    err << start_line();
  }

  return err;
}

//------------------------------------------------------------------------------
// sym_function_definition
//  field_declarator : sym_function_declarator
//  sym_field_initializer_list (optional)
//  field_body : sym_compound_statement

CHECK_RETURN Err MtCursor::emit_func_as_init(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.sym == sym_function_declarator) {
      auto func_params = c.get_field(field_parameters);
      err << emit_param_list_as_modparams(func_params);
      err << emit_replacement(c, "initial");
    }
    else if (c.sym == sym_field_initializer_list) {
      err << skip_over(c);
      err << skip_ws_inside(n);
    }
    else if (c.sym == sym_compound_statement) {
      err << emit_block(c, "begin", "end");
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// + function_definition (209) =
// |--+ type: template_type (348) =
// |--+ declarator: function_declarator (239) =
// |--+ body: compound_statement (248) =

CHECK_RETURN Err MtCursor::emit_func_as_func(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  auto func_type = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);
  auto func_body = n.get_field(field_body);

  err << emit_print("function ");
  err << emit_dispatch(func_type);
  err << emit_gap(func_type, func_decl);
  err << emit_dispatch(func_decl);
  err << emit_print(";");
  err << emit_gap(func_decl, func_body);
  err << emit_block(func_body, "", "endfunction");

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// + function_definition (209) =
// |--# type: primitive_type (80) = "void"
// |--+ declarator: function_declarator (239) =
// |--+ body: compound_statement (248) =

CHECK_RETURN Err MtCursor::emit_func_as_task(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  auto func_type = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);
  auto func_body = n.get_field(field_body);

  err << emit_replacement(func_type, "task automatic");
  err << emit_gap(func_type, func_decl);
  err << emit_dispatch(func_decl);
  err << emit_print(";");
  err << emit_gap(func_decl, func_body);
  err << emit_block(func_body, "", "endtask");

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_func_as_always_comb(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  auto func_type = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);
  auto func_body = n.get_field(field_body);
  auto func_params = func_decl.get_field(field_parameters);

  id_map.push(id_map.top());
  for (auto c : func_params) {
    if (!c.is_named()) continue;
    id_map.top()[c.name4()] = func_decl.name4() + "_" + c.name4();
  }

  err << emit_replacement(func_type, "always_comb begin :");
  err << emit_gap(func_type, func_decl);
  err << emit_replacement(func_decl, func_decl.name4().c_str());
  err << emit_gap(func_decl, func_body);
  err << emit_block(func_body, "", "end");

  id_map.pop();
  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_func_as_always_ff(MnNode n) {
  Err err;

  auto func_type = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);
  auto func_body = n.get_field(field_body);
  auto func_params = func_decl.get_field(field_parameters);

  id_map.push(id_map.top());
  for (auto c : func_params) {
    if (c.sym == sym_parameter_declaration) {
      id_map.top()[c.name4()] = func_decl.name4() + "_" + c.name4();
    }
  }

  err << emit_replacement(func_type, "always_ff @(posedge clock) begin :");
  err << emit_gap(func_type, func_decl);
  err << emit_replacement(func_decl, func_decl.name4().c_str());
  err << emit_gap(func_decl, func_body);
  err << emit_block(func_body, "", "end");

  id_map.pop();
  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Change "init/tick/tock" to "initial begin / always_comb / always_ff", change
// void methods to tasks, and change const methods to funcs.

// func_def = { field_type, field_declarator, field_body }

// Tock func with no return is the problematic one for Yosys.
// We can't emit as task, we're in tock and that would break sensitivity

CHECK_RETURN Err MtCursor::emit_sym_function_definition(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  auto return_type = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);

  current_method.push(current_mod.top()->get_method(n.name4()));
  assert(current_method.top());

  //----------
  // Emit a block declaration for the type of function we're in.

  if (current_method.top()->emit_as_always_comb) {
    err << emit_func_as_always_comb(n);
  }
  else if (current_method.top()->emit_as_always_ff) {
    err << emit_func_as_always_ff(n);
  }
  else if (current_method.top()->emit_as_init) {
    err << emit_func_as_init(n);
  }
  else if (current_method.top()->emit_as_task) {
    err << emit_func_as_task(n);
  }
  else if (current_method.top()->emit_as_func) {
    err << emit_func_as_func(n);
  }

  //----------

  if (current_method.top()->needs_binding) {
    err << emit_func_binding_vars(current_method.top());
  }

  current_method.pop();
  return err << check_done(n);
}

//------------------------------------------------------------------------------
// + field_declaration (259) =
// |--# type: type_identifier (444) = "example_data_memory"
// |--# declarator: field_identifier (440) = "data_memory"
// |--# lit (39) = ";"

CHECK_RETURN Err MtCursor::emit_component(MnNode n) {
  Err err;

  auto node_type = n.get_field(field_type);
  auto node_decl = n.get_field(field_declarator);
  auto node_semi = n.child_by_sym(anon_sym_SEMI);

  auto inst_name = node_decl.text();
  auto component_mod = lib->get_module(n.type5());

  //----------------------------------------
  // Component name

  err << emit_dispatch(node_type);

  //----------------------------------------
  // Component modparams

  bool has_template_params = node_type.sym == sym_template_type && component_mod->mod_param_list;
  bool has_constructor_params = component_mod->constructor && component_mod->constructor->param_nodes.size();

  if (has_template_params || has_constructor_params) {

    err << emit_print(" #(");
    indent.push(indent.top() + "  ");

    // Emit template arguments as module parameters
    if (has_template_params) {
      err << emit_line("// Template Parameters");

      auto template_args = node_type.get_field(field_arguments);

      std::vector<MnNode> params;
      std::vector<MnNode> args;

      for (auto c : component_mod->mod_param_list) {
        if (c.is_named()) params.push_back(c);
      }

      for (auto c : template_args) {
        if (c.is_named()) args.push_back(c);
      }

      for (int param_index = 0; param_index < args.size(); param_index++) {
        auto param = params[param_index];
        auto arg = args[param_index];

        err << emit_line(".%s(", param.name4().c_str());
        err << emit_splice(arg);
        err << emit_print("),");
      }
    }

    // Emit constructor arguments as module parameters
    if (has_constructor_params) {
      err << emit_line("// Constructor Parameters");

      // The parameter names come from the submodule's constructor
      const auto& params = component_mod->constructor->param_nodes;

      // Find the initializer node for the component and extract arguments
      std::vector<MnNode> args;
      if (current_mod.top()->constructor) {
        for(auto initializer : current_mod.top()->constructor->_node.child_by_sym(sym_field_initializer_list)) {
          if (initializer.sym != sym_field_initializer) continue;
          if (initializer.child_by_sym(alias_sym_field_identifier).text() == inst_name) {
            for (auto c : initializer.child_by_sym(sym_argument_list)) {
              if (c.is_named()) args.push_back(c);
            }
            break;
          }
        }
      }

      for (int param_index = 0; param_index < args.size(); param_index++) {
        auto param = params[param_index];
        auto arg = args[param_index];

        err << emit_line(".%s(", param.name4().c_str());
        err << emit_splice(arg);
        err << emit_print("),");
      }
    }
    indent.pop();

    // Remove trailing comma from port list
    if (at_comma) {
      err << emit_backspace();
    }

    err << emit_line(")");
  }

  //----------------------------------------
  // Component name

  err << emit_gap(node_type, node_decl);
  err << emit_dispatch(node_decl);

  //----------------------------------------
  // Port list

  err << emit_print("(");

  indent.push(indent.top() + "  ");

  if (component_mod->needs_tick()) {
    err << emit_line("// Global clock");
    err << emit_line(".clock(clock),");
  }

  if (component_mod->input_signals.size()) {
    err << emit_line("// Input signals");
    for (auto f : component_mod->input_signals) {
      err << emit_line(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
    }
  }

  if (component_mod->output_signals.size()) {
    err << emit_line("// Output signals");
    for (auto f : component_mod->output_signals) {
      err << emit_line(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
    }
  }

  if (component_mod->output_registers.size()) {
    err << emit_line("// Output registers");
    for (auto f : component_mod->output_registers) {
      err << emit_line(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
    }
  }

  for (auto m : component_mod->all_methods) {
    if (m->is_constructor()) continue;
    if (m->is_public() && m->internal_callers.empty()) {

      if (m->param_nodes.size() || m->has_return()) {
        err << emit_line("// %s() ports", m->cname());
      }

      int param_count = m->param_nodes.size();
      for (int i = 0; i < param_count; i++) {
        auto param = m->param_nodes[i];
        auto node_type = param.get_field(field_type);
        auto node_decl = param.get_field(field_declarator);

        err << emit_line(".%s_%s(%s_%s_%s),", m->cname(), node_decl.text().c_str(), inst_name.c_str(), m->cname(), node_decl.text().c_str());
      }

      if (m->has_return()) {
        auto node_type = m->_node.get_field(field_type);
        auto node_decl = m->_node.get_field(field_declarator);
        auto node_name = node_decl.get_field(field_declarator);

        err << emit_line(".%s_ret(%s_%s_ret),", m->cname(), inst_name.c_str(), m->cname());
      }
    }
  }

  // Remove trailing comma from port list
  if (at_comma) {
    err << emit_backspace();
  }

  indent.pop();

  err << emit_line(")");
  err << emit_dispatch(node_semi);

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Emits the fields that come after a submod declaration

// module my_mod(
//   .foo(my_mod_foo)
// );
// logic my_mod_foo; <-- this part

CHECK_RETURN Err MtCursor::emit_submod_binding_fields(MnNode n) {
  Err err;

  if (current_mod.top()->components.empty()) {
    return err;
  }

  auto node_type = n.get_field(field_type);
  auto node_decl = n.get_field(field_declarator);

  auto component_name = node_decl.text();
  auto component_cname = component_name.c_str();

  std::string type_name = node_type.type5();
  auto component_mod = lib->get_module(type_name);

  // Swap template arguments with the values from the template
  // instantiation.
  std::map<std::string, std::string> replacements;

  auto args = node_type.get_field(field_arguments);
  if (args) {
    int arg_count = args.named_child_count();
    for (int i = 0; i < arg_count; i++) {
      auto key = component_mod->all_modparams[i]->name();
      auto val = args.named_child(i).text();
      replacements[key] = val;
    }
  }

  // FIXME clean this mess up

  current_source.push(component_mod->source_file);
  current_mod.push(component_mod);
  id_map.push(replacements);

  for (auto field : component_mod->input_signals) {
    err << start_line();
    err << emit_splice(field->get_type_node());
    err << emit_print(" %s_", component_cname);
    err << emit_splice(field->get_decl_node());
    err << emit_print(";");
  }

  for (auto param : component_mod->input_method_params) {
    err << start_line();
    err << emit_splice(param->get_type_node());
    err << emit_print(" %s_%s_", component_cname, param->func_name.c_str());
    err << emit_splice(param->get_decl_node());
    err << emit_print(";");
  }

  for (auto field : component_mod->output_signals) {
    err << start_line();
    err << emit_splice(field->get_type_node());
    err << emit_print(" %s_", component_cname);
    err << emit_splice(field->get_decl_node());
    err << emit_print(";");
  }

  for (auto field : component_mod->output_registers) {
    err << start_line();
    err << emit_splice(field->get_type_node());
    err << emit_print(" %s_", component_cname);
    err << emit_splice(field->get_decl_node());
    err << emit_print(";");
  }

  for (auto method : component_mod->output_method_returns) {
    err << start_line();
    err << emit_splice(method->_node.get_field(field_type));
    err << emit_print(" %s_", component_cname);
    err << emit_splice(method->_node.get_field(field_declarator).get_field(field_declarator));
    err << emit_print("_ret;");
  }

  current_source.pop();
  current_mod.pop();
  id_map.pop();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_enum_specifier(MnNode n) {
  Err err = check_at(sym_enum_specifier, n);

  auto node_enum  = n.child_by_sym(anon_sym_enum);
  auto node_class = n.child_by_sym(anon_sym_class);
  auto node_name  = n.get_field(field_name);
  auto node_colon = n.child_by_sym(anon_sym_COLON);
  auto node_base  = n.get_field(field_base);
  auto node_body  = n.get_field(field_body);

  // Extract enum bit width, if present.
  override_size.push(32);
  if (node_base) {
    auto node_scope = node_base.get_field(field_scope);
    auto node_type_args = node_scope.get_field(field_arguments);
    for (auto c : node_type_args) {
      if (c.sym == sym_number_literal) {
        override_size.top() = atoi(c.start());
        break;
      }
    }
  }

  if (node_base) {
    // + type: enum_specifier (253) =
    // |--# lit (81) = "enum"
    // |--# lit (82) = "class"
    // |--# name: type_identifier (444) = "typed_enum"
    // |--# lit (85) = ":"
    // |--# base: type_identifier (444) = "int"
    // |--+ body: enumerator_list (254) =
    err << emit_print("typedef ");
    err << emit_dispatch(node_enum);
    err << emit_gap(node_enum, node_class);
    err << skip_over(node_class);
    err << skip_gap(node_class, node_name);
    err << skip_over(node_name);
    err << skip_gap(node_name, node_colon);
    err << skip_over(node_colon);
    err << skip_gap(node_colon, node_base);

    auto node_scope = node_base.get_field(field_scope);

    if (node_scope) {
      err << skip_over(node_base);
      err << emit_splice(node_base.get_field(field_scope));
    }
    else {
      err << emit_dispatch(node_base);
    }

    err << emit_gap(node_base, node_body);
    err << emit_dispatch(node_body);
    err << emit_print(" ");
    err << emit_splice(node_name);
  }
  else if (node_class) {
    // + type: enum_specifier (253) =
    // |--# lit (81) = "enum"
    // |--# lit (82) = "class"
    // |--# name: type_identifier (444) = "enum_class1"
    // |--+ body: enumerator_list (254) =
    err << emit_print("typedef ");
    err << emit_dispatch(node_enum);
    err << emit_gap(node_enum, node_class);
    err << skip_over(node_class);
    err << skip_gap(node_class, node_name);
    err << skip_over(node_name);
    err << skip_gap(node_name, node_body);
    err << emit_dispatch(node_body);
    err << emit_print(" ");
    err << emit_splice(node_name);
  }
  else if (node_name) {
    // + enum_specifier (253) =
    // |--# lit (81) = "enum"
    // |--# name: type_identifier (444) = "top_level_enum"
    // |--+ body: enumerator_list (254) =
    err << emit_print("typedef ");
    err << emit_dispatch(node_enum);
    err << emit_gap(node_enum, node_name);
    err << skip_over(node_name);
    err << skip_gap(node_name, node_body);
    err << emit_dispatch(node_body);
    err << emit_print(" ");
    err << emit_splice(node_name);
  }
  else {
    // + type: enum_specifier (253) =
    // |--# lit (81) = "enum"
    // |--+ body: enumerator_list (254) =
    err << emit_dispatch(node_enum);
    err << emit_gap(node_enum, node_body);
    err << emit_dispatch(node_body);
  }

  override_size.pop();
  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Pointer decls are used to pass strings as params, but we pull the '*' off.

CHECK_RETURN Err MtCursor::emit_sym_pointer_declarator(MnNode n) {
  Err err = check_at(sym_pointer_declarator, n);

  auto node_star = n.child(0);
  auto node_decl = n.get_field(field_declarator);

  err << skip_over(node_star);
  err << skip_gap(node_star, node_decl);
  err << emit_dispatch(node_decl);

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// + optional_parameter_declaration (321) =
// |--# type: primitive_type (80) = "int"
// |--# declarator: identifier (1) = "cycles_per_bit"
// |--# lit (63) = "="
// |--# default_value: number_literal (126) = "4"

// + optional_parameter_declaration (321) =
// |--+ type_qualifier (250) =
// |--# type: primitive_type (80) = "char"
// |--+ declarator: pointer_declarator (235) =
// |--# lit (63) = "="
// |--# default_value: nullptr (184) = "nullptr"

CHECK_RETURN Err MtCursor::emit_optional_param_as_modparam(MnNode n) {
  Err err = check_at(sym_optional_parameter_declaration, n);

  auto node_qual = n.child_by_sym(sym_type_qualifier);
  auto node_type = n.get_field(field_type);
  auto node_decl = n.get_field(field_declarator);
  auto node_eq   = n.child_by_sym(anon_sym_EQ);
  auto node_val  = n.get_field(field_default_value);

  err << emit_line("parameter ");
  if (node_qual) {
    err << emit_dispatch(node_qual);
    err << emit_gap(node_qual, node_type);
  }
  err << skip_over(node_type);
  err << skip_gap(node_type, node_decl);
  err << emit_dispatch(node_decl);
  err << emit_gap(node_decl, node_eq);
  err << emit_dispatch(node_eq);
  err << emit_gap(node_eq, node_val);
  err << emit_dispatch(node_val);
  err << emit_print(";");

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Emit field declarations. For components, also emit glue declarations and
// append the glue parameter list to the field.

// + field_declaration (259) =
// |--# type: type_identifier (444) = "example_data_memory"
// |--# declarator: field_identifier (440) = "data_memory"
// |--# lit (39) = ";"

CHECK_RETURN Err MtCursor::emit_sym_field_declaration(MnNode n) {
  Err err = check_at(n);
  assert(n.sym == sym_field_declaration);

  // Struct outside of class
  if (current_mod.top() == nullptr) {
    // sym_field_declaration
    //   field_type : sym_template_type
    //   field_declarator : sym_field_identifier
    //   lit ;

    return err << emit_children(n);
  }

  // Const local variable
  if (n.is_const()) {
    err << emit_ws_to(n);
    err << emit_print("localparam ");
    err << emit_children(n);
    return err << check_done(n);
  }

  // Enum
  if (n.get_field(field_type).sym == sym_enum_specifier) {
    return emit_children(n);
  }

  //----------
  // Actual fields

  auto field = current_mod.top()->get_field(n.name4());
  assert(field);

  if (field->is_component()) {
    // Component
    err << emit_component(n);
    err << emit_submod_binding_fields(n);
  }
  else if (field->is_port()) {
    // Ports don't go in the class body.
    err << skip_over(n);
  }
  else if (field->is_dead()) {
    err << comment_out(n);
  }
  else {
    err << emit_children(n);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// + struct_specifier (255) =
// |--# lit (83) = "struct"
// |--# name: type_identifier (444) = "MyStruct1"
// |--+ body: field_declaration_list (257) =

CHECK_RETURN Err MtCursor::emit_sym_struct_specifier(MnNode n) {
  Err err = check_at(sym_struct_specifier, n);

  auto node_struct = n.child_by_sym(anon_sym_struct);
  auto node_name   = n.get_field(field_name);
  auto node_body   = n.get_field(field_body);

  err << emit_replacement(node_struct, "typedef struct packed");
  err << emit_gap(node_struct, node_name);
  err << skip_over(node_name);
  err << skip_gap(node_name, node_body);
  err << emit_block(node_body, "{", "}");
  err << emit_print(" ");
  err << emit_splice(node_name);
  err << emit_print(";");

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_param_port(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  err << emit_line("input ");
  err << emit_splice(node_type);
  err << emit_print(" %s_", m->cname());
  err << emit_splice(node_name);
  err << emit_print(",");

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_param_binding(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  err << start_line();
  err << emit_splice(node_type);
  err << emit_print(" %s_", m->cname());
  err << emit_splice(node_name);
  err << emit_print(";");

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_return_port(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  err << emit_line("output ");
  err << emit_splice(node_type);
  err << emit_print(" %s_ret,", m->cname());

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_return_binding(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  err << start_line();
  err << emit_splice(node_type);
  err << emit_print(" %s_ret;", m->cname());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_method_ports(MtMethod* m) {
  Err err;

  if (m->param_nodes.size() || m->has_return()) {
    err << emit_line("// %s() ports", m->cname());
  }

  for (auto& param : m->param_nodes) {
    auto node_type = param.get_field(field_type);
    auto node_decl = param.get_field(field_declarator);
    err << emit_param_port(m, node_type, node_decl);
  }

  if (m->has_return()) {
    auto node_type = m->_node.get_field(field_type);
    auto node_decl = m->_node.get_field(field_declarator);
    auto node_name = node_decl.get_field(field_declarator);
    err << emit_return_port(m, node_type, node_name);
  }

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_func_binding_vars(MtMethod* m) {
  Err err;

  for (auto& param : m->param_nodes) {
    auto node_type = param.get_field(field_type);
    auto node_decl = param.get_field(field_declarator);
    err << emit_param_binding(m, node_type, node_decl);
  }

  if (m->has_return()) {
    auto node_type = m->_node.get_field(field_type);
    auto node_decl = m->_node.get_field(field_declarator);
    auto node_name = node_decl.get_field(field_declarator);
    err << emit_return_binding(m, node_type, node_name);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_field_port(MtField* f) {
  Err err;

  if (!f->is_public()) return ERR("Can't emit port for private field");
  if (f->is_component()) return ERR("Can't emit port for component field");

  if (f->is_input()) {
    err << emit_line("input ");
  } else {
    err << emit_line("output ");
  }

  err << emit_splice(f->get_type_node());
  err << emit_print(" ");
  err << emit_splice(f->get_decl_node());
  err << emit_print(",");

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_param_list_as_modparams(MnNode param_list) {
  Err err;
  push_cursor(param_list);

  for (auto c : param_list) {
    err << emit_ws_to(c);

    switch (c.sym) {
      case anon_sym_LPAREN:
      case anon_sym_RPAREN:
      case anon_sym_COMMA:
        err << skip_over(c);
        break;

      case sym_optional_parameter_declaration:
        err << emit_optional_param_as_modparam(c);
        break;

      case sym_parameter_declaration:
        err << ERR("Parameter '%s' must have a default value\n", c.text().c_str());
        break;

      default:
        err << emit_dispatch(c);
        break;
    }
  }

  err << start_line();
  pop_cursor();
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_template_params_as_modparams(MnNode param_list) {
  Err err;
  push_cursor(param_list);

  for (auto c : param_list) {
    err << emit_ws_to(c);

    switch (c.sym) {
      case anon_sym_LT:
      case anon_sym_GT:
      case anon_sym_COMMA:
        err << skip_over(c);
        break;

      case sym_optional_parameter_declaration:
        err << emit_optional_param_as_modparam(c);
        break;

      case sym_parameter_declaration:
        err << ERR("Parameter '%s' must have a default value\n", c.text().c_str());
        break;

      default:
        err << emit_dispatch(c);
        break;
    }
  }

  err << start_line();
  pop_cursor();
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_module_ports(MnNode class_body) {
  Err err;

  if (current_mod.top()->needs_tick()) {
    err << emit_line("// global clock");
    err << emit_line("input logic clock,");
  }

  if (current_mod.top()->input_signals.size()) {
    err << emit_line("// input signals");
    for (auto f : current_mod.top()->input_signals) {
      err << emit_field_port(f);
    }
  }

  if (current_mod.top()->output_signals.size()) {
    err << emit_line("// output signals");
    for (auto f : current_mod.top()->output_signals) {
      err << emit_field_port(f);
    }
  }

  if (current_mod.top()->output_registers.size()) {
    err << emit_line("// output registers");
    for (auto f : current_mod.top()->output_registers) {
      err << emit_field_port(f);
    }
  }

  for (auto m : current_mod.top()->all_methods) {
    if (!m->is_init_ && m->is_public() && m->internal_callers.empty()) {
      err << emit_method_ports(m);
    }
  }

  // Remove trailing comma from port list
  if (at_comma) {
    err << emit_backspace();
  }

  return err;
}

//------------------------------------------------------------------------------
// Change class/struct to module, add default clk/rst inputs, add input and
// ouptut ports to module param list.

// + class_specifier (306) =
// |--# lit (82) = "class"
// |--# name: type_identifier (444) = "uart_rx"
// |--+ body: field_declaration_list (257) =

CHECK_RETURN Err MtCursor::emit_sym_class_specifier(MnNode n) {
  Err err = check_at(sym_class_specifier, n);

  auto node_class = n.child_by_sym(anon_sym_class);
  auto node_name  = n.get_field(field_name);
  auto node_body  = n.get_field(field_body);

  current_mod.push(lib->get_module(node_name.text()));

  err << emit_replacement(node_class, "module");
  err << emit_gap(node_class, node_name);
  err << emit_dispatch(node_name);
  err << emit_gap(node_name, node_body);

  // Insert the port list before the module body
  err << emit_print("(");
  push_indent(node_body);
  err << emit_module_ports(node_body);
  pop_indent(node_body);
  err << emit_line(");");

  // Insert the modparam list before the module body
  if (current_mod.top()->mod_param_list) {
    push_indent(node_body);
    err << emit_template_params_as_modparams(current_mod.top()->mod_param_list);
    err << start_line();
    pop_indent(node_body);
  }

  // Emit the module body
  err << emit_block(node_body, "", "endmodule");

  current_mod.pop();
  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Emit the module body, with a few modifications.
// Discard the opening brace
// Replace the closing brace with "endmodule"

CHECK_RETURN Err MtCursor::emit_sym_field_declaration_list(MnNode n) {
  Err err = check_at(sym_field_declaration_list, n);
  push_indent(n);

  bool convert = true;
  bool noconvert = false;
  bool exclude = false;
  bool dumpit = false;

  for (auto child : n) {
    if (child.sym == sym_comment && child.contains("metron_convert off")) convert = false;
    if (child.sym == sym_comment && child.contains("metron_convert on"))  convert = true;

    if (noconvert || !convert) {
      err << comment_out(child);
      noconvert = false;
      continue;
    }

    if (dumpit) {
      child.dump_tree();
      dumpit = false;
    }

    if (child.sym == sym_comment && child.contains("metron_noconvert")) noconvert = true;
    if (child.sym == sym_comment && child.contains("dumpit"))    dumpit = true;

    err << emit_ws_to(child);

    switch (child.sym) {
      case anon_sym_LBRACE:
        err << emit_replacement(child, block_prefix.top().c_str());
        break;
      case anon_sym_COLON:
        // The latest tree sitter is not putting this in with the access specifier...
        err << skip_over(child);
        break;
      case anon_sym_RBRACE:
        err << emit_replacement(child, block_suffix.top().c_str());
        break;
      default:
        err << emit_dispatch(child);
        break;
    }
  }

  pop_indent(n);
  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_expression_statement(MnNode node) {
  Err err = check_at(sym_expression_statement, node);

  // A call expression by itself (not on the right of an assignment) is replaced
  // by binding statements if it's a submodule call or if the method called
  // requires bindings.

  if (node.child(0).sym == sym_call_expression && can_omit_call(node.child(0))) {
    err << skip_over(node);
    return err;
  }
  else {
    return err << emit_children(node);
  }
}

//------------------------------------------------------------------------------
// Change logic<N> to logic[N-1:0]

// + type: template_type (348) =
// |--# name: type_identifier (444) = "logic"
// |--+ arguments: template_argument_list (351) =

CHECK_RETURN Err MtCursor::emit_sym_template_type(MnNode n) {
  Err err = check_at(sym_template_type, n);

  auto node_name = n.get_field(field_name);
  auto node_args = n.get_field(field_arguments);

  bool is_logic = node_name.match("logic");

  err << emit_dispatch(node_name);
  err << emit_gap(node_name, node_args);

  if (is_logic) {
    auto logic_size = node_args.first_named_child();
    if (logic_size.sym == sym_number_literal) {
      int width = atoi(logic_size.start());
      if (width > 1) {
        err << emit_replacement(node_args, "[%d:0]", width - 1);
      }
      else {
        err << skip_over(node_args);
      }
    }
    else {
      err << skip_over(node_args);
      err << emit_print("[");
      err << emit_splice(logic_size);
      err << emit_print("-1:0]");
    }
  }
  else {
    err << skip_over(node_args);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Change <param, param> to #(param, param)

// FIXME use string replacement?

CHECK_RETURN Err MtCursor::emit_sym_template_argument_list(MnNode n) {
  Err err = check_at(sym_template_argument_list, n);

  for (auto c : n) {
    if (c.sym == anon_sym_LT) {
      err << emit_replacement(c, " #(");
    }
    else if (c.sym == anon_sym_GT) {
      err << emit_replacement(c, ")");
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_everything() {
  Err err;

  cursor = current_source.top()->source;
  err << emit_dispatch(current_source.top()->root_node);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_toplevel_node(MnNode node) {
  Err err = check_at(node);

  switch (node.sym) {

    // Skip weird expression statements with only semis in them that Treesitter
    // generates for some reason
    case sym_expression_statement:
      if (node.text() != ";") {
        err << ERR("Found unexpected expression statement in translation unit\n");
      }
      err << skip_over(node);
      break;

    // Skip trailing semis after class decls
    case anon_sym_SEMI:
      err << skip_over(node);
      break;

    // Add semis after enums
    case sym_enum_specifier:
      err << emit_sym_enum_specifier(node);
      err << emit_print(";");
      break;

    default:
      err << emit_dispatch(node);
      break;
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------
// Discard any trailing semicolons in the translation unit.

CHECK_RETURN Err MtCursor::emit_sym_translation_unit(MnNode n) {
  Err err = check_at(sym_translation_unit, n);

  bool noconvert = false;
  bool dumpit = false;

  for (auto c : n) {
    err << emit_ws_to(c);

    if (noconvert) {
      noconvert = false;
      err << comment_out(c);
      err << check_done(c);
      continue;
    }

    if (dumpit) {
      c.dump_tree();
      dumpit = false;
    }

    if (c.sym == sym_comment && c.contains("metron_noconvert"))  noconvert = true;
    if (c.sym == sym_comment && c.contains("dumpit"))     dumpit = true;

    err << emit_toplevel_node(c);
  }

  // Toplevel blocks can have trailing whitespace that isn't contained by child
  // nodes.
  err << emit_ws();

  return err << check_done(n);
}

//------------------------------------------------------------------------------

// FIXME use string replace?

CHECK_RETURN Err MtCursor::emit_sym_declaration_list(MnNode n) {
  Err err = check_at(sym_declaration_list, n);

  for (auto c : n) {
    err << emit_ws_to(c);

    switch (c.sym) {
      // Skip braces around package decls
      case anon_sym_LBRACE:
      case anon_sym_RBRACE:
        err << skip_over(c);
        break;
      default:
        err << emit_dispatch(c);
        break;
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
/*
[000.004]  + namespace_definition (352) =
[000.004]  |--# lit (163) = "namespace"
[000.004]  |--# name: namespace_identifier (441) = "rv_config"
[000.004]  |--+ body: declaration_list (223) =
*/

CHECK_RETURN Err MtCursor::emit_sym_namespace_definition(MnNode n) {
  Err err = check_at(sym_namespace_definition, n);

  elide_type.push(false);
  elide_value.push(false);

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.sym == anon_sym_namespace) {
      err << emit_replacement(c, "package");
    }
    else if (c.sym == alias_sym_namespace_identifier) {
      err << emit_text(c);
      err << emit_print(";");
    }
    else if (c.sym == sym_declaration_list) {
      err << emit_dispatch(c);
      err << emit_print("endpackage");
    }
    else {
      err << emit_dispatch(c);
    }
  }

  elide_type.pop();
  elide_value.pop();

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Replace "0x" prefixes with "'h"
// Replace "0b" prefixes with "'b"
// Add an explicit size prefix if needed.

CHECK_RETURN Err MtCursor::emit_sym_number_literal(MnNode n) {
  Err err = check_at(sym_number_literal, n);

  std::string body = n.text();
  int size_cast = override_size.top();

  // Count how many 's are in the number
  int spacer_count = 0;
  int prefix_count = 0;

  for (auto& c : body)
    if (c == '\'') {
      c = '_';
      spacer_count++;
    }

  if (body.starts_with("0x")) {
    prefix_count = 2;
    if (!size_cast) size_cast = ((int)body.size() - 2 - spacer_count) * 4;
    err << emit_print("%d'h", size_cast);
  } else if (body.starts_with("0b")) {
    prefix_count = 2;
    if (!size_cast) size_cast = (int)body.size() - 2 - spacer_count;
    err << emit_print("%d'b", size_cast);
  } else {
    if (size_cast) {
      err << emit_print("%d'd", size_cast);
    }
  }

  if (spacer_count) {
    err << emit_print(body.c_str() + prefix_count);
  } else {
    err << emit_span(n.start() + prefix_count, n.end());
  }

  cursor = n.end();

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Change "return x" to "(funcname) = x" to match old Verilog return style.

CHECK_RETURN Err MtCursor::emit_sym_return_statement(MnNode n) {
  Err err = check_at(sym_return_statement, n);

  auto& method = current_method.top();
  auto method_name = method->name();

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.sym == anon_sym_return) {
      err << emit_replacement(c, method_name.c_str());

      if (method->emit_as_always_comb) {
        err << emit_print("_ret =");
      }
      else if (method->emit_as_always_ff) {
        err << emit_print("_ret <=");
      }
      else {
        err << emit_print(" =");
      }
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_identifier(MnNode n) {
  Err err = check_at(sym_identifier, n);

  auto name = n.name4();
  auto& rep = id_map.top();

  if (auto it = rep.find(name); it != rep.end()) {
    err << emit_replacement(n, it->second.c_str());
  } else if (preproc_vars.top().contains(name)) {
    err << emit_print("`");
    err << emit_text(n);
  } else if (name == "DONTCARE") {
    err << emit_replacement(n, "'x");
  } else {
    err << emit_text(n);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_type_identifier(MnNode n) {
  Err err = check_at(alias_sym_type_identifier, n);

  auto name = n.name4();
  auto& rep = id_map.top();

  if (auto it = rep.find(name); it != rep.end()) {
    err << emit_replacement(n, it->second.c_str());
  } else {
    err << emit_text(n);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// FIXME loop style

CHECK_RETURN Err MtCursor::emit_sym_template_declaration(MnNode n) {
  Err err = check_at(sym_template_declaration, n);

  // The class_specifier node does _not_ have a field name.
  MnNode class_specifier;
  for (auto child : (MnNode)n) {
    if (child.sym == sym_class_specifier) {
      class_specifier = MnNode(child);
    }
  }

  assert(!class_specifier.is_null());
  std::string class_name = class_specifier.get_field(field_name).text();

  current_mod.push(lib->get_module(class_name));
  err << emit_splice(class_specifier);
  current_mod.pop();

  cursor = n.end();
  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz if the field refers to a submodule port,
// so that it instead refers to a glue expression.

// FIXME loop style

CHECK_RETURN Err MtCursor::emit_sym_field_expression(MnNode n) {
  Err err = check_at(sym_field_expression, n);

  auto method = current_method.top();
  auto component_name = n.get_field(field_argument).text();
  auto component_field = n.get_field(field_field).text();

  auto component = current_mod.top()->get_component(component_name);

  bool is_port = component && component->_type_mod->is_port(component_field);

  is_port = component && component->_type_mod->is_port(component_field);
  // FIXME needs to be || is_argument

  bool is_port_arg = false;
  if (method && (method->emit_as_always_comb || method->emit_as_always_ff)) {
    for (auto c : method->param_nodes) {
      if (c.get_field(field_declarator).text() == component_name) {
        is_port_arg = true;
        break;
      }
    }
  }

  if (component && is_port) {
    auto field = n.text();
    for (auto& c : field) {
      if (c == '.') c = '_';
    }
    err << emit_replacement(n, field.c_str());
  } else if (is_port_arg) {
    err << emit_print("%s_", method->name().c_str());
    err << emit_text(n);
  }
  else {
    // Local struct reference
    err << emit_text(n);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_case_statement(MnNode n) {
  Err err = check_at(sym_case_statement, n);

  bool anything_after_colon = false;
  int colon_hit = false;

  for (auto child : n) {
    if (child.sym == anon_sym_COLON) {
      colon_hit = true;
    } else {
      if (colon_hit) {
        if (child.sym != sym_comment) {
          anything_after_colon = true;
          break;
        }
      }
    }
  }

  for (auto c : n) {
    err << emit_ws_to(c);

    switch (c.sym) {
      case sym_break_statement:
        err << skip_over(c);
        err << skip_ws_inside(n);
        break;
      case anon_sym_case:
        err << skip_over(c);
        err << skip_ws_inside(n);
        break;
      case anon_sym_COLON:
        if (anything_after_colon) {
          err << emit_text(c);
        } else {
          err << emit_replacement(c, ",");
        }
        break;
      case sym_compound_statement:
        block_prefix.push("begin");
        block_suffix.push("end");
        err << emit_dispatch(c);
        block_prefix.pop();
        block_suffix.pop();
        break;
      default:
        err << emit_dispatch(c);
        break;
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_switch_statement(MnNode n) {
  Err err = check_at(sym_switch_statement, n);

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.sym == anon_sym_switch) {
      err << emit_replacement(c, "case");
    }
    else if (c.field == field_body) {
      block_prefix.push("");
      block_suffix.push("endcase");
      err << emit_dispatch(c);
      block_prefix.pop();
      block_suffix.pop();
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Unwrap magic /*#foo#*/ comments to pass arbitrary text to Verilog.

CHECK_RETURN Err MtCursor::emit_sym_comment(MnNode n) {
  Err err = check_at(sym_comment, n);

  auto body = n.text();
  if (body.starts_with("/*#") && body.ends_with("#*/")) {
    body.erase(body.size() - 3, 3);
    body.erase(0, 3);
    err << emit_replacement(n, body.c_str());
  } else {
    err << emit_text(n);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Change "std::string" to "string".
// Change "enum::val" to "val" (SV doesn't support scoped enum values)

// + left: qualified_identifier (401) =
// |--# scope: namespace_identifier (441) = "rv_config"
// |--# lit (43) = "::"
// |--# name: identifier (1) = "DATA_BITS"

CHECK_RETURN Err MtCursor::emit_sym_qualified_identifier(MnNode n) {
  Err err = check_at(sym_qualified_identifier, n);

  bool elide_scope = false;

  for (auto c : n) {
    if (c.field == field_scope) {
      if (c.text() == "std") elide_scope = true;
      if (current_mod.top()->get_enum(c.text())) elide_scope = true;
      err << (elide_scope ? skip_over(c) : emit_dispatch(c));
    }
    else if (c.sym == anon_sym_COLON_COLON) {
      err << (elide_scope ? skip_over(c) : emit_dispatch(c));
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

bool MtCursor::branch_contains_component_call(MnNode n) {
  if (n.sym == sym_call_expression) {
    if (n.get_field(field_function).sym == sym_field_expression) {
      return true;
    }
  }

  for (const auto& c : n) {
    if (branch_contains_component_call(c)) return true;
  }

  return false;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_dispatch(MnNode n) {
  Err err = check_at(n);

  if (auto it = emit_sym_map.find(n.sym); it != emit_sym_map.end()) {
    err << check_at(n);
    err << it->second(this, n);
    err << check_done(n);
  }
  else if (!n.is_named()) {
    if (auto it = token_map.top().find(n.text()); it != token_map.top().end()) {
      err << emit_replacement(n, (*it).second.c_str());
    }
    else {
      err << emit_text(n);
    }
  }
  else {
    // KCOV_OFF
    err << ERR("%s : No handler for %s\n", __func__, n.ts_node_type());
    n.error();
    // KCOV_ON
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_block(MnNode n, const char* prefix, const char* suffix) {
  Err err;
  block_prefix.push(prefix);
  block_suffix.push(suffix);
  err << emit_dispatch(n);
  block_prefix.pop();
  block_suffix.pop();
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_splice(MnNode n) {
  Err err;
  push_cursor(n);
  err << emit_dispatch(n);
  pop_cursor();
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_nullptr(MnNode n) {
  return emit_replacement(n, "\"\"");
}

//------------------------------------------------------------------------------
// If statements _must_ use {}, otherwise we have no place to insert component
// bindings if the branch contains a component method call.

// + if_statement (267) =
// |--# lit (86) = "if"
// |--+ condition: condition_clause (362) =
// |--+ consequence: compound_statement (248) =
// |--# lit (87) = "else"
// |--+ alternative: compound_statement (248) =

CHECK_RETURN Err MtCursor::emit_sym_if_statement(MnNode node) {
  Err err = check_at(sym_if_statement, node);

  block_prefix.push("begin");
  block_suffix.push("end");

  for (auto child : node) {
    if (child.sym == sym_expression_statement) {
      if (branch_contains_component_call(child)) {
        return err << ERR("If branches that contain component calls must use {}.\n");
      }
    }
  }

  err << emit_children(node);

  block_prefix.pop();
  block_suffix.pop();
  return err << check_done(node);
}

//------------------------------------------------------------------------------
// + using_declaration (356) =
// |--# lit (164) = "using"
// |--# lit (163) = "namespace"
// |--# identifier (1) = "rv_constants"
// |--# lit (39) = ";"

CHECK_RETURN Err MtCursor::emit_sym_using_declaration(MnNode n) {
  Err err = check_at(sym_using_declaration, n);

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.sym == anon_sym_using) {
      err << emit_replacement(c, "import");
    }
    else if (c.sym == anon_sym_namespace) {
      err << skip_over(c);
      err << skip_ws_inside(n);
    }
    else if (c.sym == sym_identifier) {
      err << emit_dispatch(c);
      err << emit_print("::*");
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Const string:
// + declaration (210) =
// |--+ storage_class_specifier (249) =
// |  |--# lit (64) = "static"
// |--+ type_qualifier (250) =
// |  |--# lit (68) = "const"
// |--# type: primitive_type (80) = "char"
// |--+ declarator: init_declarator (247) =
// |  |--+ declarator: pointer_declarator (235) =
// |  |  |--# lit (23) = "*"
// |  |  |--# declarator: identifier (1) = "TEXT_HEX"
// |  |--# lit (63) = "="
// |  |--+ value: string_literal (300) =
// |     |--# lit (137) = "\""
// |     |--# lit (137) = "\""
// |--# lit (39) = ";"

// + declaration (210) =
// |--+ type: template_type (348) =
// |--+ declarator: init_declarator (247) =
// |  |--# declarator: identifier (1) = "b"
// |  |--# lit (63) = "="
// |  |--+ value: call_expression (289) =
// |--# lit (39) = ";"

CHECK_RETURN Err MtCursor::emit_sym_declaration(MnNode n) {
  Err err = check_at(sym_declaration, n);

  bool is_const = n.is_const();

  // Check for "static const char"
  if (is_const && n.get_field(field_type).text() == "char") {
    auto init_decl = n.get_field(field_declarator);
    auto pointer_decl = init_decl.get_field(field_declarator);
    auto name = pointer_decl.get_field(field_declarator);
    auto val = init_decl.get_field(field_value);

    err << emit_print("localparam string ");
    err << emit_splice(name);
    err << emit_print(" = ");
    err << emit_splice(val);
    err << prune_trailing_ws();
    err << emit_print(";");

    cursor = n.end();
    return err;
  }

  // If a declaration does _not_ have an init value and we're eliding types, elide the whole declaration
  // as it's already in the hoisted decl list.

  for (auto child : n) {
    if (child.field == field_declarator && (child.sym != sym_init_declarator) && elide_type.top()) {
        return err << skip_over(n);
    }
  }

  // Otherwise we emit the declaration as normal

  if (n.is_const()) {
    err << emit_print("parameter ");
  }

  for (auto child : n) {
    err << emit_ws_to(child);

    if (child.field == field_type && elide_type.top()) {
      err << skip_over(child);
      err << skip_ws_inside(n);
    }
    else if (child.field == field_declarator && child.sym == sym_init_declarator && elide_value.top()) {
      auto decl_name = child.get_field(field_declarator);
      err << emit_splice(decl_name);
      cursor = child.end();
    }
    else {
      err << emit_dispatch(child);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// "unsigned int" -> "int unsigned"

CHECK_RETURN Err MtCursor::emit_sym_sized_type_specifier(MnNode n) {
  Err err = check_at(sym_sized_type_specifier, n);

  MnNode node_size;
  MnNode node_type;

  for (auto c : n) {
    if (c.field == field_type) {
      node_type = c;
    }
    else if (c.sym == anon_sym_signed || c.sym == anon_sym_unsigned) {
      node_size = c;
    }
  }

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.field == field_type) {
      err << emit_splice(node_size);
    }
    else if (c.sym == anon_sym_signed || c.sym == anon_sym_unsigned) {
      err << emit_splice(node_type);
    }
    else {
      err << emit_dispatch(c);
    }
    cursor = c.end();
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// preproc_arg has leading whitespace for some reason

CHECK_RETURN Err MtCursor::emit_sym_preproc_arg(MnNode n) {
  Err err = check_at(n);

  // If we see any other #defined constants inside a #defined value,
  // prefix them with '`'
  std::string arg = n.text();
  std::string new_arg;

  for (int i = 0; i < arg.size(); i++) {
    for (const auto& def_pair : preproc_vars.top()) {
      if (def_pair.first == arg) continue;
      if (strncmp(&arg[i], def_pair.first.c_str(), def_pair.first.size()) ==
          0) {
        new_arg.push_back('`');
        break;
      }
    }
    new_arg.push_back(arg[i]);
  }

  err << emit_replacement(n, new_arg.c_str());

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_preproc_ifdef(MnNode n) {
  Err err = check_at(sym_preproc_ifdef, n);

  for (auto child : n) {
    err << emit_ws_to(child);
    err << emit_toplevel_node(child);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_preproc_def(MnNode n) {
  Err err = check_at(sym_preproc_def, n);

  MnNode node_name;

  for (auto child : n) {
    err << emit_ws_to(child);

    if (child.field == field_name) {
      node_name = child;
    }
    else if (child.field == field_value) {
      preproc_vars.top()[node_name.text()] = child;
    }

    err << emit_dispatch(child);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// TreeSitter gives us inconsistent semicolons between sections of the for
// expression :/

// + for_statement (272) =
// |--# lit (93) = "for"
// |--# lit (5) = "("
// |--+ initializer: declaration (210) =
// |--+ condition: binary_expression (283) =
// |--# lit (39) = ";"
// |--+ update: update_expression (284) =
// |--# lit (8) = ")"
// |--+ body: compound_statement (248) =

// + for_statement (272) =
// |--# lit (93) = "for"
// |--# lit (5) = "("
// |--+ initializer: assignment_expression (280) =
// |--# lit (39) = ";"
// |--+ condition: binary_expression (283) =
// |--# lit (39) = ";"
// |--+ update: update_expression (284) =
// |--# lit (8) = ")"
// |--+ body: compound_statement (248) =

CHECK_RETURN Err MtCursor::emit_sym_for_statement(MnNode n) {
  Err err = check_at(sym_for_statement, n);

#if 0
  auto node_for    = n.child_by_sym(anon_sym_for);
  auto node_lparen = n.child_by_sym(anon_sym_LPAREN);
  auto node_init   = n.get_field(field_initializer);
  auto node_cond   = n.get_field(field_condition);
  auto node_semi   = n.child_by_sym(anon_sym_SEMI);
  auto node_update = n.get_field(field_update);
  auto node_rparen = n.child_by_sym(anon_sym_RPAREN);
  auto node_body   = n.get_field(field_body);

  err << emit_dispatch(node_for);
  err << emit_gap(node_for, node_lparen);
  err << emit_dispatch(node_lparen);
  err << emit_gap(node_lparen, node_init);
  err << emit_dispatch(node_init);
  err << emit_gap(node_init, node_cond);
  err << emit_dispatch(node_cond);
  err << emit_gap(node_cond, node_semi);
  err << emit_dispatch(node_semi);
  err << emit_gap(node_semi, node_update);
  err << emit_dispatch(node_update);
  err << emit_gap(node_update, node_rparen);
  err << emit_dispatch(node_rparen);
  err << emit_gap(node_rparen, node_body);
  err << emit_block(node_body, "begin", "end");
#endif

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.field == field_body) {
      err << emit_block(c, "begin", "end");
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Emit the block with the correct type of "begin/end" pair, hoisting locals
// to the top of the body scope.

CHECK_RETURN Err MtCursor::emit_sym_compound_statement(MnNode n) {
  const std::string& delim_begin = block_prefix.top();
  const std::string& delim_end = block_suffix.top();

  Err err = check_at(sym_compound_statement, n);

  bool noconvert = false;
  bool dumpit = false;

  for (auto c : n) {
    err << emit_ws_to(c);

    if (noconvert) {
      noconvert = false;
      err << comment_out(c);
      continue;
    }

    if (dumpit) {
      c.dump_tree();
      dumpit = false;
    }

    if (c.sym == sym_comment && c.contains("metron_noconvert")) noconvert = true;
    if (c.sym == sym_comment && c.contains("dumpit")) dumpit = true;

    // We may need to insert input port bindings before any statement that
    // could include a call expression. We search the tree for calls and emit
    // those bindings here.
    if (c.sym != sym_compound_statement) {
      err << emit_call_arg_bindings(c);
    }

    switch (c.sym) {

      // Hoisted decls go immediately after the opening brace
      case anon_sym_LBRACE:
        err << emit_replacement(c, "%s", delim_begin.c_str());
        push_indent(n);
        err << emit_hoisted_decls(n);
        break;

      case sym_declaration:
        err << emit_dispatch(c);
        break;

      case sym_compound_statement:
        err << emit_block(c, "begin", "end");
        break;

      case anon_sym_RBRACE:
        err << emit_replacement(c, "%s", delim_end.c_str());
        pop_indent(n);
        break;

      default:
        err << emit_dispatch(c);
        break;
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_update_expression(MnNode n) {
  Err err = check_at(sym_update_expression, n);

  auto mod = current_mod.top();
  auto method = current_method.top();
  auto id = n.get_field(field_argument);
  auto op = n.get_field(field_operator);

  auto left_is_field = mod->get_field(id.name4()) != nullptr;

  const char* new_op = (method && method->is_tick_ && left_is_field) ? " <= " : " = ";

  if (n.get_field(field_operator).text() == "++") {
    err << emit_splice(id);
    err << emit_print(new_op);
    err << emit_splice(id);
    err << emit_print(" + 1");
  } else if (n.get_field(field_operator).text() == "--") {
    err << emit_splice(id);
    err << emit_print(new_op);
    err << emit_splice(id);
    err << emit_print(" - 1");
  } else {
    err << ERR("Unknown update expression %s\n", n.text().c_str());
  }

  cursor = n.end();
  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_children(MnNode n) {
  Err err = check_at(n);

  for (auto child : n) {
    err << emit_ws_to(child);
    err << emit_dispatch(child);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
