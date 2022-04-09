#include "MtCursor.h"

#include <stdarg.h>

#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "Platform.h"
#include "metron_tools.h"

void print_escaped(char s);

//------------------------------------------------------------------------------

MtCursor::MtCursor(MtModLibrary* lib, MtSourceFile* source_file,
                   std::string* out)
    : lib(lib), source_file(source_file), str_out(out) {
  indent_stack.push_back("");
  cursor = source_file->source;
}

//------------------------------------------------------------------------------

void MtCursor::push_indent(MnNode body) {
  assert(body.sym == sym_compound_statement ||
         body.sym == sym_field_declaration_list);

  auto n = body.first_named_child().node;
  if (ts_node_is_null(n)) {
    indent_stack.push_back("");
    return;
  }

  if (ts_node_symbol(n) == sym_access_specifier) {
    n = ts_node_next_sibling(n);
  }
  const char* begin = &source_file->source[ts_node_start_byte(n)] - 1;
  const char* end = &source_file->source[ts_node_start_byte(n)];

  std::string indent;

  while (*begin != '\n' && *begin != '{') begin--;
  if (*begin == '{') {
    indent = "";
  } else {
    indent = std::string(begin + 1, end);
  }

  for (auto& c : indent) {
    assert(isspace(c));
  }

  indent_stack.push_back(indent);
}

void MtCursor::pop_indent(MnNode class_body) { indent_stack.pop_back(); }

//------------------------------------------------------------------------------
// Generic emit() methods

CHECK_RETURN Err MtCursor::emit_newline() {
  return emit_char('\n');
}

CHECK_RETURN Err MtCursor::emit_indent() {
  Err err;
  for (auto c : indent_stack.back()) {
    err |= emit_char(c);
  }
  return err;
}

CHECK_RETURN Err MtCursor::emit_char(char c) {
  Err err;
  if (c < 0 || !isspace(c)) {
    line_dirty = true;
  }

  if (c == '\n') {
    line_dirty = false;
    line_elided = false;
  }

  if (echo) putchar(c);
  str_out->push_back(c);

  at_newline = c == '\n';
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_ws() {
  Err err;
  while (cursor < source_file->source_end && isspace(*cursor)) {
    err |= emit_char(*cursor++);
  }
  return err;
}

CHECK_RETURN Err MtCursor::emit_ws_to_newline() {
  Err err;
  while (cursor < source_file->source_end && isspace(*cursor)) {
    auto c = *cursor++;
    err |= emit_char(c);
    if (c == '\n') {
      return err;
    }
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::skip_over(MnNode n) {
  Err err;
  if (n.is_null()) {
    err = 1;
  } else {
    assert(cursor == n.start());
    cursor = n.end();
    line_elided = true;
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::skip_ws() {
  Err err;
  while (*cursor && isspace(*cursor)) {
    cursor++;
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::comment_out(MnNode n) {
  Err err;
  assert(cursor == n.start());
  err |= emit_printf("/*");
  err |= emit_text(n);
  err |= emit_printf("*/");
  assert(cursor == n.end());
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_span(const char* a, const char* b) {
  Err err;
  assert(a != b);
  assert(cursor >= source_file->source);
  assert(cursor <= source_file->source_end);
  for (auto c = a; c < b; c++) {
    err |= emit_char(*c);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_text(MnNode n) {
  Err err;
  assert(cursor == n.start());
  err |= emit_span(n.start(), n.end());
  cursor = n.end();
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_printf(const char* fmt, ...) {
  Err err;
  if (echo) printf("\u001b[38;2;128;255;128m");

  va_list args;
  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  auto buf = new char[size + 1];

  va_start(args, fmt);
  vsnprintf(buf, size_t(size) + 1, fmt, args);
  va_end(args);

  for (int i = 0; i < size; i++) {
    err |= emit_char(buf[i]);
  }
  delete[] buf;

  if (echo) printf("\u001b[0m");
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_replacement(MnNode n, const char* fmt, ...) {
  Err err;
  assert(cursor == n.start());

  if (echo) printf("\u001b[38;2;255;255;128m");

  cursor = n.end();

  va_list args;
  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  auto buf = new char[size + 1];

  va_start(args, fmt);
  vsnprintf(buf, size_t(size) + 1, fmt, args);
  va_end(args);

  for (int i = 0; i < size; i++) {
    err |= emit_char(buf[i]);
  }
  delete[] buf;

  if (echo) printf("\u001b[0m");
  return err;
}

//------------------------------------------------------------------------------
// Replace "#include" with "`include" and ".h" with ".sv"

CHECK_RETURN Err MtCursor::emit_preproc_include(MnPreprocInclude n) {
  Err err;

  assert(cursor == n.start());

  err << emit_replacement(n.child(0), "`include") << emit_ws();

  auto path = n.path_node().text();
  assert(path.ends_with(".h\""));
  path.resize(path.size() - 3);
  path = path + ".sv\"";
  err << emit_printf(path.c_str());

  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// Change '=' to '<=' if lhs is a field and we're inside a sequential block.

// sym_assignment_expression := { left: identifier, operator: lit, right : expr
// }

CHECK_RETURN Err MtCursor::emit_assignment(MnAssignmentExpr n) {
  Err err;

  assert(cursor == n.start());

  auto lhs = n.lhs();
  auto rhs = n.rhs();

  err << emit_dispatch(lhs) << emit_ws();

  if (current_method->is_tick) {
    auto lhs_field = current_mod->get_field(lhs.name4());
    if (lhs_field) {
      bool lhs_is_reg1 = (lhs_field->state == FIELD_RD_WR_L) || (lhs_field->state == FIELD____WR_L);
      if (lhs_is_reg1) {
        err << emit_printf("<");
      }
    }
  }

  // Emit_dispatch makes sense here, as we really could have anything on the

  return err << emit_text(n.op()) << emit_ws() << emit_dispatch(rhs) << (cursor != n.end());
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_static_bit_extract(MnCallExpr call, int bx_width) {
  Err err;
  assert(cursor == call.start());

  int arg_count = call.args().named_child_count();

  auto arg0 = call.args().named_child(0);
  auto arg1 = call.args().named_child(1);

  if (arg_count == 1) {
    if (arg0.sym == sym_number_literal) {
      // Explicitly sized literal - 8'd10

      cursor = arg0.start();
      err |= emit_number_literal(MnNumberLiteral(arg0), bx_width);
      cursor = call.end();
    } else if (arg0.sym == sym_identifier ||
               arg0.sym == sym_subscript_expression) {
      if (arg0.text() == "DONTCARE") {
        // Size-casting expression
        err |= emit_printf("%d'bx", bx_width);
        cursor = call.end();
      } else {
        // Size-casting expression
        cursor = arg0.start();
        err << emit_printf("%d'(", bx_width) << emit_dispatch(arg0) << emit_printf(")");
        cursor = call.end();
      }

    } else {
      // Size-casting expression
      cursor = arg0.start();
      err << emit_printf("%d'(", bx_width) << emit_dispatch(arg0) << emit_printf(")");
      cursor = call.end();
    }
  } else if (arg_count == 2) {
    // Slicing logic array - foo[7:2]

    if (arg1.sym == sym_number_literal) {
      // Slice at offset
      if (bx_width == 1) {
        err |= emit_replacement(call, "%s[%s]", arg0.text().c_str(),
                         arg1.text().c_str());
      } else {
        int offset = atoi(arg1.start());
        err |= emit_replacement(call, "%s[%d:%d]", arg0.text().c_str(),
                         bx_width - 1 + offset, offset);
      }
    } else {
      if (bx_width == 1) {
        err |= emit_replacement(call, "%s[%s]", arg0.text().c_str(),
                         arg1.text().c_str());
        ;
      } else {
        err |= emit_replacement(call, "%s[%d + %s : %s]", arg0.text().c_str(),
                         bx_width - 1, arg1.text().c_str(),
                         arg1.text().c_str());
      }
    }

  } else {
    debugbreak();
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_dynamic_bit_extract(MnCallExpr call, MnNode bx_node) {
  Err err;

  assert(cursor == call.start());

  int arg_count = call.args().named_child_count();

  auto arg0 = call.args().named_child(0);
  auto arg1 = call.args().named_child(1);

  if (arg_count == 1) {
    // Non-literal size-casting expression - bits'(expression)
    if (arg0.text() == "DONTCARE") {
      cursor = bx_node.start();
      err |= emit_dispatch(bx_node);
      err |= emit_printf("'(1'bx)");
      cursor = call.end();
    } else {
      cursor = bx_node.start();
      err |= emit_printf("(");
      err |= emit_dispatch(bx_node);
      err |= emit_printf(")");
      err |= emit_printf("'(");
      cursor = arg0.start();
      err |= emit_dispatch(arg0);
      err |= emit_printf(")");
      cursor = call.end();
    }

  } else if (arg_count == 2) {
    // Non-literal slice expression - expression[bits+offset-1:offset];

    cursor = arg0.start();
    err |= emit_dispatch(arg0);

    if (arg1.sym != sym_number_literal) debugbreak();
    int offset = atoi(arg1.start());

    err |= emit_printf("[%s+%d:%d]", bx_node.text().c_str(), offset - 1, offset);
    cursor = call.end();
  } else {
    debugbreak();
  }

  return err;
}

//------------------------------------------------------------------------------
// Replace function names with macro names where needed, comment out explicit
// init/final/tick/tock calls.

CHECK_RETURN Err MtCursor::emit_call(MnCallExpr n) {
  Err err;

  assert(cursor == n.start());
  node_stack.push_back(n);

  MnFunc func = n.func();
  MnArgList args = n.args();

  // If we're calling a member function, look at the name of the member
  // function and not the whole foo.bar().

  std::string func_name = func.name();

  if (func_name == "coerce") {
    // Convert to cast? We probably shouldn't be calling coerce() directly.
    n.dump_tree();
    debugbreak();

  } else if (func_name == "sra") {
    auto lhs = args.named_child(0);
    auto rhs = args.named_child(1);

    err |= emit_printf("($signed(");
    cursor = lhs.start();
    err |= emit_dispatch(lhs);
    err |= emit_printf(") >>> ");
    cursor = rhs.start();
    err |= emit_dispatch(rhs);
    err |= emit_printf(")");
    cursor = n.end();

    // call.dump_tree();

  } else if (func_name == "signed") {

    err |= emit_replacement(func, "$signed");
    err |= emit_arg_list(args);
  } else if (func_name == "unsigned") {
    err |= emit_replacement(func, "$unsigned");
    err |= emit_arg_list(args);
  } else if (func_name == "clog2") {
    err |= emit_replacement(func, "$clog2");
    err |= emit_arg_list(args);
  } else if (func_name == "pow2") {
    err |= emit_replacement(func, "2**");
    err |= emit_arg_list(args);
  } else if (func_name == "readmemh") {
    err |= emit_replacement(func, "$readmemh");
    err |= emit_arg_list(args);
  } else if (func_name == "value_plusargs") {
    err |= emit_replacement(func, "$value$plusargs");
    err |= emit_arg_list(args);
  } else if (func_name == "write") {
    err |= emit_replacement(func, "$write");
    err |= emit_arg_list(args);
  } else if (func_name == "sign_extend") {
    err |= emit_replacement(func, "$signed");
    err |= emit_arg_list(args);
  } else if (func_name.starts_with("final")) {
    err |= comment_out(n);
  } else if (func_name.starts_with("tick")) {
    err |= comment_out(n);
  } else if (func_name.starts_with("tock")) {
    err |= comment_out(n);
  } else if (func_name == "bx") {
    // Bit extract.
    auto template_arg = func.as_templ().args().named_child(0);
    err |= emit_dynamic_bit_extract(n, template_arg);
  } else if (func_name == "b1")
    err |= emit_static_bit_extract(n, 1);
  else if (func_name == "b2")
    err |= emit_static_bit_extract(n, 2);
  else if (func_name == "b3")
    err |= emit_static_bit_extract(n, 3);
  else if (func_name == "b4")
    err |= emit_static_bit_extract(n, 4);
  else if (func_name == "b5")
    err |= emit_static_bit_extract(n, 5);
  else if (func_name == "b6")
    err |= emit_static_bit_extract(n, 6);
  else if (func_name == "b7")
    err |= emit_static_bit_extract(n, 7);
  else if (func_name == "b8")
    err |= emit_static_bit_extract(n, 8);
  else if (func_name == "b9")
    err |= emit_static_bit_extract(n, 9);

  else if (func_name == "b10")
    err |= emit_static_bit_extract(n, 10);
  else if (func_name == "b11")
    err |= emit_static_bit_extract(n, 11);
  else if (func_name == "b12")
    err |= emit_static_bit_extract(n, 12);
  else if (func_name == "b13")
    err |= emit_static_bit_extract(n, 13);
  else if (func_name == "b14")
    err |= emit_static_bit_extract(n, 14);
  else if (func_name == "b15")
    err |= emit_static_bit_extract(n, 15);
  else if (func_name == "b16")
    err |= emit_static_bit_extract(n, 16);
  else if (func_name == "b17")
    err |= emit_static_bit_extract(n, 17);
  else if (func_name == "b18")
    err |= emit_static_bit_extract(n, 18);
  else if (func_name == "b19")
    err |= emit_static_bit_extract(n, 19);

  else if (func_name == "b20")
    err |= emit_static_bit_extract(n, 20);
  else if (func_name == "b21")
    err |= emit_static_bit_extract(n, 21);
  else if (func_name == "b22")
    err |= emit_static_bit_extract(n, 22);
  else if (func_name == "b23")
    err |= emit_static_bit_extract(n, 23);
  else if (func_name == "b24")
    err |= emit_static_bit_extract(n, 24);
  else if (func_name == "b25")
    err |= emit_static_bit_extract(n, 25);
  else if (func_name == "b26")
    err |= emit_static_bit_extract(n, 26);
  else if (func_name == "b27")
    err |= emit_static_bit_extract(n, 27);
  else if (func_name == "b28")
    err |= emit_static_bit_extract(n, 28);
  else if (func_name == "b29")
    err |= emit_static_bit_extract(n, 29);

  else if (func_name == "b30")
    err |= emit_static_bit_extract(n, 30);
  else if (func_name == "b31")
    err |= emit_static_bit_extract(n, 31);
  else if (func_name == "b32")
    err |= emit_static_bit_extract(n, 32);
  else if (func_name == "b33")
    err |= emit_static_bit_extract(n, 33);
  else if (func_name == "b34")
    err |= emit_static_bit_extract(n, 34);
  else if (func_name == "b35")
    err |= emit_static_bit_extract(n, 35);
  else if (func_name == "b36")
    err |= emit_static_bit_extract(n, 36);
  else if (func_name == "b37")
    err |= emit_static_bit_extract(n, 37);
  else if (func_name == "b38")
    err |= emit_static_bit_extract(n, 38);
  else if (func_name == "b39")
    err |= emit_static_bit_extract(n, 39);

  else if (func_name == "b40")
    err |= emit_static_bit_extract(n, 40);
  else if (func_name == "b41")
    err |= emit_static_bit_extract(n, 41);
  else if (func_name == "b42")
    err |= emit_static_bit_extract(n, 42);
  else if (func_name == "b43")
    err |= emit_static_bit_extract(n, 43);
  else if (func_name == "b44")
    err |= emit_static_bit_extract(n, 44);
  else if (func_name == "b45")
    err |= emit_static_bit_extract(n, 45);
  else if (func_name == "b46")
    err |= emit_static_bit_extract(n, 46);
  else if (func_name == "b47")
    err |= emit_static_bit_extract(n, 47);
  else if (func_name == "b48")
    err |= emit_static_bit_extract(n, 48);
  else if (func_name == "b49")
    err |= emit_static_bit_extract(n, 49);

  else if (func_name == "b50")
    err |= emit_static_bit_extract(n, 50);
  else if (func_name == "b51")
    err |= emit_static_bit_extract(n, 51);
  else if (func_name == "b52")
    err |= emit_static_bit_extract(n, 52);
  else if (func_name == "b53")
    err |= emit_static_bit_extract(n, 53);
  else if (func_name == "b54")
    err |= emit_static_bit_extract(n, 54);
  else if (func_name == "b55")
    err |= emit_static_bit_extract(n, 55);
  else if (func_name == "b56")
    err |= emit_static_bit_extract(n, 56);
  else if (func_name == "b57")
    err |= emit_static_bit_extract(n, 57);
  else if (func_name == "b58")
    err |= emit_static_bit_extract(n, 58);
  else if (func_name == "b59")
    err |= emit_static_bit_extract(n, 59);

  else if (func_name == "b60")
    err |= emit_static_bit_extract(n, 60);
  else if (func_name == "b61")
    err |= emit_static_bit_extract(n, 61);
  else if (func_name == "b62")
    err |= emit_static_bit_extract(n, 62);
  else if (func_name == "b63")
    err |= emit_static_bit_extract(n, 63);
  else if (func_name == "b64")
    err |= emit_static_bit_extract(n, 64);

  else if (func_name == "cat") {
    // Remove "cat" and replace parens with brackets
    err |= skip_over(func);
    auto child_count = args.child_count();
    // for (const auto& arg : (MtNode&)args) {
    for (int i = 0; i < child_count; i++) {
      const auto& arg = args.child(i);
      switch (arg.sym) {
        case anon_sym_LPAREN: {
          err |= emit_replacement(arg, "{");
          break;
        }
        case anon_sym_RPAREN: {
          err |= emit_replacement(arg, "}");
          break;
        }
        default:
          err |= emit_dispatch(arg);
          break;
      }
      if (i < child_count - 1) {
        err |= emit_ws();
      }
    }
    if (cursor != n.end()) {
      n.dump_tree();
    }
  } else if (func_name == "dup") {
    // Convert "dup<15>(x)" to "{15 {x}}"

    assert(args.named_child_count() == 1);

    err |= skip_over(func);

    auto template_arg = func.as_templ().args().named_child(0);
    int dup_count = atoi(template_arg.start());
    err |= emit_printf("{%d ", dup_count);
    err |= emit_printf("{");

    auto func_arg = args.named_child(0);
    cursor = func_arg.start();
    err |= emit_dispatch(func_arg);

    err |= emit_printf("}}");
    cursor = n.end();
  } else if (func.sym == sym_field_expression) {
    // Submod output getter call - just translate the field expression
    err |= emit_dispatch(func);
    cursor = n.end();
  } else {
    // All other function/task calls go through normally.
    err |= emit_children(n);
  }

  node_stack.pop_back();

  if (cursor != n.end()) n.dump_tree();

  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------
// Replace "logic blah = x;" with "logic blah;"

CHECK_RETURN Err MtCursor::emit_init_declarator_as_decl(MnDecl n) {
  Err err;
  assert(cursor == n.start());

  err |= emit_template_type(n._type());
  err |= emit_ws();
  err |= emit_identifier(n._init_decl().decl());
  err |= emit_printf(";");
  cursor = n.end();

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Replace "logic blah = x;" with "blah = x;"

CHECK_RETURN Err MtCursor::emit_init_declarator_as_assign(MnDecl n) {
  Err err;

  assert(cursor == n.start());

  // We don't need to emit anything for decls without initialization values.
  if (!n.is_init_decl()) {
    err |= comment_out(n);
    return err;
  }

  assert(n.is_init_decl());
  cursor = n._init_decl().start();
  err |= emit_dispatch(n._init_decl());
  err |= emit_printf(";");
  cursor = n.end();

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Emit local variable declarations at the top of the block scope.

CHECK_RETURN Err MtCursor::emit_hoisted_decls(MnCompoundStatement n) {
  Err err;
  bool any_to_hoist = false;

  for (const auto& c : (MnNode&)n) {
    if (c.sym == sym_declaration) {
      bool is_localparam = c.sym == sym_declaration && c.child_count() >= 4 &&
                           c.child(0).text() == "static" &&
                           c.child(1).text() == "const";

      if (is_localparam) {
      } else {
        any_to_hoist = true;
        break;
      }
    }
  }

  if (!any_to_hoist) {
    return err;
  }

  /*
  if (!at_newline) {
    LOG_R("We're in some weird one-liner with a local variable?");
    debugbreak();
    emit_newline();
    emit_indent();
  }
  */

  MtCursor old_cursor = *this;
  for (const auto& c : (MnNode&)n) {
    if (c.sym == sym_declaration) {
      bool is_localparam = c.sym == sym_declaration && c.child_count() >= 4 &&
                           c.child(0).text() == "static" &&
                           c.child(1).text() == "const";

      if (is_localparam) {
      } else {
        cursor = c.start();

        auto d = MnDecl(c);

        if (d.is_init_decl()) {
          err |= emit_indent();
          err |= emit_init_declarator_as_decl(MnDecl(c));
          err |= emit_newline();
        } else {
          err |= emit_indent();
          err |= emit_dispatch(d);
          err |= emit_newline();
        }
      }
    }
  }
  *this = old_cursor;

  assert(at_newline);
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_func_decl(MnFuncDeclarator n) {
  assert(cursor == n.start());
  return emit_children(n);

  /*
  emit(MtFieldIdentifier(n.get_field(field_declarator)));
  emit_ws();
  emit(MtParameterList(n.get_field(field_parameters)));
  emit_ws();
  */
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_submod_input_port_bindings(MnNode n) {
  Err err;
  auto old_cursor = cursor;

  // Emit bindings for child nodes first, but do _not_ recurse into compound
  // blocks.

  for (int i = 0; i < n.child_count(); i++) {
    auto c = n.child(i);
    if (n.sym != sym_compound_statement) {
      err |= emit_submod_input_port_bindings(c);
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

        auto submod = current_mod->get_submod(inst_id.text());
        assert(submod);

        auto submod_mod = source_file->lib->get_module(submod->type_name());
        // auto submod_mod = submod->mod;
        // assert(submod_mod);

        auto submod_meth = submod_mod->get_method(meth_id.text());
        assert(submod_meth);

        for (int i = 0; i < submod_meth->params.size(); i++) {
          auto& param = submod_meth->params[i];
          err |= emit_printf("%s_%s = ", inst_id.text().c_str(), param.c_str());

          auto arg_node = args_node.named_child(i);
          cursor = arg_node.start();
          err |= emit_dispatch(arg_node);
          cursor = arg_node.end();

          err |= emit_printf(";");
          err |= emit_newline();
          err |= emit_indent();
        }
      }
    }
  }

  cursor = old_cursor;
  return err;
}

//------------------------------------------------------------------------------
// Change "init/tick/tock" to "initial begin / always_comb / always_ff", change
// void methods to tasks, and change const methods to funcs.

// func_def = { field_type, field_declarator, field_body }

CHECK_RETURN Err MtCursor::emit_func_def(MnFuncDefinition n) {
  Err err;
  // n.dump_tree();

  assert(cursor == n.start());
  node_stack.push_back(n);

  auto return_type = n.get_field(field_type);
  auto func_decl = n.decl();

  current_method = current_mod->get_method(n.name4());
  assert(current_method);

  if (current_method->is_tick) {
    int x = 1;
    x++;
  }

  //----------
  // Emit a block declaration for the type of function we're in.

  if (current_method->is_init) {
    if (!return_type.is_null()) err |= skip_over(return_type);
    err |= skip_ws();
    err |= emit_replacement(func_decl, "initial");
  } else if (current_method->is_tick) {
    /*
    if (in_public) {
      emit("\nXXXXX TICK CANNOT BE PUBLIC XXX\n");
    }
    */

    err |= skip_over(return_type);
    err |= skip_ws();
    // emit_replacement(func_decl, "always_ff @(posedge clock)");
    err |= emit_replacement(func_decl, "task %s();", current_method->name().c_str());
  } else if (current_method->is_tock) {
    err |= skip_over(return_type);
    err |= skip_ws();
    err |= emit_replacement(func_decl, "always_comb");
  } else if (current_method->is_task) {
    err |= skip_over(return_type);
    err |= skip_ws();
    err |= emit_printf("task ");
    err |= emit_dispatch(func_decl);
    err |= emit_printf(";");
  } else if (current_method->is_func) {
    if (in_public) {
      err |= skip_over(return_type);
      err |= skip_ws();
      err |= emit_replacement(func_decl, "always_comb");
      err |= emit_ws();

    } else {
      // emit("function %s ", n.type().text().c_str());
      err |= emit_printf("function ");
      err |= emit_dispatch(return_type);
      err |= emit_ws();
      err |= emit_dispatch(func_decl);
      err |= emit_printf(";");
    }
  } else {
    debugbreak();
  }

  err |= emit_ws();

  if (current_method->is_init)
    err |= emit_printf("begin /*%s*/", current_method->name().c_str());
  else if (current_method->is_tick) {
    // emit("begin : %s", current_function_name.c_str());
    // emit(" : %s", current_function_name.c_str());
  } else if (current_method->is_tock)
    err |= emit_printf("begin /*%s*/", current_method->name().c_str());
  else if (current_method->is_task)
    err |= emit_printf("");
  else if (current_method->is_func) {
    if (in_public) {
      err |= emit_printf("begin");
    } else {
      err |= emit_printf("");
    }
  } else
    debugbreak();

  //----------
  // Emit the function body with the correct type of "begin/end" pair,
  // hoisting locals to the top of the body scope.

  auto func_body = n.body();
  push_indent(func_body);

  auto body_count = func_body.child_count();
  for (int i = 0; i < body_count; i++) {
    auto c = func_body.child(i);

    err |= emit_submod_input_port_bindings(c);

    switch (c.sym) {
      case anon_sym_LBRACE:
        err |= skip_over(c);

        // while (*cursor != '\n') emit_char(*cursor++);
        // emit_char(*cursor++);
        err |= emit_ws_to_newline();
        err |= emit_hoisted_decls(func_body);
        err |= emit_ws();
        break;

      case sym_declaration: {
        MnDecl d(c);
        if (d.is_init_decl()) {
          err |= emit_init_declarator_as_assign(c);
        } else {
          // skip_over(c);
          err |= comment_out(c);
        }
        break;
      }

      case sym_expression_statement:
        err |= emit_dispatch(c);
        break;

      case anon_sym_RBRACE:
        err |= skip_over(c);
        break;

      case sym_return_statement:
        if (i != body_count - 1) {
          err |= emit_return(MnReturnStatement(c));
        } else {
          LOG_R("Return statement not at end of function body\n");
          err = true;
        }
        break;

      default:
        err |= emit_dispatch(c);
        break;
    }
    if (err) return err;
    if (i != body_count - 1) err |= emit_ws();
  }

  pop_indent(func_body);

  //----------

  if (current_method->is_init) {
    err |= emit_printf("end");
  }
  else if (current_method->is_tick) {
    err |= emit_printf("endtask");
    err |= emit_newline();
    err |= emit_indent();
    err |= emit_printf("always_ff @(posedge clock) %s();", current_method->name().c_str());
  } else if (current_method->is_tock) {
    err |= emit_printf("end");
  }
  else if (current_method->is_task) {
    err |= emit_printf("endtask");
  }
  else if (current_method->is_func) {
    if (current_method->is_public) {
      err |= emit_printf("end");
    } else {
      err |= emit_printf("endfunction");
    }
  } else
    debugbreak();

  assert(cursor == n.end());

  //----------

  current_method = nullptr;

  node_stack.pop_back();

  return err;
}

//------------------------------------------------------------------------------
// TreeSitterCPP support for enums is SUPER BROKEN and produces different parse
// trees in different contexts. :/
// So, we're going to dig the info we need out of it but it may be flaky.

/*
========== tree dump begin
[0] s236 field_declaration:
|   [0] f32 s230 type.enum_specifier:
|   |   [0] s79 lit: "enum"
|   |   [1] f22 s395 name.type_identifier: "opcode_e"
|   [1] f11 s272 default_value.initializer_list:
|   |   [0] s59 lit: "{"
|   |   [1] s258 assignment_expression:
|   |   |   [0] f19 s1 left.identifier: "OPCODE_LOAD"
|   |   |   [1] f23 s63 operator.lit: "="
|   |   |   [2] f29 s112 right.number_literal: "0x03"
|   |   [2] s7 lit: ","
|   |   [3] s60 lit: "}"
|   [2] s39 lit: ";"
========== tree dump end

========== tree dump begin
[0] s236 field_declaration:
|   [0] f32 s230 type.enum_specifier:
|   |   [0] s79 lit: "enum"
|   |   [1] f5 s231 body.enumerator_list:
|   |   |   [0] s59 lit: "{"
|   |   |   [1] s238 enumerator:
|   |   |   |   [0] f22 s1 name.identifier: "OPCODE_LOAD"
|   |   |   [2] s7 lit: ","
|   |   |   [3] s60 lit: "}"
|   [1] s39 lit: ";"
========== tree dump end
*/

CHECK_RETURN Err MtCursor::emit_field_as_enum_class(MnFieldDecl n) {
  Err err;
  assert(cursor == n.start());

  assert(n.sym == sym_field_declaration);

  std::string enum_name;
  MnNode node_values;
  int bit_width = 0;
  std::string enum_type = "";

  if (n.sym == sym_enum_specifier &&
      n.get_field(field_body).sym == sym_enumerator_list) {
    auto node_name = n.get_field(field_name);
    auto node_base = n.get_field(field_base);
    enum_name = node_name.is_null() ? "" : node_name.text();
    enum_type = node_base.is_null() ? "" : node_base.text();
    node_values = n.get_field(field_body);
    bit_width = 0;
  } else if (n.sym == sym_field_declaration &&
             n.get_field(field_type).sym == sym_enum_specifier &&
             n.get_field(field_type).get_field(field_body).sym ==
                 sym_enumerator_list) {
    // Anonymous enums have "body" nested under "type"
    node_values = n.get_field(field_type).get_field(field_body);
    bit_width = 0;
  } else if (n.sym == sym_field_declaration &&
             n.get_field(field_type).sym == sym_enum_specifier &&
             n.get_field(field_default_value).sym == sym_initializer_list) {
    // TreeSitterCPP BUG - "enum class foo : int = {}" misinterpreted as
    // default_value

    enum_name = n.get_field(field_type).get_field(field_name).text();
    node_values = n.get_field(field_default_value);
    bit_width = 0;
  } else if (n.sym == sym_field_declaration && n.child_count() == 3 &&
             n.child(0).sym == sym_enum_specifier &&
             n.child(1).sym == sym_bitfield_clause) {
    // TreeSitterCPP BUG - "enum class foo : logic<2> = {}" misinterpreted as
    // bitfield
    auto node_bitfield = n.child(1);
    auto node_compound = node_bitfield.child(1);
    auto node_basetype = node_compound.get_field(field_type);
    auto node_scope = node_basetype.get_field(field_scope);
    auto node_args = node_scope.get_field(field_arguments);
    auto node_bitwidth = node_args.child(1);
    assert(node_bitwidth.sym == sym_number_literal);

    enum_name = n.get_field(field_type).get_field(field_name).text();
    node_values = node_compound.get_field(field_value);
    bit_width = atoi(node_bitwidth.start());
  } else if (n.sym == sym_declaration && n.child_count() == 3 &&
             n.child(0).sym == sym_enum_specifier &&
             n.child(1).sym == sym_init_declarator) {
    // TreeSitterCPP BUG - "enum class foo : logic<2> = {}" in namespace
    // misinterpreted as declarator
    auto node_decl1 = n.get_field(field_declarator);
    auto node_decl2 = node_decl1.get_field(field_declarator);
    auto node_scope = node_decl2.get_field(field_scope);
    auto node_args = node_scope.get_field(field_arguments);
    auto node_bitwidth = node_args.child(1);
    assert(node_bitwidth.sym == sym_number_literal);

    enum_name = n.get_field(field_type).get_field(field_name).text();
    node_values = node_decl1.get_field(field_value);
    bit_width = atoi(node_bitwidth.start());
  } else {
    n.dump_tree();
    debugbreak();
  }

  err |= emit_printf("typedef enum ");
  if (bit_width == 1) {
    err |= emit_printf("logic ", bit_width - 1);
  } else if (bit_width > 1) {
    err |= emit_printf("logic[%d:0] ", bit_width - 1);
  } else if (enum_type.size()) {
    if (enum_type == "int") {
      err |= emit_printf("integer ");
    }
  } else {
    // emit("integer ");
  }

  override_size = bit_width;
  cursor = node_values.start();
  err |= emit_dispatch(node_values);
  if (enum_name.size()) {
    err |= emit_printf(" %s", enum_name.c_str());
  }
  override_size = 0;
  cursor = n.end();

  // BUG: Trailing semicolons are inconsistent.
  if (n.text().back() == ';') err |= emit_printf(";");
  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_field_as_submod(MnFieldDecl n) {
  Err err;
  std::string type_name = n.type5();
  auto submod_mod = lib->get_module(type_name);

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
      auto key = submod_mod->modparams[i]->name3();
      auto val = args.named_child(i).text();
      replacements[key] = val;
    }
  }

  cursor = node_type.start();
  err |= emit_dispatch(node_type);
  err |= emit_ws();
  err |= emit_dispatch(node_decl);
  err |= emit_ws();

  err |= emit_printf("(");

  indent_stack.push_back(indent_stack.back() + "  ");

  err |= emit_newline();
  err |= emit_indent();
  err |= emit_printf("// Inputs");

  err |= emit_newline();
  err |= emit_indent();
  err |= emit_printf(".clock(clock)");

  int port_count = int(submod_mod->inputs.size() + submod_mod->outputs.size());

  for (auto m : submod_mod->all_methods)
    if (m->is_public && m->has_return) port_count++;

  if (port_count) {
    err |= emit_printf(",");
  }

  int port_index = 0;

  for (auto n : submod_mod->inputs) {
    auto key = inst_name + "." + n->name();

    err |= emit_newline();
    err |= emit_indent();
    err |= emit_printf(".%s(%s_%s)", n->name().c_str(), inst_name.c_str(), n->name().c_str());

    if (port_index++ < port_count - 1) {
      err |= emit_printf(", ");
    }
  }

  err |= emit_newline();
  err |= emit_indent();
  err |= emit_printf("// Outputs");

  for (auto n : submod_mod->outputs) {
    err |= emit_newline();
    err |= emit_indent();
    err |= emit_printf(".%s(%s_%s)", n->name().c_str(), inst_name.c_str(), n->name().c_str());

    if (port_index++ < port_count - 1) {
      err |= emit_printf(", ");
    }
  }

  for (auto m : submod_mod->all_methods) {
    if (m->is_public && m->has_return) {
      err |= emit_newline();
      err |= emit_indent();
      err |= emit_printf(".%s(%s_%s)", m->name().c_str(), inst_name.c_str(),
           m->name().c_str());

      if (port_index++ < port_count - 1) {
        err |= emit_printf(", ");
      }
    }
  }

  indent_stack.pop_back();

  err |= emit_newline();
  err |= emit_indent();
  err |= emit_printf(")");
  err |= emit_dispatch(node_semi);
  err |= emit_newline();

  // emit("Output ports go here!\n");
  // emit_newline();
  err |= emit_output_ports(n);

  cursor = n.end();

  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_output_ports(MnFieldDecl submod) {
  Err err;

  if (current_mod->submods.empty()) {
    return err;
  }

  assert(at_newline);

  auto submod_type = submod.child(0);  // type
  auto submod_decl = submod.child(1);  // decl
  auto submod_semi = submod.child(2);  // semi

  std::string type_name = submod_type.type5();
  auto submod_mod = lib->get_module(type_name);

  // Swap template arguments with the values from the template
  // instantiation.
  std::map<std::string, std::string> replacements;

  auto args = submod_type.get_field(field_arguments);
  if (args) {
    int arg_count = args.named_child_count();
    for (int i = 0; i < arg_count; i++) {
      auto key = submod_mod->modparams[i]->name3();
      auto val = args.named_child(i).text();
      replacements[key] = val;
    }
  }

  for (auto n : submod_mod->inputs) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    MtCursor subcursor(lib, submod_mod->source_file, str_out);
    subcursor.echo = echo;
    subcursor.in_ports = true;
    subcursor.id_replacements = replacements;
    subcursor.cursor = output_type.start();

    err |= emit_indent();
    err |= subcursor.emit_dispatch(output_type);
    err |= subcursor.emit_ws();
    err |= emit_printf("%s_", submod_decl.text().c_str());
    err |= subcursor.emit_dispatch(output_decl);
    err |= emit_printf(";");
    err |= emit_newline();
  }

  for (auto n : submod_mod->outputs) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    MtCursor subcursor(lib, submod_mod->source_file, str_out);
    subcursor.echo = echo;
    subcursor.in_ports = true;
    subcursor.id_replacements = replacements;
    subcursor.cursor = output_type.start();

    err |= emit_indent();
    err |= subcursor.emit_dispatch(output_type);
    err |= subcursor.emit_ws();
    err |= emit_printf("%s_", submod_decl.text().c_str());
    err |= subcursor.emit_dispatch(output_decl);
    err |= emit_printf(";");
    err |= emit_newline();
  }

  for (auto m : submod_mod->all_methods) {
    if (!m->is_public || !m->has_return) continue;

    auto getter_type = m->node.get_field(field_type);
    auto getter_decl = m->node.get_field(field_declarator);
    auto getter_name = getter_decl.get_field(field_declarator);

    MtCursor sub_cursor(lib, submod_mod->source_file, str_out);
    sub_cursor.echo = echo;
    sub_cursor.in_ports = true;
    sub_cursor.id_replacements = replacements;

    sub_cursor.cursor = getter_type.start();

    err |= emit_indent();
    err |= sub_cursor.skip_ws();
    err |= sub_cursor.emit_dispatch(getter_type);
    err |= sub_cursor.emit_ws();
    err |= emit_printf("%s_", submod_decl.text().c_str());
    err |= sub_cursor.emit_dispatch(getter_name);
    err |= emit_printf(";");
    err |= emit_newline();
  }

  return err;
}

//------------------------------------------------------------------------------
// Emit field declarations. For submodules, also emit glue declarations and
// append the glue parameter list to the field.

// field_declaration = { type:type_identifier, declarator:field_identifier+ }
// field_declaration = { type:template_type,   declarator:field_identifier+ }
// field_declaration = { type:enum_specifier,  bitfield_clause (TREESITTER BUG)
// }

CHECK_RETURN Err MtCursor::emit_field_decl(MnFieldDecl n) {
  Err err;

  assert(cursor == n.start());

  // Handle "enum class", which is broken a bit in TreeSitterCpp
  if (n.type().child_count() >= 3 && n.type().child(0).text() == "enum" &&
      n.type().child(1).text() == "class" &&
      n.type().child(2).sym == alias_sym_type_identifier) {
    err |= emit_field_as_enum_class(n);
    return err;
  }

  std::string type_name = n.type5();

  if (lib->has_module(type_name)) {
    err |= emit_field_as_submod(n);
  } else if (n.type().is_enum()) {
    err |= emit_field_as_enum_class(n);
  } else if (current_mod->get_output(n.name().text())) {
    if (!in_ports) {
      // skip_to_next_sibling(n);
      // skip_over(n);
      err |= comment_out(n);
    } else {
      err |= emit_children(n);
    }
  } else if (n.is_static() && n.is_const()) {
    err |= emit_children(n);
  } else if (current_mod->get_enum(type_name)) {
    err |= emit_children(n);
  } else if (type_name == "logic") {
    err |= emit_children(n);
  } else {
    n.dump_tree();
    debugbreak();
  }
  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------
// Change class/struct to module, add default clk/rst inputs, add input and
// ouptut ports to module param list.

CHECK_RETURN Err MtCursor::emit_class(MnClassSpecifier n) {
  Err err;

  assert(cursor == n.start());
  node_stack.push_back(n);

  auto class_lit = n.child(0);
  auto class_name = n.get_field(field_name);
  auto class_body = n.get_field(field_body);

  //----------

  auto old_mod = current_mod;
  current_mod = source_file->get_module(class_name.text());
  assert(current_mod);

  err |= emit_indent();
  err |= emit_replacement(class_lit, "module");
  err |= emit_ws();
  err |= emit_dispatch(class_name);
  err |= emit_newline();

  // Patch the template parameter list in after the module declaration, before
  // the port list.

  if (current_mod->mod_param_list) {
    err |= emit_indent();
    MtCursor sub_cursor = *this;
    sub_cursor.cursor = current_mod->mod_param_list.start();
    for (const auto& c : (MnNode&)current_mod->mod_param_list) {
      switch (c.sym) {
        case anon_sym_LT:
          err |= sub_cursor.emit_replacement(c, "#(");
          err |= sub_cursor.emit_ws();
          break;
        case anon_sym_GT:
          err |= sub_cursor.emit_replacement(c, ")");
          err |= sub_cursor.skip_ws();
          break;

        case sym_parameter_declaration:
          err |= sub_cursor.emit_printf("parameter ");
          err |= sub_cursor.emit_dispatch(c);
          err |= sub_cursor.emit_ws();
          break;

        case sym_optional_parameter_declaration:
          err |= sub_cursor.emit_printf("parameter ");
          err |= sub_cursor.emit_dispatch(c);
          err |= sub_cursor.emit_ws();
          break;

        case anon_sym_COMMA:
          err |= sub_cursor.emit_text(c);
          err |= sub_cursor.emit_ws();
          break;

        default:
          c.dump_tree();
          assert(false);
          break;
      }
    }
    err |= emit_newline();
  }

  err |= emit_indent();
  err |= emit_printf("(");
  err |= emit_newline();

  {
    // Save the indentation level of the struct body so we can use it in the
    // port list.
    push_indent(class_body);

    in_ports = true;
    trim_namespaces = false;

    int port_count =
        int(current_mod->inputs.size() + current_mod->outputs.size());
    for (auto m : current_mod->all_methods)
      if (m->is_public && m->has_return) port_count++;

    int port_index = 0;

    err |= emit_indent();
    err |= emit_printf("input logic clock");
    if (port_count) {
      err |= emit_printf(",");
    }
    err |= emit_newline();

    for (auto input : current_mod->inputs) {
      err |= emit_indent();
      err |= emit_printf("input ");

      auto node_type = input->get_type_node();  // type
      auto node_decl = input->get_decl_node();  // decl

      MtCursor sub_cursor = *this;
      sub_cursor.cursor = node_type.start();
      err |= sub_cursor.emit_dispatch(node_type);
      err |= sub_cursor.emit_ws();
      err |= sub_cursor.emit_dispatch(node_decl);

      if (port_index++ < port_count - 1) {
        err |= emit_printf(",");
      }
      err |= emit_newline();
    }

    for (auto output : current_mod->outputs) {
      err |= emit_indent();
      err |= emit_printf("output ");

      auto node_type = output->get_type_node();  // type
      auto node_decl = output->get_decl_node();  // decl

      MtCursor sub_cursor = *this;
      sub_cursor.cursor = node_type.start();
      err |= sub_cursor.emit_dispatch(node_type);
      err |= sub_cursor.emit_ws();
      err |= sub_cursor.emit_dispatch(node_decl);

      if (port_index++ < port_count - 1) {
        err |= emit_printf(",");
      }
      err |= emit_newline();
    }

    for (auto m : current_mod->all_methods) {
      if (!m->is_public || !m->has_return) continue;

      err |= emit_indent();
      err |= emit_printf("output ");

      MtCursor sub_cursor = *this;

      auto getter_type = m->node.get_field(field_type);
      auto getter_decl = m->node.get_field(field_declarator);
      auto getter_name = getter_decl.get_field(field_declarator);

      // getter_decl.dump_tree();

      sub_cursor.cursor = getter_type.start();
      err |= sub_cursor.emit_dispatch(getter_type);
      err |= sub_cursor.emit_ws();
      sub_cursor.cursor = getter_name.start();
      err |= sub_cursor.emit_dispatch(getter_name);

      if (port_index++ < port_count - 1) {
        err |= emit_printf(",");
      }
      err |= emit_newline();
    }

    pop_indent(class_body);
    err |= emit_indent();
    err |= emit_printf(");");
    trim_namespaces = true;
    in_ports = false;
  }

  // Whitespace between the end of the port list and the module body.
  err |= skip_ws();

  // Emit the module body, with a few modifications.
  // Discard the opening brace
  // Replace the closing brace with "endmodule"

  assert(class_body.sym == sym_field_declaration_list);

  push_indent(class_body);

  auto body_size = class_body.child_count();
  for (int i = 0; i < body_size; i++) {
    auto c = class_body.child(i);
    switch (c.sym) {
      case anon_sym_LBRACE:
        err |= skip_over(c);
        err |= emit_ws();
        break;
      case anon_sym_RBRACE:
        err |= emit_replacement(c, "endmodule");
        break;
      default:
        err |= emit_dispatch(c);
        break;
    }
    if (err) return err;
    if (i != body_size - 1) {
      err |= emit_ws();
    }
  }

  pop_indent(class_body);

  cursor = n.end();

  current_mod = old_mod;

  node_stack.pop_back();
  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_expression(MnExprStatement n) {
  Err err;

  //n.dump_tree();

  if (n.child(0).sym == sym_call_expression) {
    auto call = n.child(0);
    if ( call.get_field(field_function).sym == sym_field_expression) {
      // This node is a block-level call to a submodule - since there's nothing
      // on the LHS to bind the output to, we just comment out the whole call.
      err |= comment_out(call);

      // There should only be a semicolon after the call...
      for (int i = 1; i < n.child_count(); i++) {
        err |= emit_dispatch(n.child(i));
      }
      return err;
    }
  }

  // Other calls get translated.
  err |= emit_children(n);

  return err;
}

//------------------------------------------------------------------------------
// Change "{ blah(); foo(); int x = 1; }" to "begin blah(); ... end"

CHECK_RETURN Err MtCursor::emit_compound(MnCompoundStatement n) {
  Err err;

  assert(cursor == n.start());
  node_stack.push_back(n);

  push_indent(n);

  auto body_count = n.child_count();
  for (int i = 0; i < body_count; i++) {
    auto c = n.child(i);

    err |= emit_submod_input_port_bindings(c);

    switch (c.sym) {
      case anon_sym_LBRACE:
        err |= emit_replacement(c, "begin");
        err |= emit_ws_to_newline();
        err |= emit_hoisted_decls(n);
        err |= emit_ws();
        break;
      case sym_declaration:
        err |= emit_init_declarator_as_assign(c);
        break;
      case anon_sym_RBRACE:
        err |= emit_replacement(c, "end");
        break;
      default:
        err |= emit_dispatch(c);
        break;
    }
    if (i != body_count - 1) {
      err |= emit_ws();
    }
  }

  pop_indent(n);

  node_stack.pop_back();
  err |= cursor != n.end();
  return err;
}

//------------------------------------------------------------------------------
// Change logic<N> to logic[N-1:0]

CHECK_RETURN Err MtCursor::emit_template_type(MnTemplateType n) {
  Err err;

  assert(cursor == n.start());

  err |= emit_type_id(n.name());
  err |= emit_ws();

  auto args = n.args();

  bool is_logic = n.name().match("logic");
  if (is_logic) {
    auto logic_size = args.first_named_child();
    switch (logic_size.sym) {
      case sym_number_literal: {
        int width = atoi(logic_size.start());
        if (width > 1) {
          err |= emit_replacement(args, "[%d:0]", width - 1);
        }
        cursor = args.end();
        break;
      }
      default:
        cursor = logic_size.start();
        err |= emit_printf("[");
        err |= emit_dispatch(logic_size);
        err |= emit_printf("-1:0]");
        break;
    }
  } else {
    err |= emit_template_arg_list(args);
  }

  cursor = n.end();
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Change <param, param> to #(param, param)

CHECK_RETURN Err MtCursor::emit_template_arg_list(MnTemplateArgList n) {
  Err err;
  assert(cursor == n.start());

  auto child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    auto c = n.child(i);
    switch (c.sym) {
      case anon_sym_LT:
        err |= emit_replacement(c, " #(");
        break;
      case anon_sym_GT:
        err |= emit_replacement(c, ")");
        break;
      default:
        err |= emit_dispatch(c);
        break;
    }
    if (i != child_count - 1) {
      err |= emit_ws();
    }
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Enum lists do _not_ turn braces into begin/end.

CHECK_RETURN Err MtCursor::emit_enum_list(MnEnumeratorList n) {
  Err err;

  for (const auto& c : (MnNode&)n) {
    switch (c.sym) {
      case anon_sym_LBRACE:
        err |= emit_text(c);
        break;
      case anon_sym_RBRACE:
        err |= emit_text(c);
        break;
      default:
        err |= emit_dispatch(c);
        break;
    }
    err |= emit_ws();
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Discard any trailing semicolons in the translation unit.

CHECK_RETURN Err MtCursor::emit_translation_unit(MnTranslationUnit n) {
  Err err;
  assert(cursor == n.start());

  node_stack.push_back(n);
  auto child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    auto c = n.child(i);
    switch (c.sym) {
      case anon_sym_SEMI:
        err |= skip_over(c);
        break;
      default:
        err |= emit_dispatch(c);
        break;
    }
    if (err) return err;
    if (i != child_count - 1) {
      err |= emit_ws();
    }
  }
  node_stack.pop_back();

  if (cursor < source_file->source_end) {
    err |= emit_span(cursor, source_file->source_end);
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Replace "0x" prefixes with "'h"
// Replace "0b" prefixes with "'b"
// Add an explicit size prefix if needed.

CHECK_RETURN Err MtCursor::emit_number_literal(MnNumberLiteral n, int size_cast) {
  Err err;
  assert(cursor == n.start());

  assert(!override_size || !size_cast);
  if (override_size) size_cast = override_size;

  std::string body = n.text();

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
    err |= emit_printf("%d'h", size_cast);
  } else if (body.starts_with("0b")) {
    prefix_count = 2;
    if (!size_cast) size_cast = (int)body.size() - 2 - spacer_count;
    err |= emit_printf("%d'b", size_cast);
  } else {
    if (size_cast) {
      err |= emit_printf("%d'd", size_cast);
    }
  }

  if (spacer_count) {
    err |= emit_printf(body.c_str() + prefix_count);
  } else {
    err |= emit_span(n.start() + prefix_count, n.end());
  }

  cursor = n.end();
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Change "return x" to "(funcname) = x" to match old Verilog return style.

// FIXME FIXME FIXME WE NEED TO CHECK THAT THE RETURN DOESN'T EARLY OUT TO
// MATCH VERILOG SEMANTICS

CHECK_RETURN Err MtCursor::emit_return(MnReturnStatement n) {
  Err err;
  assert(cursor == n.start());

  auto node_lit = n.child(0);
  auto node_expr = n.child(1);

  /*
  if (current_method->is_tock) {
    printf("RETURNS IN TOCKS ARE BROKEN DO NOT USE");
    exit(-1);
  }

  if (current_method->is_tick) {
    printf("RETURNS IN TICKS ARE BROKEN DO NOT USE");
    exit(-1);
  }
  */

  cursor = node_expr.start();
  err |= emit_printf("%s = ", current_method->name().c_str());
  err |= emit_dispatch(node_expr);
  err |= emit_printf(";");

  // FIXME We can only emit a 'return' if we're in a task or function.
  /*
  emit_newline();
  emit_indent();
  emit("return;");
  */
  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// FIXME translate types here

CHECK_RETURN Err MtCursor::emit_data_type(MnDataType n) {
  Err err;
  assert(cursor == n.start());
  err |= emit_text(n);
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// FIXME translate types here

CHECK_RETURN Err MtCursor::emit_identifier(MnIdentifier n) {
  Err err;
  assert(cursor == n.start());

  auto name = n.name4();
  auto it = id_replacements.find(name);
  if (it != id_replacements.end()) {
    err |= emit_replacement(n, it->second.c_str());
  } else {
    if (preproc_vars.contains(name)) {
      err |= emit_printf("`");
    }
    err |= emit_text(n);
  }
  assert(cursor == n.end());
  return err;
}

CHECK_RETURN Err MtCursor::emit_type_id(MnTypeIdentifier n) {
  Err err;
  assert(cursor == n.start());

  auto name = n.name4();
  auto it = id_replacements.find(name);
  if (it != id_replacements.end()) {
    err |= emit_replacement(n, it->second.c_str());
  } else {
    err |= emit_text(n);
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// For some reason the class's trailing semicolon ends up with the template
// field_decl, so we prune it here.

CHECK_RETURN Err MtCursor::emit_template_decl(MnTemplateDecl n) {
  Err err;

  assert(cursor == n.start());

  MnClassSpecifier class_specifier;
  MnTemplateParamList param_list;

  for (int i = 0; i < n.child_count(); i++) {
    auto child = n.child(i);

    if (child.sym == sym_template_parameter_list) {
      param_list = MnTemplateParamList(child);
    }

    if (child.sym == sym_class_specifier) {
      class_specifier = MnClassSpecifier(child);
    }
  }

  assert(!class_specifier.is_null());
  std::string class_name = class_specifier.get_field(field_name).text();

  /*
  if (!in_module_or_package) {
    assert(!current_mod);
    for (auto mod : source_file->modules) {
      if (mod->mod_name == struct_name) {
        current_mod = mod;
        break;
      }
    }
    assert(current_mod);
  }
  */

  auto old_mod = current_mod;
  current_mod = source_file->get_module(class_name);

  cursor = class_specifier.start();
  err |= emit_class(class_specifier);
  cursor = n.end();

  current_mod = old_mod;
  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz, so that a field expression instead
// refers to a glue expression.

CHECK_RETURN Err MtCursor::emit_field_expr(MnFieldExpr n) {
  Err err;
  assert(cursor == n.start());

  auto field = n.text();
  for (auto& c : field) {
    if (c == '.') c = '_';
  }
  err |= emit_replacement(n, field.c_str());
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_case_statement(MnCaseStatement n) {
  Err err;

  assert(cursor == n.start());
  node_stack.push_back(n);
  auto child_count = n.child_count();

  // FIXME checking for a break at the top level of the case isn't going to be
  // robust enough...

  // bool empty_case = false;
  // bool break= false;

  for (int i = 0; i < child_count; i++) {
    auto c = n.child(i);
    if (c.sym == sym_break_statement) {
      // break_statement
      // got_break = true;
      err |= comment_out(c);
    } else if (c.sym == anon_sym_case) {
      err |= comment_out(c);
    } else {
      if (c.sym == anon_sym_COLON) {
        // If there's nothing after the colon, emit a comma.
        if (i == child_count - 1) {
          // empty_case = true;
          err |= emit_replacement(c, ",");
        } else {
          err |= emit_text(c);
        }
      } else {
        err |= emit_dispatch(c);
      }
    }
    if (i != child_count - 1) {
      err |= emit_ws();
    }
  }

  /*
  if (!got_break && !empty_case) {
    LOG_R("Found non-empty case without break
    error = true;
  }
  */

  node_stack.pop_back();
  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_switch(MnSwitchStatement n) {
  Err err;
  assert(cursor == n.start());

  auto child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    auto c = n.child(i);
    if (c.sym == anon_sym_switch) {
      err |= emit_replacement(c, "case");
    } else if (c.field == field_body) {
      auto gc_count = c.child_count();
      for (int j = 0; j < gc_count; j++) {
        auto gc = c.child(j);
        if (gc.sym == anon_sym_LBRACE) {
          err |= skip_over(gc);
        } else if (gc.sym == anon_sym_RBRACE) {
          err |= emit_replacement(gc, "endcase");
        } else {
          err |= emit_dispatch(gc);
        }
        if (j != gc_count - 1) {
          err |= emit_ws();
        }
      }

    } else {
      err |= emit_dispatch(c);
    }

    if (i != child_count - 1) {
      err |= emit_ws();
    }
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Unwrap magic /*#foo#*/ comments to pass arbitrary text to Verilog.

CHECK_RETURN Err MtCursor::emit_comment(MnComment n) {
  Err err;
  assert(cursor == n.start());

  auto body = n.text();
  if (body.starts_with("/*#") && body.ends_with("#*/")) {
    body.erase(body.size() - 3, 3);
    body.erase(0, 3);
    err |= emit_replacement(n, body.c_str());
  } else {
    err |= emit_text(n);
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Verilog doesn't use "break"

CHECK_RETURN Err MtCursor::emit_break(MnBreakStatement n) {
  Err err;
  assert(cursor == n.start());
  err |= comment_out(n);
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

//CHECK_RETURN Err MtCursor::emit_field_decl_list(MnFieldDeclList n) {
//  Err error;
//  assert(cursor == n.start());
//  error |= emit_children(n);
//  assert(cursor == n.end());
//  return error;
//}

//------------------------------------------------------------------------------
// TreeSitter nodes slightly broken for "a = b ? c : d;"...

CHECK_RETURN Err MtCursor::emit_condition(MnCondExpr n) {
  Err err;
  assert(cursor == n.start());
  err |= emit_children(n);
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Static variables become localparams at module level.

CHECK_RETURN Err MtCursor::emit_storage_spec(MnStorageSpec n) {
  Err err;
  assert(cursor == n.start());
  if (n.match("static")) {
    err |= emit_replacement(n, "localparam");
  } else {
    err |= comment_out(n);
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// ...er, this was something about namespace resolution?
// so this is chopping off the std:: in std::string...

CHECK_RETURN Err MtCursor::emit_qualified_id(MnQualifiedId n) {
  Err err;
  assert(cursor == n.start());

  if (n.text() == "std::string") {
    err |= emit_replacement(n, "string");
  }
  else {
    err |= emit_children(n);
  }

  /*
  if (trim_namespaces) {
    // Chop "enum::" off off "enum::enum_value"
    auto last_child = n.child(n.child_count() - 1);
    cursor = last_child.start();
    err |= emit_dispatch(last_child);
    cursor = n.end();
  } else {
    err |= emit_children(n);
  }
  */

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

bool MtCursor::branch_contains_submod_call(MnNode n) {
  if (n.sym == sym_call_expression) {
    if (n.get_field(field_function).sym == sym_field_expression) {
      return true;
    }
  }

  for (const auto& c : n) {
    if (branch_contains_submod_call(c)) return true;
  }

  return false;
}

//------------------------------------------------------------------------------
// If statements _must_ use {}, otherwise we have no place to insert submodule
// bindings if the branch contains a submod call.

CHECK_RETURN Err MtCursor::emit_if_statement(MnIfStatement n) {
  Err err;

  //n.dump_tree();

  auto node_then = n.get_field(field_consequence);
  auto node_else = n.get_field(field_alternative);

  if (!node_then.is_null() && node_then.sym != sym_compound_statement) {

    if (branch_contains_submod_call(node_then)) {
      LOG_R("If statements that contain submod calls must use {}.\n");
      err |= true;
      return err;
    }
  }

  if (!node_else.is_null() && node_else.sym != sym_compound_statement) {
    if (branch_contains_submod_call(node_then)) {
      LOG_R("Else statements that contain submod calls must use {}.\n");
      err |= true;
      return err;
    }
  }

  assert(cursor == n.start());
  err |= emit_children(n);
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Enums are broken.

CHECK_RETURN Err MtCursor::emit_enum_specifier(MnEnumSpecifier n) {
  Err err;
  assert(cursor == n.start());
  // err |= emit_sym_field_declaration_as_enum_class(MtFieldDecl(n));
  // for (auto c : n) err |= emit_dispatch(c);
  debugbreak();
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_using_decl(MnUsingDecl n) {
  Err err;
  assert(cursor == n.start());
  auto name = n.child(2).text();
  err |= emit_replacement(n, "import %s::*;", name.c_str());
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// FIXME - When do we hit this? It doesn't look finished.
// It's for namespace decls

// FIXME need to handle const char* string declarations

/*
========== tree dump begin
[00:000:187] declaration =
[00:000:226] |  storage_class_specifier =
[00:000:064] |  |  lit = "static"
[01:000:227] |  type_qualifier =
[00:000:068] |  |  lit = "const"
[02:032:078] |  type: primitive_type = "char"
[03:009:224] |  declarator: init_declarator =
[00:009:212] |  |  declarator: pointer_declarator =
[00:000:023] |  |  |  lit = "*"
[01:009:001] |  |  |  declarator: identifier = "TEXT_HEX"
[01:000:063] |  |  lit = "="
[02:034:278] |  |  value: string_literal =
[00:000:123] |  |  |  lit = "\""
[01:000:123] |  |  |  lit = "\""
[04:000:039] |  lit = ";"
========== tree dump end
*/

CHECK_RETURN Err MtCursor::emit_decl(MnDecl n) {
  Err err;
  // n.dump_tree();

  assert(cursor == n.start());

  // Check for "static const char"
  if (n.is_static() && n.is_const()) {
    auto node_type = n.get_field(field_type);
    if (node_type.text() == "char") {
      err |= emit_printf("localparam string ");
      auto init_decl = n.get_field(field_declarator);
      auto pointer_decl = init_decl.get_field(field_declarator);
      auto name = pointer_decl.get_field(field_declarator);
      cursor = name.start();
      err |= emit_text(name);
      err |= emit_printf(" = ");

      auto val = init_decl.get_field(field_value);
      cursor = val.start();
      err |= emit_text(val);
      err |= emit_printf(";");
      cursor = n.end();
      return err;
    }
  }

  // Check for enum classes, which are broken.
  auto node_type = n.get_field(field_type);
  if (node_type.child_count() >= 2 && node_type.child(0).text() == "enum" &&
      node_type.child(1).text() == "class") {
    debugbreak();
    // emit_field_decl_as_enum_class(MtFieldDecl(n));
    return err;
  }

  if (n.child_count() >= 5 && n.child(0).text() == "static" &&
      n.child(1).text() == "const") {
    err |= emit_printf("parameter ");
    cursor = n.child(2).start();
    err |= emit_dispatch(n.child(2));
    err |= emit_ws();
    err |= emit_dispatch(n.child(3));
    err |= emit_ws();
    err |= emit_dispatch(n.child(4));

    cursor = n.end();
    return err;
  }

  // Regular boring local variable declaration?
  for (const auto& c : (MnNode)n) {
    err |= emit_ws();
    err |= emit_dispatch(c);
  }

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// "unsigned int" -> "int unsigned"

CHECK_RETURN Err MtCursor::emit_sized_type_spec(MnSizedTypeSpec n) {
  Err err;
  assert(cursor == n.start());

  assert(n.child_count() == 2);

  cursor = n.child(1).start();
  err |= emit_dispatch(n.child(1));

  err |= emit_span(n.child(0).end(), n.child(1).start());

  cursor = n.child(0).start();
  err |= emit_dispatch(n.child(0));

  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// FIXME - Do we have a test case for namespaces?

CHECK_RETURN Err MtCursor::emit_namespace_def(MnNamespaceDef n) {
  Err err;
  assert(cursor == n.start());

  auto node_name = n.get_field(field_name);
  auto node_body = n.get_field(field_body);

  err |= emit_printf("package %s;", node_name.text().c_str());
  cursor = node_body.start();

  for (const auto& c : node_body) {
    if (c.sym == anon_sym_LBRACE) {
      err |= emit_replacement(c, "");
    } else if (c.sym == anon_sym_RBRACE) {
      err |= emit_replacement(c, "");
    } else {
      err |= emit_dispatch(c);
    }
    err |= emit_ws();
  }

  err |= emit_printf("endpackage");

  err |= emit_indent();
  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// Arg lists are the same in C and Verilog.

CHECK_RETURN Err MtCursor::emit_arg_list(MnArgList n) {
  assert(cursor == n.start());
  return emit_children(n);
}

CHECK_RETURN Err MtCursor::emit_param_list(MnParameterList n) {
  assert(cursor == n.start());
  return emit_children(n);
}

CHECK_RETURN Err MtCursor::emit_field_id(MnFieldIdentifier n) {
  Err err;
  assert(cursor == n.start());
  err |= emit_text(n);
  return err;
}

//------------------------------------------------------------------------------
// FIXME need to do smarter stuff here...

CHECK_RETURN Err MtCursor::emit_preproc(MnNode n) {
  Err err;
  switch (n.sym) {
    case sym_preproc_def: {
      auto lit = n.child(0);
      auto name = n.get_field(field_name);
      auto value = n.get_field(field_value);

      err |= emit_replacement(lit, "`define");
      err |= emit_ws();
      err |= emit_dispatch(name);
      if (!value.is_null()) {
        err |= emit_ws();
        err |= emit_dispatch(value);
      }

      preproc_vars[name.text()] = value;
      break;
    }

    case sym_preproc_if:
      err |= skip_over(n);
      break;

    case sym_preproc_ifdef: {
      err |= emit_children(n);
      break;
    }

    case sym_preproc_else: {
      err |= emit_children(n);
      break;
    }

    case sym_preproc_call:
      err |= skip_over(n);
      break;

    case sym_preproc_arg: {
      err |= emit_text(n);
      break;
    }
    case sym_preproc_include: {
      // FIXME we need to scan the include for types n defines n stuff...
      err |= emit_preproc_include(MnPreprocInclude(n));
      break;
    }
  }
  return err;
}

//------------------------------------------------------------------------------
// Call the correct emit() method based on the node type.

CHECK_RETURN Err MtCursor::emit_dispatch(MnNode n) {
  Err err;

  if (cursor != n.start()) {
    auto s = n.start();

    n.dump_source_lines();
    n.dump_tree();
    assert(cursor == n.start());
  }

  switch (n.sym) {
    case sym_preproc_def:
    case sym_preproc_if:
    case sym_preproc_ifdef:
    case sym_preproc_else:
    case sym_preproc_call:
    case sym_preproc_arg:
    case sym_preproc_include:
      err |= emit_preproc(n);
      break;

    case sym_type_qualifier:
      err |= comment_out(n);
      break;

    case sym_access_specifier:
      in_public = n.child(0).text() == "public";
      err |= comment_out(n);
      break;

    case sym_for_statement:
    case sym_parenthesized_expression:
    case sym_parameter_declaration:
    case sym_optional_parameter_declaration:
    case sym_condition_clause:
    case sym_unary_expression:
    case sym_subscript_expression:
    case sym_enumerator:
    case sym_type_definition:
    case sym_binary_expression:
    case sym_array_declarator:
    case sym_type_descriptor:
    case sym_init_declarator:
    case sym_initializer_list:
    case sym_declaration_list:
    case sym_update_expression:
      err |= emit_children(n);
      break;

    case alias_sym_field_identifier:
      err |= emit_field_id(MnFieldIdentifier(n));
      break;
    case sym_parameter_list:
      err |= emit_param_list(MnParameterList(n));
      break;
    case sym_function_declarator:
      err |= emit_func_decl(MnFuncDeclarator(n));
      break;
    case sym_expression_statement:
      err |= emit_expression(MnExprStatement(n));
      break;
    case sym_argument_list:
      err |= emit_arg_list(MnArgList(n));
      break;
    case sym_enum_specifier:
      err |= emit_enum_specifier(MnEnumSpecifier(n));
      break;
    case sym_if_statement:
      err |= emit_if_statement(MnIfStatement(n));
      break;
    case sym_qualified_identifier:
      err |= emit_qualified_id(MnQualifiedId(n));
      break;
    case sym_storage_class_specifier:
      err |= emit_storage_spec(MnStorageSpec(n));
      break;
    case sym_conditional_expression:
      err |= emit_condition(MnCondExpr(n));
      break;
    //case sym_field_declaration_list:
    //  error |= emit_field_decl_list(MnFieldDeclList(n));
    //  break;
    case sym_break_statement:
      err |= emit_break(MnBreakStatement(n));
      break;
    case sym_identifier:
      err |= emit_identifier(MnIdentifier(n));
      break;
    case sym_class_specifier:
      err |= emit_class(MnClassSpecifier(n));
      break;
    case sym_number_literal:
      err |= emit_number_literal(MnNumberLiteral(n));
      break;
    case sym_field_expression:
      err |= emit_field_expr(MnFieldExpr(n));
      break;
    case sym_return_statement:
      LOG_R("\n");
      LOG_R(
          "Saw a return statement somewhere other than the end of a function "
          "body\n");
      n.dump_source_lines();
      err = true;
      // emit(MnReturnStatement(n));
      err |= skip_over(n);
      break;
    case sym_template_declaration:
      err |= emit_template_decl(MnTemplateDecl(n));
      break;
    case sym_field_declaration:
      err |= emit_field_decl(MnFieldDecl(n));
      break;
    case sym_compound_statement:
      err |= emit_compound(MnCompoundStatement(n));
      break;
    case sym_template_type:
      err |= emit_template_type(MnTemplateType(n));
      break;
    case sym_translation_unit:
      err |= emit_translation_unit(MnTranslationUnit(n));
      break;
    case sym_primitive_type:
      err |= emit_data_type(MnDataType(n));
      break;
    case alias_sym_type_identifier:
      err |= emit_type_id(MnTypeIdentifier(n));
      break;
    case sym_function_definition:
      err |= emit_func_def(MnFuncDefinition(n));
      break;
    case sym_call_expression:
      err |= emit_call(MnCallExpr(n));
      break;
    case sym_assignment_expression:
      err |= emit_assignment(MnAssignmentExpr(n));
      break;
    case sym_template_argument_list:
      err |= emit_template_arg_list(MnTemplateArgList(n));
      break;
    case sym_comment:
      err |= emit_comment(MnComment(n));
      break;
    case sym_enumerator_list:
      err |= emit_enum_list(MnEnumeratorList(n));
      break;
    case sym_case_statement:
      err |= emit_case_statement(MnCaseStatement(n));
      break;
    case sym_switch_statement:
      err |= emit_switch(MnSwitchStatement(n));
      break;
    case sym_using_declaration:
      err |= emit_using_decl(MnUsingDecl(n));
      break;
    case sym_sized_type_specifier:
      err |= emit_sized_type_spec(MnSizedTypeSpec(n));
      break;
    case sym_declaration:
      err |= emit_decl(MnDecl(n));
      break;
    case sym_namespace_definition:
      err |= emit_namespace_def(MnNamespaceDef(n));
      break;

    case alias_sym_namespace_identifier:
      err |= emit_text(n);
      break;

    default:
      static std::set<int> passthru_syms = {
          alias_sym_namespace_identifier, alias_sym_field_identifier,
          sym_sized_type_specifier, sym_string_literal};

      if (!n.is_named()) {
        auto text = n.text();
        if (text == "#ifdef")
          err |= emit_replacement(n, "`ifdef");
        else if (text == "#ifndef")
          err |= emit_replacement(n, "`ifndef");
        else if (text == "#else")
          err |= emit_replacement(n, "`else");
        else if (text == "#endif")
          err |= emit_replacement(n, "`endif");
        else {
          // FIXME TreeSitter bug - we get a anon_sym_SEMI with no text in
          // alu_control.h
          // FIXME TreeSitter - #ifdefs in if/else trees break things
          if (n.start() != n.end()) {
            err |= emit_text(n);
          }
        }
      }

      else if (passthru_syms.contains(n.sym)) {
        err |= emit_text(n);
      } else {
        printf("Don't know what to do with %d %s\n", n.sym, n.ts_node_type());
        n.dump_tree();
        debugbreak();
      }
      break;
  }
  assert(err || cursor == n.end());

  /*
  if (error) {
    n.dump_source_lines();
    n.dump_tree();
    debugbreak();
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_children(MnNode n) {
  Err err;

  assert(cursor == n.start());
  node_stack.push_back(n);
  auto count = n.child_count();
  for (auto i = 0; i < count; i++) {
    err |= emit_dispatch(n.child(i));
    if (i != count - 1) {
      err |= emit_ws();
    }
  }
  node_stack.pop_back();

  return err;
}

//------------------------------------------------------------------------------
