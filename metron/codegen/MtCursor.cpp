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
  { sym_argument_list,                  &MtCursor::emit_children },
  { sym_array_declarator,               &MtCursor::emit_children },
  { sym_assignment_expression,          &MtCursor::emit_sym_assignment_expression },
  { sym_binary_expression,              &MtCursor::emit_children },
  { sym_break_statement,                &MtCursor::emit_sym_break_statement },
  { sym_call_expression,                &MtCursor::emit_sym_call_expression },
  { sym_case_statement,                 &MtCursor::emit_sym_case_statement },
  { sym_class_specifier,                &MtCursor::emit_sym_class_specifier },
  { sym_comment,                        &MtCursor::emit_sym_comment },
  { sym_compound_statement,             &MtCursor::emit_sym_compound_statement },
  { sym_condition_clause,               &MtCursor::emit_sym_condition_clause },
  { sym_conditional_expression,         &MtCursor::emit_sym_conditional_expression },
  { sym_declaration,                    &MtCursor::emit_sym_declaration },
  { sym_enum_specifier,                 &MtCursor::emit_sym_enum_specifier },
  { sym_enumerator_list,                &MtCursor::emit_children },
  { sym_enumerator,                     &MtCursor::emit_children },
  { sym_expression_statement,           &MtCursor::emit_sym_expression_statement },
  { sym_field_declaration,              &MtCursor::emit_sym_field_declaration },
  { sym_field_expression,               &MtCursor::emit_sym_field_expression },
  { sym_for_statement,                  &MtCursor::emit_sym_for_statement },
  { sym_function_declarator,            &MtCursor::emit_sym_function_declarator },
  { sym_function_definition,            &MtCursor::emit_sym_function_definition },
  { sym_identifier,                     &MtCursor::emit_sym_identifier },
  { sym_if_statement,                   &MtCursor::emit_sym_if_statement },
  { sym_init_declarator,                &MtCursor::emit_sym_init_declarator },
  { sym_initializer_list,               &MtCursor::emit_sym_initializer_list },
  { sym_namespace_definition,           &MtCursor::emit_sym_namespace_definition },
  { sym_nullptr,                        &MtCursor::emit_sym_nullptr },
  { sym_number_literal,                 &MtCursor::emit_sym_number_literal },
  { sym_optional_parameter_declaration, &MtCursor::emit_children },
  { sym_parameter_declaration,          &MtCursor::emit_children },
  { sym_parameter_list,                 &MtCursor::emit_children},
  { sym_parenthesized_expression,       &MtCursor::emit_children },
  { sym_pointer_declarator,             &MtCursor::emit_sym_pointer_declarator },
  { sym_preproc_arg,                    &MtCursor::emit_sym_preproc_arg },
  { sym_preproc_call,                   &MtCursor::skip_over },
  { sym_preproc_def,                    &MtCursor::emit_sym_preproc_def },
  { sym_preproc_else,                   &MtCursor::skip_over },
  { sym_preproc_if,                     &MtCursor::skip_over },
  { sym_preproc_ifdef,                  &MtCursor::emit_sym_preproc_ifdef },
  { sym_preproc_include,                &MtCursor::emit_sym_preproc_include },
  { sym_primitive_type,                 &MtCursor::emit_sym_primitive_type },
  { sym_qualified_identifier,           &MtCursor::emit_sym_qualified_identifier },
  { sym_return_statement,               &MtCursor::emit_sym_return_statement },
  { sym_sized_type_specifier,           &MtCursor::emit_sym_sized_type_specifier },
  { sym_storage_class_specifier,        &MtCursor::emit_sym_storage_class_specifier },
  { sym_struct_specifier,               &MtCursor::emit_sym_struct_specifier },
  { sym_subscript_expression,           &MtCursor::emit_children },
  { sym_switch_statement,               &MtCursor::emit_sym_switch_statement },
  { sym_template_argument_list,         &MtCursor::emit_sym_template_argument_list },
  { sym_template_declaration,           &MtCursor::emit_sym_template_declaration },
  { sym_template_type,                  &MtCursor::emit_sym_template_type },
  { sym_translation_unit,               &MtCursor::emit_sym_translation_unit },
  { sym_type_definition,                &MtCursor::emit_children },
  { sym_type_descriptor,                &MtCursor::emit_children },
  { sym_unary_expression,               &MtCursor::emit_children },
  { sym_update_expression,              &MtCursor::emit_sym_update_expression },
  { sym_using_declaration,              &MtCursor::emit_sym_using_declaration },
};

//------------------------------------------------------------------------------

MtCursor::MtCursor(MtModLibrary* lib, MtSourceFile* source, MtModule* mod,
                   std::string* out)
    : str_out(out) {
  this->lib = lib;
  config.current_source = source;
  config.current_mod = mod;
  cursor = config.current_source->source;

  //block_prefix.push("begin");
  //block_suffix.push("end");
  override_size.push(0);
  indent.push("");

  // FIXME preproc_vars should be a set
  preproc_vars.push(string_to_node());
  preproc_vars.top()["IV_TEST"] = MnNode();

  id_replacements.push(string_to_string());

  elide_type.push(false);
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

CHECK_RETURN Err MtCursor::check_done(MnNode n) {
  Err err;

  if (cursor < n.end()) {
    err << ERR("Cursor was left inside the current node\n");
  }

  if (cursor > n.end()) {
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

  const char* begin = &config.current_source->source[ts_node_start_byte(n)] - 1;
  const char* end = &config.current_source->source[ts_node_start_byte(n)];

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
    LOG_C(color, "%c", c);
  }

  at_newline = c == '\n';
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_ws() {
  Err err;
  while (cursor < config.current_source->source_end && isspace(*cursor)) {
    err << emit_char(*cursor++);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_ws_to(const MnNode& n) {
  Err err;
  while (cursor < config.current_source->source_end && isspace(*cursor) &&
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
  if (n.is_null()) {
    return ERR("Skipping over null node\n");
  }

  Err err = check_at(n);
  if (echo) {
    LOG_C(0x8080FF, "%s", n.text().c_str());
  }
  cursor = n.end();
  line_elided = true;

  return err;
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
  auto end2 = end1;

  // If the block ends in a newline, put the comment terminator _before_ the newline
  if (end1[-1] == '\n') end1--;

  err << emit_print("/*");
  err << emit_span(start, end1);
  err << emit_print("*/");
  if (end1 != end2)err << emit_span(end1, end2);

  cursor = n.end();

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_span(const char* a, const char* b) {
  Err err;

  if (a == b) return err << ERR("Empty span\n");

  for (auto c = a; c < b; c++) {
    err << emit_char(*c);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_text(MnNode n) {
  Err err = check_at(n);

  err << emit_span(n.start(), n.end());
  cursor = n.end();

  return err << check_done(n);
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_print(const char* fmt, ...) {
  Err err;

  va_list args;
  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  auto buf = new char[size + 1];

  va_start(args, fmt);
  vsnprintf(buf, size_t(size) + 1, fmt, args);
  va_end(args);

  for (int i = 0; i < size; i++) {
    err << emit_char(buf[i], 0x80FF80);
  }
  delete[] buf;

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_replacement(MnNode n, const char* fmt, ...) {
  Err err = check_at(n);

  va_list args;
  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  auto buf = new char[size + 1];

  va_start(args, fmt);
  vsnprintf(buf, size_t(size) + 1, fmt, args);
  va_end(args);

  for (int i = 0; i < size; i++) {
    err << emit_char(buf[i], 0x80FFFF);
  }
  delete[] buf;

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

// FIXME handle the path .h to .sv with string replacement?

CHECK_RETURN Err MtCursor::emit_sym_preproc_include(MnNode n) {
  Err err = check_at(sym_preproc_include, n);

  auto path = n.get_field(field_path).text();
  if (!path.ends_with(".h\"")) return err << ERR("Include did not end with .h\n");

  path.resize(path.size() - 3);
  path = path + ".sv\"";

  for (auto child : n) {
    err << emit_ws_to(child);

    if (child.field == field_path) {
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
        err << emit_dispatch(child); break;
      }
      case field_operator: {
        // There may not be a method if we're in an enum initializer list.
        bool left_is_field = config.current_mod->get_field(lhs.name4()) != nullptr;
        if (config.current_method && config.current_method->is_tick_ && left_is_field) {
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
      case field_right:
        err << emit_dispatch(child);
        break;
      default:
        err << ERR("Unknown node type in sym_assignment_expression");
        break;
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

  auto arg0_text = arg0.text();

  if (arg_count == 1) {
    if (arg0.sym == sym_number_literal) {
      // Explicitly sized literal - 8'd10

      cursor = arg0.start();

      override_size.push(bx_width);
      err << emit_sym_number_literal(MnNode(arg0));
      override_size.pop();

      cursor = call.end();
    } else if (arg0.sym == sym_identifier ||
               arg0.sym == sym_subscript_expression) {
      if (arg0.text() == "DONTCARE") {
        // Size-casting expression
        err << emit_print("%d'bx", bx_width);
        cursor = call.end();
      } else {
        // Size-casting expression
        cursor = arg0.start();
        err << emit_print("%d'(", bx_width) << emit_dispatch(arg0)
            << emit_print(")");
        cursor = call.end();
      }

    } else {
      // Size-casting expression
      cursor = arg0.start();
      err << emit_print("%d'(", bx_width) << emit_dispatch(arg0)
          << emit_print(")");
      cursor = call.end();
    }
  } else if (arg_count == 2) {
    // Slicing logic array - foo[7:2]

    if (bx_width == 1) {
      // b1(foo, 7) -> foo[7]
      cursor = arg0.start();
      err << emit_dispatch(arg0);
      err << emit_print("[");
      cursor = arg1.start();
      err << emit_dispatch(arg1);
      err << emit_print("]");
      cursor = call.end();
    }
    else {
      cursor = arg0.start();
      err << emit_dispatch(arg0);
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
        cursor = arg1.start();
        err << emit_dispatch(arg1);
        err << emit_print(" : ");
        cursor = arg1.start();
        err << emit_dispatch(arg1);
      }
      err << emit_print("]");
      cursor = call.end();

    }
  } else {
    err << ERR("emit_static_bit_extract got > 1 args\n");
  }

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

  auto arg0_field = config.current_mod->get_field(arg0_text);
  if (arg0_field) {
    if (arg0_field->is_input()) {
      printf("INPUT FIELD %s!\n", arg0_field->cname());
    }
  }

  if (arg_count == 1) {
    // Non-literal size-casting expression - bits'(expression)
    if (arg0.text() == "DONTCARE") {
      cursor = bx_node.start();
      err << emit_dispatch(bx_node);
      err << emit_print("'('x)");
      cursor = call.end();
    } else {
      cursor = bx_node.start();
      err << emit_print("(");
      err << emit_dispatch(bx_node);
      err << emit_print(")");
      err << emit_print("'(");
      cursor = arg0.start();
      err << emit_dispatch(arg0);
      err << emit_print(")");
      cursor = call.end();
    }

  } else if (arg_count == 2) {
    // Non-literal slice expression - expression[bits+offset-1:offset];

    cursor = arg0.start();
    err << emit_dispatch(arg0);

    if (arg1.sym != sym_number_literal) {
      err << ERR("emit_dynamic_bit_extract saw a non-literal?\n");
    }
    int offset = atoi(arg1.start());

    err << emit_print("[%s+%d:%d]", bx_node.text().c_str(), offset - 1, offset);
    cursor = call.end();
  } else {
    err << ERR("emit_dynamic_bit_extract saw too many args?\n");
  }

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
    auto dst_method = config.current_mod->get_method(func.name4());
    if (dst_method && dst_method->needs_binding) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
// Replace function names with macro names where needed, comment out explicit
// init/tick/tock calls.

CHECK_RETURN Err MtCursor::emit_sym_call_expression(MnNode n) {
  assert(n.sym == sym_call_expression);
  Err err;

  MnNode func = n.get_field(field_function);
  MnNode args = n.get_field(field_arguments);

  std::string func_name = func.name4();

  auto method = config.current_mod->get_method(func_name);

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

  err << emit_ws_to(n);
  assert(cursor == n.start());

  if (func_name == "sra") {
    auto lhs = args.named_child(0);
    auto rhs = args.named_child(1);

    err << emit_print("($signed(");
    cursor = lhs.start();
    err << emit_dispatch(lhs);
    err << emit_print(") >>> ");
    cursor = rhs.start();
    err << emit_dispatch(rhs);
    err << emit_print(")");
    cursor = n.end();

  } else if (func_name == "signed") {
    err << emit_replacement(func, "$signed");
    err << emit_dispatch(args);
  } else if (func_name == "unsigned") {
    err << emit_replacement(func, "$unsigned");
    err << emit_dispatch(args);
  } else if (func_name == "clog2") {
    err << emit_replacement(func, "$clog2");
    err << emit_dispatch(args);
  } else if (func_name == "pow2") {
    err << emit_replacement(func, "2**");
    err << emit_dispatch(args);
  } else if (func_name == "readmemh") {
    err << emit_replacement(func, "$readmemh");
    err << emit_dispatch(args);
  } else if (func_name == "value_plusargs") {
    err << emit_replacement(func, "$value$plusargs");
    err << emit_dispatch(args);
  } else if (func_name == "write") {
    err << emit_replacement(func, "$write");
    err << emit_dispatch(args);
  } else if (func_name == "sign_extend") {
    err << emit_replacement(func, "$signed");
    err << emit_dispatch(args);
  } else if (func_name == "zero_extend") {
    err << emit_replacement(func, "$unsigned");
    err << emit_dispatch(args);
  }
  else if (func_name == "bx") {
    // Bit extract.
    auto template_arg = func.get_field(field_arguments).named_child(0);
    err << emit_dynamic_bit_extract(n, template_arg);
  }
  else if (func_name == "cat") {
    // Remove "cat" and replace parens with brackets
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
  }
  else if (func_name == "dup") {
    // Convert "dup<15>(x)" to "{15 {x}}"

    if (args.named_child_count() != 1) return err << ERR("dup() had too many children\n");

    err << skip_over(func);

    auto template_arg = func.get_field(field_arguments).named_child(0);
    int dup_count = atoi(template_arg.start());
    err << emit_print("{%d ", dup_count);
    err << emit_print("{");

    auto func_arg = args.named_child(0);
    cursor = func_arg.start();
    err << emit_dispatch(func_arg);

    err << emit_print("}}");
    cursor = n.end();
  }

  else if (func.sym == sym_field_expression) {
    // Component method call.

    auto node_component = n.get_field(field_function).get_field(field_argument);

    auto dst_component = config.current_mod->get_field(node_component.text());
    auto dst_mod = lib->get_module(dst_component->type_name());
    if (!dst_mod) return err << ERR("dst_mod null\n");
    auto node_func = n.get_field(field_function).get_field(field_field);
    auto dst_method = dst_mod->get_method(node_func.text());

    if (!dst_method) return err << ERR("dst_method null\n");

    err << emit_print("%s_%s_ret", node_component.text().c_str(), dst_method->cname());
    cursor = n.end();
  }
  else {
    // Internal method call.

    if (method->needs_binding) {
      err << emit_replacement(n, "%s_ret", method->cname());
    }
    else {
      err << emit_children(n);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Emit local variable declarations at the top of the block scope.

CHECK_RETURN Err MtCursor::emit_hoisted_decls(MnNode n) {
  Err err;

  push_cursor(n);
  push_config();
  elide_type.push(false);
  elide_value.push(true);

  for (const auto& c : (MnNode&)n) {
    if (c.sym == sym_declaration) {
      if (c.is_const()) {
        // Don't emit decls for localparams
        continue;
      } else {
        err << start_line();
        cursor = c.start();
        err << emit_dispatch(c);
      }
    }

    if (c.sym == sym_for_statement) {
      auto init = c.get_field(field_initializer);
      if (init.sym == sym_declaration) {
        err << start_line();
        cursor = init.start();
        err << emit_dispatch(init);
      }
    }
  }

  elide_type.pop();
  elide_value.pop();
  pop_config();
  pop_cursor();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_local_call_arg_binding(MtMethod* method, MnNode param, MnNode val) {
  Err err;

  auto param_name = param.get_field(field_declarator).text();

  err << emit_print("%s_%s = ", method->cname(), param_name.c_str());

  push_cursor(val);
  err << emit_dispatch(val);
  pop_cursor();

  err << prune_trailing_ws();
  err << emit_print(";");

  return err;
}

CHECK_RETURN Err MtCursor::emit_component_call_arg_binding(MnNode inst, MtMethod* method, MnNode param, MnNode val) {
  Err err;

  err << emit_print("%s_%s_%s = ", inst.text().c_str(),
                    method->name().c_str(),
                    param.get_field(field_declarator).text().c_str());

  push_cursor(val);
  err << emit_dispatch(val);
  pop_cursor();

  err << prune_trailing_ws();
  err << emit_print(";");

  return err;
}
//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_call_arg_bindings(MnNode n) {
  Err err;
  auto old_cursor = cursor;

  // Emit bindings for child nodes first, but do _not_ recurse into compound
  // blocks.

  for (auto c : n) {
    if (c.sym != sym_compound_statement) {
      err << emit_call_arg_bindings(c);
    }
  }

  // OK, now we can emit bindings for the call we're at.

  if (n.sym == sym_call_expression) {
    auto func_node = n.get_field(field_function);
    auto args_node = n.get_field(field_arguments);

    if (func_node.sym == sym_field_expression) {
      if (args_node.named_child_count() != 0) {
        auto inst_id = func_node.get_field(field_argument);
        auto meth_id = func_node.get_field(field_field);

        auto component = config.current_mod->get_component(inst_id.text());
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
          err << start_line();
        }
      }
    }
    else if (func_node.sym == sym_identifier) {
      auto method = config.current_mod->get_method(func_node.text().c_str());
      if (method && method->needs_binding) {
        for (int i = 0; i < method->param_nodes.size(); i++) {
          err << emit_local_call_arg_binding(
            method,
            method->param_nodes[i],
            args_node.named_child(i)
          );
          err << start_line();
        }
      }
    }
  }

  cursor = old_cursor;
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
      push_cursor(func_params);
      err << emit_module_parameter_list(func_params);
      pop_cursor();
      err << emit_replacement(c, "initial");
    }
    else if (c.sym == sym_field_initializer_list) {
      err << skip_over(c);
      err << skip_ws_inside(n);
    }
    else if (c.sym == sym_compound_statement) {
      block_prefix.push("begin");
      block_suffix.push("end");
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
/*
[000.017]  + function_definition (209) =
[000.017]  |--+ type: template_type (348) =
[000.017]  |--+ declarator: function_declarator (239) =
[000.017]  |--+ body: compound_statement (248) =
*/

CHECK_RETURN Err MtCursor::emit_func_as_func(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.field == field_type) {
      err << emit_print("function ");
      err << emit_dispatch(c);
    }
    else if (c.field == field_declarator) {
      err << emit_dispatch(c);
      err << prune_trailing_ws();
      err << emit_print(";");
    }
    else if (c.field == field_body) {
      block_prefix.push("");
      block_suffix.push("endfunction");
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
// sym_function_definition
//  field_type
//  field_declarator : sym_function_declarator
//  field_body : sym_compound_statement

CHECK_RETURN Err MtCursor::emit_func_as_task(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.field == field_type) {
      err << emit_replacement(c, "task automatic");
    }
    else if (c.field == field_declarator) {
      err << emit_dispatch(c);
      err << prune_trailing_ws();
      err << emit_print(";");
    }
    else if (c.field == field_body) {
      block_prefix.push("");
      block_suffix.push("endtask");
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

CHECK_RETURN Err MtCursor::emit_func_as_always_comb(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  auto func_ret = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);
  auto func_body = n.get_field(field_body);
  auto func_params = func_decl.get_field(field_parameters);

  id_replacements.push(id_replacements.top());
  for (auto c : func_params) {
    if (!c.is_named()) continue;
    id_replacements.top()[c.name4()] = func_decl.name4() + "_" + c.name4();
  }

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.field == field_type) {
      err << skip_over(c);
      err << skip_ws_inside(n);
    }
    else if (c.field == field_declarator) {
      err << emit_replacement(c, "always_comb begin : %s", c.name4().c_str());
      err << skip_ws_inside(n);
    }
    else if (c.field == field_body) {
      block_prefix.push("");
      block_suffix.push("end");
      err << emit_dispatch(c);
      block_prefix.pop();
      block_suffix.pop();
    }
  }

  id_replacements.pop();

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_func_as_always_ff(MnNode n) {
  Err err;

  auto func_decl = n.get_field(field_declarator);
  auto func_params = func_decl.get_field(field_parameters);

  id_replacements.push(id_replacements.top());
  for (auto c : func_params) {
    if (c.sym == sym_parameter_declaration) {
      id_replacements.top()[c.name4()] = func_decl.name4() + "_" + c.name4();
    }
  }

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.field == field_type) {
      err << emit_replacement(c, "always_ff @(posedge clock) begin : %s", func_decl.name4().c_str());
    }
    else if (c.field == field_declarator) {
      err << skip_over(c);
    }
    else if (c.field == field_body) {
      block_prefix.push("");
      block_suffix.push("end");
      err << emit_dispatch(c);
      block_prefix.pop();
      block_suffix.pop();
    }
    else if (c.sym == sym_comment) {
      err << emit_sym_comment(c);
    }
    else {
      err << ERR("Unknown node type in emit_func_as_always_ff");
    }
  }

  id_replacements.pop();

  assert(cursor == n.end());
  return err;
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

  config.current_method = config.current_mod->get_method(n.name4());
  assert(config.current_method);

  //----------
  // Emit a block declaration for the type of function we're in.

  if (config.current_method->emit_as_always_comb) {
    err << emit_func_as_always_comb(n);
  }
  else if (config.current_method->emit_as_always_ff) {
    err << emit_func_as_always_ff(n);
  }
  else if (config.current_method->emit_as_init) {
    err << emit_func_as_init(n);
  }
  else if (config.current_method->emit_as_task) {
    err << emit_func_as_task(n);
  }
  else if (config.current_method->emit_as_func) {
    err << emit_func_as_func(n);
  }

  //----------

  if (config.current_method->needs_binding) {
    err << emit_func_binding_vars(config.current_method);
  }

  config.current_method = nullptr;
  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_component_port_list(MnNode n) {
  Err err;

  auto node_type = n.child(0);  // type
  auto node_decl = n.child(1);  // decl
  auto node_semi = n.child(2);  // semi

  auto inst_name = node_decl.text();
  auto component_mod = lib->get_module(n.type5());

  cursor = node_type.start();
  err << emit_dispatch(node_type);

  bool has_template_params = node_type.sym == sym_template_type && component_mod->mod_param_list;
  bool has_constructor_params = component_mod->constructor && component_mod->constructor->param_nodes.size();

  if (has_template_params || has_constructor_params) {

    err << emit_print(" #(");
    indent.push(indent.top() + "  ");

    // Count up how many parameters we're passing to the submodule.
    int param_count = 0;

    // All the named elements of the template argument list count as parameters.
    if (has_template_params) {
      auto template_args = node_type.get_field(field_arguments);
      for (auto c : template_args) {
        if (c.is_named()) param_count++;
      }
    }

    // If the field initializer for this component passes arguments to the component's constructor,
    // count them as parameters.
    if (has_constructor_params && config.current_mod->constructor) {
      for(auto initializer : config.current_mod->constructor->_node.child_by_sym(sym_field_initializer_list)) {
        if (initializer.sym != sym_field_initializer) continue;
        if (initializer.child_by_sym(alias_sym_field_identifier).text() == inst_name) {
          for (auto c : initializer.child_by_sym(sym_argument_list)) {
            if (c.is_named()) {
              param_count++;
            }
          }
          break;
        }
      }
    }

    // Emit template arguments as module parameters
    if (has_template_params) {
      err << start_line();
      err << emit_print("// Template Parameters");

      auto template_args = node_type.get_field(field_arguments);

      std::vector<MnNode> params;
      std::vector<MnNode> args;

      for (auto c : component_mod->mod_param_list) {
        if (c.is_named()) params.push_back(c);
      }

      for (auto c : template_args) {
        if (c.is_named()) args.push_back(c);
      }

      //auto old_cursor = cursor;
      for (int param_index = 0; param_index < args.size(); param_index++) {
        cursor = args[param_index].start();

        auto param = params[param_index];
        auto arg = args[param_index];

        err << start_line();
        err << emit_print(".%s(", param.name4().c_str());
        err << emit_dispatch(arg);
        err << emit_print(")");
        if(--param_count) err << emit_print(",");
      }
    }

    // Emit constructor arguments as module parameters
    if (has_constructor_params) {
      err << start_line();
      err << emit_print("// Constructor Parameters");

      // The parameter names come from the submodule's constructor
      const auto& params = component_mod->constructor->param_nodes;

      // Find the initializer node for the component and extract arguments
      std::vector<MnNode> args;
      if (config.current_mod->constructor) {
        for(auto initializer : config.current_mod->constructor->_node.child_by_sym(sym_field_initializer_list)) {
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
        cursor = args[param_index].start();

        auto param = params[param_index];
        auto arg = args[param_index];

        err << start_line();
        err << emit_print(".%s(", param.name4().c_str());
        err << emit_dispatch(arg);
        err << emit_print(")");
        if(--param_count) err << emit_print(",");
      }
    }
    indent.pop();

    err << start_line();
    err << emit_print(")");
  }
  cursor = node_type.end();

  err << emit_ws_to(node_decl);
  err << emit_dispatch(node_decl);
  err << emit_print("(");

  {
    indent.push(indent.top() + "  ");

    if (component_mod->needs_tick()) {
      err << start_line();
      err << emit_print("// Global clock");
      err << start_line();
      err << emit_print(".clock(clock),");
      trailing_comma = true;
    }

    if (component_mod->input_signals.size()) {
      err << start_line();
      err << emit_print("// Input signals");
      for (auto f : component_mod->input_signals) {
        err << start_line();
        err << emit_print(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
        trailing_comma = true;
      }
    }

    if (component_mod->output_signals.size()) {
      err << start_line();
      err << emit_print("// Output signals");
      for (auto f : component_mod->output_signals) {
        err << start_line();
        err << emit_print(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
        trailing_comma = true;
      }
    }

    if (component_mod->output_registers.size()) {
      err << start_line();
      err << emit_print("// Output registers");
      for (auto f : component_mod->output_registers) {
        err << start_line();
        err << emit_print(".%s(%s_%s),", f->cname(), inst_name.c_str(), f->cname());
        trailing_comma = true;
      }
    }

    for (auto m : component_mod->all_methods) {
      if (m->is_constructor()) continue;
      if (m->is_public() && m->internal_callers.empty()) {

        if (m->param_nodes.size() || m->has_return()) {
          err << start_line();
          err << emit_print("// %s() ports", m->cname());
        }

        int param_count = m->param_nodes.size();
        for (int i = 0; i < param_count; i++) {
          auto param = m->param_nodes[i];
          auto node_type = param.get_field(field_type);
          auto node_decl = param.get_field(field_declarator);

          err << start_line();
          err << emit_print(".%s_%s(%s_%s_%s),", m->cname(), node_decl.text().c_str(),
                            inst_name.c_str(), m->cname(), node_decl.text().c_str());
          trailing_comma = true;
        }

        if (m->has_return()) {
          auto node_type = m->_node.get_field(field_type);
          auto node_decl = m->_node.get_field(field_declarator);
          auto node_name = node_decl.get_field(field_declarator);

          err << start_line();
          err << emit_print(".%s_ret(%s_%s_ret),", m->cname(),
                            inst_name.c_str(), m->cname());
          trailing_comma = true;
        }
      }
    }

    // Remove trailing comma from port list
    if (trailing_comma) {
      err << emit_backspace();
      trailing_comma = false;
    }

    indent.pop();
  }

  err << start_line();
  err << emit_print(")");
  err << emit_text(node_semi);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_field_as_component(MnNode n) {
  Err err = check_at(n);

  // FIXME loop style?

  std::string type_name = n.type5();
  auto component_mod = lib->get_module(type_name);

  auto node_type = n.child(0);  // type
  auto node_decl = n.child(1);  // decl
  auto node_semi = n.child(2);  // semi

  auto inst_name = node_decl.text();

  // Swap template arguments with the values from the template instantiation.
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

  err << emit_component_port_list(n);
  err << emit_submod_binding_fields(n);
  cursor = n.end();

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Emits the fields that come after a submod declaration

// module my_mod(
//   .foo(my_mod_foo)
// );
// logic my_mod_foo; <-- this part

CHECK_RETURN Err MtCursor::emit_submod_binding_fields(MnNode component_decl) {
  Err err;

  if (config.current_mod->components.empty()) {
    return err;
  }

  auto component_type_node = component_decl.child(0);  // type
  auto component_name_node = component_decl.child(1);  // decl
  auto component_semi_node = component_decl.child(2);  // semi

  auto component_name = component_name_node.text();
  auto component_cname = component_name.c_str();

  std::string type_name = component_type_node.type5();
  auto component_mod = lib->get_module(type_name);

  // Swap template arguments with the values from the template
  // instantiation.
  std::map<std::string, std::string> replacements;

  auto args = component_type_node.get_field(field_arguments);
  if (args) {
    int arg_count = args.named_child_count();
    for (int i = 0; i < arg_count; i++) {
      auto key = component_mod->all_modparams[i]->name();
      auto val = args.named_child(i).text();
      replacements[key] = val;
    }
  }

  for (auto n : component_mod->input_signals) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    auto old_source = config.current_source;
    auto old_mod = config.current_mod;
    auto old_cursor = cursor;

    push_config();

    config.current_source = component_mod->source_file;
    id_replacements.push(replacements);
    cursor = output_type.start();

    err << start_line();
    err << emit_dispatch(output_type);
    err << emit_ws();
    err << emit_print("%s_", component_cname);
    err << emit_dispatch(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");

    pop_config();

    config.current_source = old_source;
    config.current_mod = old_mod;
    id_replacements.pop();
    cursor = old_cursor;
  }

  for (auto n : component_mod->input_method_params) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    auto old_source = config.current_source;
    auto old_mod = config.current_mod;
    auto old_cursor = cursor;

    push_config();

    config.current_source = component_mod->source_file;
    id_replacements.push(replacements);
    cursor = output_type.start();

    err << start_line();
    err << emit_dispatch(output_type);
    err << emit_ws();
    err << emit_print("%s_%s_", component_cname, n->func_name.c_str());
    err << emit_dispatch(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");

    pop_config();

    config.current_source = old_source;
    config.current_mod = old_mod;
    id_replacements.pop();
    cursor = old_cursor;
  }

  for (auto n : component_mod->output_signals) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    auto old_source = config.current_source;
    auto old_mod = config.current_mod;
    auto old_cursor = cursor;

    push_config();

    config.current_source = component_mod->source_file;
    id_replacements.push(replacements);
    cursor = output_type.start();

    err << start_line();
    err << emit_dispatch(output_type);
    err << emit_ws();
    err << emit_print("%s_", component_cname);
    err << emit_dispatch(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");

    pop_config();

    config.current_source = old_source;
    config.current_mod = old_mod;
    id_replacements.pop();
    cursor = old_cursor;
  }

  for (auto n : component_mod->output_registers) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    auto old_source = config.current_source;
    auto old_mod = config.current_mod;
    auto old_cursor = cursor;

    push_config();

    config.current_source = component_mod->source_file;
    id_replacements.push(replacements);
    cursor = output_type.start();

    err << start_line();
    err << emit_dispatch(output_type);
    err << emit_ws();
    err << emit_print("%s_", component_cname);
    err << emit_dispatch(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");

    pop_config();

    config.current_source = old_source;
    config.current_mod = old_mod;
    id_replacements.pop();
    cursor = old_cursor;
  }

  for (auto m : component_mod->output_method_returns) {
    auto getter_type = m->_node.get_field(field_type);
    auto getter_decl = m->_node.get_field(field_declarator);
    auto getter_name = getter_decl.get_field(field_declarator);

    auto old_source = config.current_source;
    auto old_mod = config.current_mod;
    auto old_cursor = cursor;

    push_config();

    config.current_source = component_mod->source_file;
    id_replacements.push(replacements);
    cursor = getter_type.start();

    err << start_line();
    err << emit_dispatch(getter_type);
    err << emit_ws();
    err << emit_print("%s_", component_cname);
    err << emit_dispatch(getter_name);
    err << prune_trailing_ws();
    err << emit_print("_ret;");

    pop_config();

    config.current_source = old_source;
    config.current_mod = old_mod;
    id_replacements.pop();
    cursor = old_cursor;
  }

  return err;
}























































//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_qualified_identifier_as_type(MnNode n) {
  // enum class sized_enum : logic<8>::BASE { A8 = 0b01, B8 = 0x02, C8 = 3 };
  Err err = check_at(sym_qualified_identifier, n);
  auto node_scope = n.get_field(field_scope);
  cursor = node_scope.start();
  err << emit_dispatch(node_scope);
  cursor = n.end();
  return err << check_done(n);
}

CHECK_RETURN Err MtCursor::emit_sym_enum_specifier(MnNode n) {
  Err err = check_at(sym_enum_specifier, n);

  // Extract enum name, if present.
  std::string enum_name = "";
  MnNode node_name = n.get_field(field_name);
  if (node_name) {
    enum_name = node_name.text();
  }

  // Extract enum bit width, if present.
  override_size.push(32);
  MnNode node_base = n.get_field(field_base);
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

  if (node_name) {
    err << emit_print("typedef ");
  }

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.sym == anon_sym_class) {
      err << skip_over(c);
      err << skip_ws_inside(n);
    }
    else if (c.field == field_name && c.sym == alias_sym_type_identifier) {
      err << skip_over(c);
      err << skip_ws_inside(n);
    }
    else if (c.sym == anon_sym_COLON) {
      err << skip_over(c);
      err << skip_ws_inside(n);
    }
    else if (c.field == field_base && c.sym == sym_qualified_identifier) {
      // enum class sized_enum : logic<8>::BASE { A8 = 0b01, B8 = 0x02, C8 = 3 };
      auto node_scope = c.get_field(field_scope);
      cursor = node_scope.start();
      err << emit_dispatch(node_scope);
      cursor = c.end();
    }
    else {
      err << emit_dispatch(c);
    }
  }

  if (node_name) {
    auto old_cursor = cursor;
    cursor = node_name.start();
    err << emit_print(" ");
    err << emit_sym_type_identifier(node_name);
    cursor = old_cursor;
  }

  override_size.pop();
  return err << check_done(n);
}





































































//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_init_declarator(MnNode node) {
  Err err = check_at(node);

  for (auto child : node) {
    err << emit_ws_to(child);

    if (child.field == field_declarator) {
      err << emit_dispatch(child);
      if (elide_value.top()) {
        cursor = node.end();
        break;
      }
    }
    else if (child.field == field_value) {
      if (elide_value.top()) {
        err << skip_over(child);
        err << skip_ws_inside(node);
      }
      else {
        err << emit_dispatch(child);
      }
    }
    else if (child.sym == anon_sym_EQ) {
      err << emit_dispatch(child);
    }
    else {
      err << ERR("Unknown node type in sym_init_declarator");
    }
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_pointer_declarator(MnNode node) {
  Err err = check_at(node);

  auto decl2 = node.get_field(field_declarator);
  cursor = decl2.start();

  err << emit_dispatch(decl2);

  return err << check_done(node);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_function_declarator(MnNode node) {
  Err err = check_at(node);

  push_config();
  elide_value.push(false);

  for (auto c : node) {
    err << emit_ws_to(c);

    if (c.sym == sym_parameter_list) {
      err << emit_dispatch(c);
      err << skip_ws_inside(node);
    }
    else if (c.sym == sym_type_qualifier) {
      err << skip_over(c);
      err << skip_ws_inside(node);
    }
    else {
      err << emit_dispatch(c);
    }
  }

  elide_value.pop();
  pop_config();
  return err << check_done(node);
}

//------------------------------------------------------------------------------

/*
[000.018]  + optional_parameter_declaration (321) =
[000.018]  |--# type: primitive_type (80) = "int"
[000.018]  |--# declarator: identifier (1) = "cycles_per_bit"
[000.018]  |--# lit (63) = "="
[000.018]  |--# default_value: number_literal (126) = "4"
*/

CHECK_RETURN Err MtCursor::emit_module_parameter_declaration(MnNode node) {
  Err err = check_at(sym_optional_parameter_declaration, node);

  for (auto c : node) {
    err << emit_ws_to(c);

    if (c.field == field_type) {
      err << skip_over(c);
      err << skip_ws_inside(node);
    }
    else if (c.sym == sym_type_qualifier) {
      err << skip_over(c);
      err << skip_ws_inside(node);
    }
    else {
      err << emit_dispatch(c);
    }
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------
// Emit field declarations. For components, also emit glue declarations and
// append the glue parameter list to the field.

// field_declaration = { type:type_identifier, declarator:field_identifier+ }
// field_declaration = { type:template_type,   declarator:field_identifier+ }
// field_declaration = { type:enum_specifier,  bitfield_clause (TREESITTER BUG)
// }

CHECK_RETURN Err MtCursor::emit_sym_field_declaration(MnNode n) {
  Err err = check_at(n);
  assert(n.sym == sym_field_declaration);

  // Struct outside of class
  if (config.current_mod == nullptr) {
    // sym_field_declaration
    //   field_type : sym_template_type
    //   field_declarator : sym_field_identifier
    //   lit ;

    for (auto c : n) {
      err << emit_ws_to(c);

      if (c.sym == sym_storage_class_specifier) {
        err << skip_over(c);
        err << skip_ws_inside(n);
      }
      else if (c.sym == sym_type_qualifier) {
        err << skip_over(c);
        err << skip_ws_inside(n);
      }
      else {
        err << emit_dispatch(c);
      }
    }

    return err << check_done(n);
  }

  // Const local variable
  if (n.is_const()) {
    err << emit_ws_to(n);
    err << emit_print("localparam ");

    for (auto c : n) {
      err << emit_ws_to(c);

      if (c.sym == sym_storage_class_specifier) {
        err << skip_over(c);
        err << skip_ws_inside(n);
      }
      else if (c.sym == sym_type_qualifier) {
        err << skip_over(c);
        err << skip_ws_inside(n);
      }
      else {
        err << emit_dispatch(c);
      }
    }

    return err << check_done(n);
  }

  // Enum
  if (n.get_field(field_type).sym == sym_enum_specifier) {
    return emit_children(n);
  }

  //----------
  // Actual fields

  auto field = config.current_mod->get_field(n.name4());
  assert(field);

  if (field->is_component()) {
    // Component
    err << emit_field_as_component(n);
    return err << check_done(n);
  }
  else if (field->is_port()) {
    // Ports don't go in the class body.
    err << skip_over(n);
    return err << check_done(n);
  }
  else if (field->is_dead()) {
    err << comment_out(n);
    return err << check_done(n);
  }
  else {
    for (auto c : n) {
      err << emit_ws_to(c);

      if (c.field == field_type) {
        err << emit_dispatch(c);
      }
      else if (c.field == field_declarator) {
        push_config();
        elide_value.push(false);
        err << emit_dispatch(c);
        elide_value.pop();
        pop_config();
      }
      else if (c.sym == anon_sym_SEMI) {
        err << emit_text(c);
      }
      else {
        err << ERR("Unknown node type in field");
      }
    }
    return err << check_done(n);
  }
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_struct_specifier(MnNode n) {
  Err err = check_at(n);
  assert(n.sym == sym_struct_specifier);

  // FIXME loop style
  // FIXME - Do we _have_ to mark it as packed?

  auto node_name = n.get_field(field_name);
  auto node_body = n.get_field(field_body);

  if (node_name) {
    // struct Foo {};

    err << emit_print("typedef ");
    err << emit_text(n.child(0));
    err << emit_print(" packed ");
    cursor = node_body.start();
    err << emit_sym_field_declaration_list(node_body, true);
    err << emit_print(" ");

    push_cursor(node_name);
    err << emit_dispatch(node_name);
    pop_cursor();

    err << emit_print(";");
    cursor = n.end();
  } else {
    // typedef struct {} Foo;

    err << emit_text(n.child(0));
    err << emit_print(" packed ");
    err << emit_sym_field_declaration_list(node_body, true);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_param_port(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  push_cursor(node_type);
  err << emit_print("input ");
  err << emit_dispatch(node_type);
  pop_cursor();

  push_cursor(node_name);
  err << emit_print(" %s_", m->cname());
  err << emit_dispatch(node_name);
  err << emit_print(",");
  pop_cursor();

  trailing_comma = true;

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_param_binding(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  push_cursor(node_type);
  err << emit_dispatch(node_type);
  pop_cursor();

  push_cursor(node_name);
  err << emit_print(" %s_", m->cname());
  err << emit_dispatch(node_name);
  err << emit_print(";");
  pop_cursor();

  trailing_comma = false;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_return_port(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  push_cursor(node_type);
  err << emit_print("output ");
  err << emit_dispatch(node_type);
  err << emit_print(" %s_ret,", m->cname());
  pop_cursor();

  trailing_comma = true;

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_return_binding(MtMethod* m, MnNode node_type, MnNode node_name) {
  Err err;

  push_cursor(node_type);
  err << emit_dispatch(node_type);
  err << emit_print(" %s_ret;", m->cname());
  pop_cursor();

  trailing_comma = false;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_method_ports(MtMethod* m) {
  Err err;

  if (m->param_nodes.size() || m->has_return()) {
    err << start_line();
    err << emit_print("// %s() ports", m->cname());
  }

  for (auto& param : m->param_nodes) {
    auto node_type = param.get_field(field_type);
    auto node_decl = param.get_field(field_declarator);
    err << start_line();
    err << emit_param_port(m, node_type, node_decl);
  }

  if (m->has_return()) {
    auto node_type = m->_node.get_field(field_type);
    auto node_decl = m->_node.get_field(field_declarator);
    auto node_name = node_decl.get_field(field_declarator);
    err << start_line();
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
    err << start_line();
    err << emit_param_binding(m, node_type, node_decl);
  }

  if (m->has_return()) {
    auto node_type = m->_node.get_field(field_type);
    auto node_decl = m->_node.get_field(field_declarator);
    auto node_name = node_decl.get_field(field_declarator);
    err << start_line();
    err << emit_return_binding(m, node_type, node_name);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_field_port(MtField* f) {
  Err err;

  if (!f->is_public()) return err;
  if (f->is_component()) return err;

  if (f->is_public() && f->is_input()) {
    err << emit_print("input ");
  } else if (f->is_public() && f->is_signal()) {
    err << emit_print("output ");
  } else if (f->is_public() && f->is_register()) {
    err << emit_print("output ");
  } else {
    err << ERR("Unknown field port type for %s\n", f->cname());
  }

  if (err.has_err()) return err;

  auto node_type = f->get_type_node();
  auto node_decl = f->get_decl_node();

  push_cursor(node_type);
  err << emit_dispatch(node_type);
  pop_cursor();

  push_cursor(node_decl);
  err << emit_print(" ");
  err << emit_dispatch(node_decl);
  err << emit_print(",");
  pop_cursor();

  trailing_comma = true;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_module_parameter_list(MnNode param_list) {
  Err err;
  assert(cursor == param_list.start());
  assert(param_list.sym == sym_parameter_list || param_list.sym == sym_template_parameter_list);

  int param_count = 0;

  for (const auto& c : param_list) {
    switch (c.sym) {
      case sym_optional_parameter_declaration:
      case sym_parameter_declaration:
        param_count++;
        break;
    }
  }

  //----------
  // FIXME handle default template params

  for (const auto& c : param_list) {
    err << emit_ws_to(c);

    switch (c.sym) {
      case sym_optional_parameter_declaration:
        err << start_line();
        err << emit_print("parameter ");
        err << emit_module_parameter_declaration(c);
        err << emit_print(";");
        break;

      case sym_parameter_declaration:
        err << ERR("Parameter '%s' must have a default value\n", c.text().c_str());
        break;

      default:
        err << skip_over(c);
        break;
    }
  }

  err << start_line();
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_module_ports(MnNode class_body) {
  Err err;

  err << emit_print("(");

  // Save the indentation level of the struct body so we can use it in the
  // port list.
  push_indent(class_body);

  if (config.current_mod->needs_tick()) {
    err << start_line();
    err << emit_print("// global clock");
    err << start_line();
    err << emit_print("input logic clock,");
    trailing_comma = true;
  }

  if (config.current_mod->input_signals.size()) {
    err << start_line();
    err << emit_print("// input signals");
    for (auto f : config.current_mod->input_signals) {
      err << start_line();
      err << emit_field_port(f);
    }
  }

  if (config.current_mod->output_signals.size()) {
    err << start_line();
    err << emit_print("// output signals");
    for (auto f : config.current_mod->output_signals) {
      err << start_line();
      err << emit_field_port(f);
    }
  }

  if (config.current_mod->output_registers.size()) {
    err << start_line();
    err << emit_print("// output registers");
    for (auto f : config.current_mod->output_registers) {
      err << start_line();
      err << emit_field_port(f);
    }
  }

  for (auto m : config.current_mod->all_methods) {
    if (!m->is_init_ && m->is_public() && m->internal_callers.empty()) {
      err << emit_method_ports(m);
    }
  }
  // Remove trailing comma from port list
  if (trailing_comma) {
    err << emit_backspace();
    trailing_comma = false;
  }

  pop_indent(class_body);
  err << start_line();
  err << emit_print(");");

  return err;
}

//------------------------------------------------------------------------------
// Change class/struct to module, add default clk/rst inputs, add input and
// ouptut ports to module param list.

// sym_class_specifier
//   lit "class"
//   field_name:sym_type_idenfiier
//   field_body:sym_field_declaration_list

CHECK_RETURN Err MtCursor::emit_sym_class_specifier(MnNode n) {
  Err err = check_at(sym_class_specifier, n);

  auto class_lit = n.child(0);
  auto class_name = n.get_field(field_name);
  auto class_body = n.get_field(field_body);

  auto old_mod = config.current_mod;
  config.current_mod = lib->get_module(class_name.text());
  assert(config.current_mod);

  //----------

  err << emit_replacement(class_lit, "module");

  err << emit_ws_to(class_name);
  err << emit_dispatch(class_name);

  err << emit_ws_to(class_body);
  err << emit_module_ports(class_body);

  push_indent(class_body);
  if (config.current_mod->mod_param_list) {
    push_cursor(config.current_mod->mod_param_list);
    err << emit_module_parameter_list(config.current_mod->mod_param_list);
    pop_cursor();
  }
  cursor = class_body.start();
  err << emit_sym_field_declaration_list(class_body, false);
  pop_indent(class_body);

  cursor = n.end();
  config.current_mod = old_mod;

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_trigger_call(MtMethod* m) {
  Err err;

  if (m->has_return()) {
    err << emit_print("%s_ret = ", m->cname());
  }

  err << emit_print("%s(", m->cname());

  int param_count = m->param_nodes.size();
  for (int i = 0; i < param_count; i++) {
    auto param = m->param_nodes[i];
    err << emit_print("%s_%s", m->cname(),
                      param.get_field(field_declarator).text().c_str());
    if (i < param_count - 1) {
      err << emit_print(", ");
    }
  }
  err << emit_print(");");

  return err;
}

//------------------------------------------------------------------------------
// Emit the module body, with a few modifications.
// Discard the opening brace
// Replace the closing brace with "endmodule"

CHECK_RETURN Err MtCursor::emit_sym_field_declaration_list(MnNode n, bool is_struct) {
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
        if (!is_struct) {
          err << skip_over(child);
        }
        else {
          err << emit_text(child);
        }
        break;
      case sym_access_specifier:
        err << comment_out(child);
        break;
      case anon_sym_COLON:
        // The latest tree sitter is not putting this in with the access specifier...
        err << skip_over(child);
        break;
      case sym_field_declaration:
        err << emit_sym_field_declaration(child);
        break;
      case sym_function_definition:
        err << emit_sym_function_definition(child);
        break;
      case anon_sym_RBRACE:
        if (is_struct) {
          err << emit_text(child);
        }
        else {
          err << emit_replacement(child, "endmodule");
        }
        break;
      case sym_comment:
        err << emit_sym_comment(child);
        break;
      default:
        err << ERR("Unknown node type in sym_field_declaration_list");
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

CHECK_RETURN Err MtCursor::emit_sym_template_type(MnNode n) {
  Err err = check_at(sym_template_type, n);

  err << emit_sym_type_identifier(n.get_field(field_name));
  auto args = n.get_field(field_arguments);

  bool is_logic = n.get_field(field_name).match("logic");
  if (is_logic) {
    auto logic_size = args.first_named_child();
    switch (logic_size.sym) {
      case sym_number_literal: {
        int width = atoi(logic_size.start());
        if (width > 1) {
          err << emit_replacement(args, "[%d:0]", width - 1);
        }
        cursor = args.end();
        break;
      }
      default:
        cursor = logic_size.start();
        err << emit_print("[");
        err << emit_dispatch(logic_size);
        err << emit_print("-1:0]");
        break;
    }
  } else {
    // Don't do this here, it needs to go with the rest of the args in the full param list
    //err << emit_sym_template_argument_list(args);
  }
  cursor = n.end();

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Change <param, param> to #(param, param)

CHECK_RETURN Err MtCursor::emit_sym_template_argument_list(MnNode n) {
  Err err = check_at(sym_template_argument_list, n);

  for (auto c : n) {
    switch (c.sym) {
      case anon_sym_LT:
        err << emit_replacement(c, " #(");
        break;
      case anon_sym_GT:
        err << emit_replacement(c, ")");
        break;
      default:
        err << emit_dispatch(c);
        break;
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_everything() {
  Err err;

  cursor = config.current_source->source;
  err << emit_dispatch(config.current_source->root_node);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_toplevel_node(MnNode node) {
  Err err = check_at(node);

  switch (node.sym) {

    case sym_expression_statement:
      if (node.text() != ";") {
        err << ERR("Found unexpected expression statement in translation unit\n");
      }
      err << skip_over(node);
      break;

    case anon_sym_SEMI:
      err << skip_over(node);
      break;

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
    err << check_done(c);
  }

  // Toplevel blocks can have trailing whitespace that isn't contained by child
  // nodes.
  err << emit_ws();

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_declaration_list(MnNode n) {
  Err err = check_at(sym_declaration_list, n);

  for (auto c : n) {
    err << emit_ws_to(c);

    switch (c.sym) {
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
      err << emit_sym_declaration_list(c);
      err << emit_print("endpackage");
    }
    else {
      c.dump_tree();
      exit(1);
    }
  }

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

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.sym == anon_sym_return) {

      if (config.current_method->is_tock_) {
        assert(config.current_method->needs_binding || config.current_method->needs_ports);
        err << emit_replacement(c, "%s_ret =", config.current_method->name().c_str());
      }
      else if (config.current_method->is_func_ && config.current_method->is_public() && !config.current_method->called_in_module()) {
        assert(config.current_method->needs_binding || config.current_method->needs_ports);
        err << emit_replacement(c, "%s_ret =", config.current_method->name().c_str());
      }
      else if (config.current_method->is_tick_) {
        assert(config.current_method->needs_binding || config.current_method->needs_ports);
        err << emit_replacement(c, "%s_ret <=", config.current_method->name().c_str());
      }
      else {
        err << emit_replacement(c, "%s =", config.current_method->name().c_str());
      }
    } else if (c.is_expression()) {
      err << emit_dispatch(c);
    } else if (c.is_identifier()) {
      err << emit_dispatch(c);
    } else if (c.is_literal()) {
      err << emit_literal(c);
    } else if (c.sym == anon_sym_SEMI) {
      err << emit_text(c);
    }
    else {
      err << ERR("Unknown node type in sym_return");
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// FIXME translate types here

CHECK_RETURN Err MtCursor::emit_sym_primitive_type(MnNode n) {
  Err err = check_at(sym_primitive_type, n);

  err << emit_text(n);

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_identifier(MnNode n) {
  Err err = check_at(sym_identifier, n);

  auto name = n.name4();

  auto it = id_replacements.top().find(name);
  if (it != id_replacements.top().end()) {
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
  auto it = id_replacements.top().find(name);
  if (it != id_replacements.top().end()) {
    err << emit_replacement(n, it->second.c_str());
  } else {
    err << emit_text(n);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_template_declaration(MnNode n) {
  Err err = check_at(sym_template_declaration, n);

  MnNode class_specifier;
  MnNode param_list;

  for (auto child : (MnNode)n) {
    if (child.sym == sym_template_parameter_list) {
      param_list = MnNode(child);
    }

    if (child.sym == sym_class_specifier) {
      class_specifier = MnNode(child);
    }
  }

  assert(!class_specifier.is_null());
  std::string class_name = class_specifier.get_field(field_name).text();

  auto old_mod = config.current_mod;
  config.current_mod = lib->get_module(class_name);

  cursor = class_specifier.start();
  err << emit_dispatch(class_specifier);
  cursor = n.end();

  config.current_mod = old_mod;

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz if the field refers to a submodule port,
// so that it instead refers to a glue expression.

CHECK_RETURN Err MtCursor::emit_sym_field_expression(MnNode n) {
  Err err = check_at(sym_field_expression, n);

  auto component_name = n.get_field(field_argument).text();
  auto component_field = n.get_field(field_field).text();

  auto component = config.current_mod->get_component(component_name);

  bool is_port = component && component->_type_mod->is_port(component_field);
  //printf("is port %s %d\n", component_field.c_str(), is_port);

  is_port = component && component->_type_mod->is_port(component_field);
  // FIXME needs to be || is_argument

  bool is_port_arg = false;
  if (config.current_method && (config.current_method->emit_as_always_comb || config.current_method->emit_as_always_ff)) {
    for (auto c : config.current_method->param_nodes) {
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
    err << emit_print("%s_", config.current_method->name().c_str());
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
      case sym_identifier:
        err << emit_sym_identifier(c);
        break;
      case anon_sym_case:
        err << skip_over(c);
        err << skip_ws();
        break;
      case anon_sym_COLON:
        if (anything_after_colon) {
          err << emit_text(c);
        } else {
          err << emit_replacement(c, ",");
        }
        break;
      case sym_compound_statement:
        push_config();
        block_prefix.push("begin");
        block_suffix.push("end");
        elide_type.push(true);
        elide_value.push(false);
        err << emit_dispatch(c);
        elide_type.pop();
        elide_value.pop();
        block_prefix.pop();
        block_suffix.pop();
        pop_config();

        break;
      case sym_qualified_identifier:
        err << emit_sym_qualified_identifier(c);
        break;
      case sym_number_literal:
        err << emit_sym_number_literal(c);
        break;
      case anon_sym_default:
        err << emit_text(c);
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
    else if (c.field == field_condition) {
      err << emit_sym_condition_clause(c);
    }
    else if (c.field == field_body) {
      block_prefix.push("");
      block_suffix.push("endcase");
      err << emit_dispatch(c);
      block_prefix.pop();
      block_suffix.pop();
    }
    else {
      err << emit_leaf(c);
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
// Verilog doesn't use "break"

CHECK_RETURN Err MtCursor::emit_sym_break_statement(MnNode n) {
  Err err = check_at(sym_break_statement, n);

  err << skip_over(n);

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// TreeSitter nodes slightly broken for "a = b ? c : d;"...

CHECK_RETURN Err MtCursor::emit_sym_conditional_expression(MnNode n) {
  Err err = check_at(sym_conditional_expression, n);

  for (auto child : n) {
    err << emit_ws_to(child);
    err << emit_dispatch(child);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// Static variables become localparams at module level.

CHECK_RETURN Err MtCursor::emit_sym_storage_class_specifier(MnNode n) {
  Err err = check_at(sym_storage_class_specifier, n);

  err << skip_over(n);

  return err;
}

//------------------------------------------------------------------------------
// Change "std::string" to "string".
// Change "enum::val" to "val" (SV doesn't support scoped enum values)

CHECK_RETURN Err MtCursor::emit_sym_qualified_identifier(MnNode node) {
  Err err = check_at(sym_qualified_identifier, node);

  bool elide_scope = false;

  for (auto child : node) {
    if (child.field == field_scope) {
      if (child.text() == "std") elide_scope = true;
      if (config.current_mod->get_enum(child.text())) elide_scope = true;
    }
  }

  for (auto child : node) {
    if (child.sym == alias_sym_namespace_identifier) {
      if (elide_scope) {
        err << skip_over(child);
      }
      else {
        err << emit_text(child);
      }
    }
    else if (child.sym == sym_identifier) {
      err << emit_sym_identifier(child);
    }
    else if (child.sym == alias_sym_type_identifier) {
      err << emit_sym_type_identifier(child);
    }
    else if (child.sym == anon_sym_COLON_COLON) {
      if (elide_scope) {
        err << skip_over(child);
      }
      else {
        err << emit_text(child);
      }
    }
    else {
      err << ERR("Unknown node type in sym_qualified_identifier");
    }
  }

  return err << check_done(node);
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

CHECK_RETURN Err MtCursor::emit_literal(MnNode n) {
  Err err;

  switch(n.sym) {
    case sym_string_literal:
      err << emit_text(n);
      break;
    case sym_raw_string_literal:
      err << ERR("Raw string literals are not supported yet");
      break;
    case sym_char_literal:
      err << emit_text(n);
      break;
    case sym_number_literal:
      err << emit_sym_number_literal(n);
      break;
    case sym_user_defined_literal:
      err << ERR("User-defined literals are not supported yet");
      break;
    default:
      err << ERR("Unknown node type in literal");
      break;
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_dispatch(MnNode n) {
  Err err = check_at(n);

  if (n.is_leaf()) {
    err << emit_leaf(n);
  }
  else if (auto it = emit_sym_map.find(n.sym); it != emit_sym_map.end()) {
    err << it->second(this, n);
  }
  else {
    err << ERR("emit_dispatch - no handler for sym %s\n", n.ts_node_type());
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_nullptr(MnNode n) {
  return emit_replacement(n, "\"\"");
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_condition_clause(MnNode node) {
  assert(node.sym == sym_condition_clause);
  Err err;

  for (auto child : node) {
    err << emit_ws_to(child);

    if (child.sym == anon_sym_LPAREN || child.sym == anon_sym_RPAREN) {
      err << emit_text(child);
    }
    else if (child.is_comment()) {
      err << emit_sym_comment(child);
    }
    else if (child.is_identifier()) {
      err << emit_dispatch(child);
    }
    else if (child.is_expression()) {
      err << emit_dispatch(child);
    }
    else if (child.is_literal()) {
      err << emit_literal(child);
    }
    else {
      child.dump_tree();
      err << ERR("Unknown node type in sym_condition_clause");
    }
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------
// If statements _must_ use {}, otherwise we have no place to insert component
// bindings if the branch contains a component method call.

CHECK_RETURN Err MtCursor::emit_sym_if_statement(MnNode node) {
  Err err = check_at(sym_if_statement, node);

  for (auto child : node) {
    err << emit_ws_to(child);

    switch (child.sym) {
      case sym_condition_clause:
        err << emit_sym_condition_clause(child);
        break;
      case sym_if_statement:
        err << emit_sym_if_statement(child);
        break;
      case sym_compound_statement:
        block_prefix.push("begin");
        block_suffix.push("end");
        err << emit_dispatch(child);
        block_prefix.pop();
        block_suffix.pop();
        break;
      case sym_expression_statement:
        if (branch_contains_component_call(child)) {
          return err << ERR("If branches that contain component calls must use {}.\n");
        } else {
          push_config();
          block_prefix.push("begin");
          block_suffix.push("end");
          elide_type.push(true);
          elide_value.push(false);
          err << emit_dispatch(child);
          elide_type.pop();
          elide_value.pop();
          block_prefix.pop();
          block_suffix.pop();
          pop_config();

        }
        break;
      default:
        err << emit_leaf(child);
        break;
    }
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_using_declaration(MnNode n) {
  Err err = check_at(sym_using_declaration, n);

  // FIXME child index
  auto name = n.child(2).text();
  err << emit_replacement(n, "import %s::*;", name.c_str());

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_declaration(MnNode n) {
  Err err = check_at(sym_declaration, n);

  bool is_static = false;
  bool is_const = false;

  MnNode node_type;

  for (auto child : (MnNode)n) {
    if (child.sym == sym_storage_class_specifier && child.text() == "static")
      is_static = true;
    if (child.sym == sym_type_qualifier && child.text() == "const")
      is_const = true;

    if (child.field == field_type) node_type = child;
  }

  // Check for "static const char"
  if (is_const && node_type.text() == "char") {
    err << emit_print("localparam string ");
    auto init_decl = n.get_field(field_declarator);
    auto pointer_decl = init_decl.get_field(field_declarator);
    auto name = pointer_decl.get_field(field_declarator);
    cursor = name.start();
    err << emit_text(name);
    err << emit_print(" = ");

    auto val = init_decl.get_field(field_value);
    cursor = val.start();
    err << emit_text(val);
    err << prune_trailing_ws();
    err << emit_print(";");
    cursor = n.end();
    return err;
  }

  if (is_const) {
    err << emit_print("parameter ");
  }

  // Declarations inside scopes get their type removed, as the type+name decl
  // has been hoisted to the top of the scope.

  bool elide_decl = false;
  for (auto child : n) {
    if (child.field == field_declarator && child.is_identifier() && elide_type.top()) {
      elide_decl = true;
    }
  }

  if (elide_decl) {
    err << skip_over(n);
  }
  else {
    for (auto child : n) {
      err << emit_ws_to(child);

      if (child.sym == sym_storage_class_specifier) {
        err << skip_over(child);
        err << skip_ws();
      }
      else if (child.sym == sym_type_qualifier) {
        err << skip_over(child);
        err << skip_ws();
      }
      else if (child.field == field_type && elide_type.top()) {
        err << skip_over(child);
        err << skip_ws();
      }
      else {
        err << emit_dispatch(child);
      }
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
      push_cursor(node_size);
      err << emit_dispatch(node_size);
      pop_cursor();
      cursor = c.end();
    }
    else if (c.sym == anon_sym_signed || c.sym == anon_sym_unsigned) {
      push_cursor(node_type);
      err << emit_dispatch(node_type);
      pop_cursor();
      cursor = c.end();
    }
    else {
      err << emit_dispatch(c);
    }
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

// This doesn't really work right, TreeSitter interprets "logic<8>" in "#define FOO logic<8>" as a template function call
#if 0

  {
    MtSourceFile dummy_source;
    dummy_source.lib = current_source->lib;
    dummy_source.source = arg.data();
    dummy_source.source_end = arg.data() + arg.size();
    dummy_source.lang = tree_sitter_cpp();
    dummy_source.parser = ts_parser_new();
    ts_parser_set_language(dummy_source.parser, dummy_source.lang);
    dummy_source.tree = ts_parser_parse_string(dummy_source.parser, NULL, arg.data(), (uint32_t)arg.size());

    TSNode ts_root = ts_tree_root_node(dummy_source.tree);
    auto root_sym = ts_node_symbol(ts_root);
    dummy_source.root_node = MnNode(ts_root, root_sym, 0, &dummy_source);

    if (dummy_source.root_node.sym == sym_translation_unit) {
      dummy_source.root_node = dummy_source.root_node.child(0);
    }

    // TreeSitter tacks an ERROR node on if we parse just a bare integer constant
    if (dummy_source.root_node.sym == 0xFFFF) {
      dummy_source.root_node = dummy_source.root_node.child(0);
    }

    //dummy_source.root_node.dump_tree();

    MtCursor preproc_cursor(current_source->lib, &dummy_source, nullptr, str_out);
    preproc_cursor.preproc_vars = preproc_vars;

    //dummy_source.root_node.dump_tree();
    if (dummy_source.root_node.is_expression()) {
      err << preproc_cursor.emit_dispatch(dummy_source.root_node);
    }
    else if (dummy_source.root_node.is_statement()) {
      err << preproc_cursor.emit_statement(dummy_source.root_node);
    }
    else {
      err << preproc_cursor.emit_leaf(dummy_source.root_node);
    }
  }
  cursor = n.end();
#endif

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_preproc_def(MnNode n) {
  Err err = check_at(sym_preproc_def, n);

  MnNode node_name;

  for (auto child : n) {
    err << emit_ws_to(child);
    if (child.field == field_name) {
      err << emit_dispatch(child);
      node_name = child;
    }
    else if (child.field == field_value) {
      preproc_vars.top()[node_name.text()] = child;
      err << emit_sym_preproc_arg(child);
    }
    else {
      err << emit_leaf(child);
    }
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------
// FIXME need to do smarter stuff here...

CHECK_RETURN Err MtCursor::emit_preproc(MnNode n) {
  Err err = check_at(n);

  switch (n.sym) {
    case sym_preproc_def:
      err << emit_sym_preproc_def(n);
      break;
    case sym_preproc_if:
      err << skip_over(n);
      break;
    case sym_preproc_call:
      err << skip_over(n);
      break;
    case sym_preproc_arg:
      err << emit_sym_preproc_arg(n);
      break;
    case sym_preproc_include:
      err << emit_sym_preproc_include(n);
      break;
    case sym_preproc_ifdef:
      err << emit_sym_preproc_ifdef(n);
      break;
    default:
      n.error();
      break;
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_for_statement(MnNode node) {
  Err err = check_at(sym_for_statement, node);

  for (auto child : node) {
    err << emit_ws_to(child);

    if (child.sym == sym_declaration) {
      push_config();
      elide_type.push(true);
      elide_value.push(false);
      err << emit_dispatch(child);
      elide_type.pop();
      elide_value.pop();
      pop_config();
    }
    else if (child.is_expression()) {
      err << emit_dispatch(child);
    }
    else if (child.is_statement()) {
      push_config();
      block_prefix.push("begin");
      block_suffix.push("end");
      elide_type.push(true);
      elide_value.push(false);
      err << emit_dispatch(child);
      elide_type.pop();
      elide_value.pop();
      block_prefix.pop();
      block_suffix.pop();
      pop_config();
    }
    else {
      err << emit_dispatch(child);
    }
   }

  return err << check_done(node);
}

//------------------------------------------------------------------------------
// Emit the block with the correct type of "begin/end" pair, hoisting locals
// to the top of the body scope.

CHECK_RETURN Err MtCursor::emit_sym_compound_statement(MnNode node) {
  const std::string& delim_begin = block_prefix.top();
  const std::string& delim_end = block_suffix.top();

  Err err = check_at(sym_compound_statement, node);

  bool noconvert = false;
  bool dumpit = false;

  for (auto child : node) {
    err << emit_ws_to(child);

    if (noconvert) {
      noconvert = false;
      err << comment_out(child);
      continue;
    }

    if (dumpit) {
      child.dump_tree();
      dumpit = false;
    }

    if (child.sym == sym_comment && child.contains("metron_noconvert"))  noconvert = true;
    if (child.sym == sym_comment && child.contains("dumpit"))     dumpit = true;

    // We may need to insert input port bindings before any statement that
    // could include a call expression. We search the tree for calls and emit
    // those bindings here.
    if (child.sym != sym_compound_statement) {
      err << emit_call_arg_bindings(child);
    }

    switch (child.sym) {
      case anon_sym_LBRACE:
        err << emit_replacement(child, "%s", delim_begin.c_str());
        // Hoisted decls go immediately after the opening brace
        push_indent(node);
        err << emit_hoisted_decls(node);
        break;

      case sym_declaration:
        push_config();
        elide_type.push(true);
        elide_value.push(false);
        err << emit_dispatch(child);
        elide_type.pop();
        elide_value.pop();
        pop_config();
        break;

      case sym_compound_statement:
        push_config();
        block_prefix.push("begin");
        block_suffix.push("end");
        elide_type.push(true);
        elide_value.push(false);
        err << emit_dispatch(child);
        elide_type.pop();
        elide_value.pop();
        block_prefix.pop();
        block_suffix.pop();
        pop_config();
        break;

      case anon_sym_RBRACE:
        err << emit_replacement(child, "%s", delim_end.c_str());
        pop_indent(node);
        break;

      default:
        err << emit_dispatch(child);
        break;
    }
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_update_expression(MnNode n) {
  Err err = check_at(sym_update_expression, n);

  auto id = n.get_field(field_argument);
  auto op = n.get_field(field_operator);

  auto left_is_field = config.current_mod->get_field(id.name4()) != nullptr;

  if (n.get_field(field_operator).text() == "++") {
    push_cursor(id);
    err << emit_dispatch(id);
    pop_cursor();
    if (config.current_method && config.current_method->is_tick_ && left_is_field) {
      err << emit_print(" <= ");
    }
    else{
      err << emit_print(" = ");
    }
    push_cursor(id);
    err << emit_dispatch(id);
    pop_cursor();
    err << emit_print(" + 1");
  } else if (n.get_field(field_operator).text() == "--") {
    push_cursor(id);
    err << emit_dispatch(id);
    pop_cursor();
    if (config.current_method && config.current_method->is_tick_ && left_is_field) {
      err << emit_print(" <= ");
    }
    else{
      err << emit_print(" = ");
    }
    push_cursor(id);
    err << emit_dispatch(id);
    pop_cursor();
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

CHECK_RETURN Err MtCursor::emit_leaf(MnNode node) {
  Err err = check_at(node);

  static std::map<std::string,std::string> keyword_map = {
    {"#include", "`include"},
    {"#ifdef",   "`ifdef"},
    {"#ifndef",  "`ifndef"},
    {"#else",    "`else"},
    {"#elif",    "`elif"},
    {"#endif",   "`endif"},
    {"#define",  "`define"},
    {"#undef",   "`undef"},
  };

  if (!node.is_named()) {
    auto it = keyword_map.find(node.text());
    if (it != keyword_map.end()) {
      err << emit_replacement(node, (*it).second.c_str());
    }
    else {
      err << emit_text(node);
    }
    return err;
  }
  else if (node.is_literal()) {
    err << emit_literal(node);
  }
  else if (node.is_comment()) {
    err << emit_sym_comment(node);
  }
  else {
    // KCOV_OFF
    err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
    node.error();
    // KCOV_ON
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------
