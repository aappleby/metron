#include "MtCursor.h"

#include <stdarg.h>

#include "MtField.h"
#include "MtFuncParam.h"
#include "MtMethod.h"
#include "MtModLibrary.h"
#include "MtModParam.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "Platform.h"
#include "metron_tools.h"

void print_escaped(char s);

/*
CHECK_RETURN Err MtCursor::emit_sym_type_definition(MnNode node) {
  Err err;
  assert(cursor == node.start());
  assert(node.sym == sym_type_definition);

  int child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);
    switch (child.field) {
      default:
        err << emit_default(child);
        break;
    }
    if (err.has_err()) return err;
    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == node.end());
  return err;
}
*/

//------------------------------------------------------------------------------

MtCursor::MtCursor(MtModLibrary* lib, MtSourceFile* source, MtModule* mod,
                   std::string* out)
    : lib(lib), current_source(source), current_mod(mod), str_out(out) {
  indent_stack.push_back("");
  cursor = current_source->source;

  // FIXME preproc_vars should be a set
  preproc_vars["IV_TEST"] = MnNode();
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
  const char* begin = &current_source->source[ts_node_start_byte(n)] - 1;
  const char* end = &current_source->source[ts_node_start_byte(n)];

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

CHECK_RETURN Err MtCursor::emit_newline() { return emit_char('\n'); }

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_indent() {
  Err err;
  for (auto c : indent_stack.back()) {
    err << emit_char(c);
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
      str_out->pop_back();
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
  while (cursor < current_source->source_end && isspace(*cursor)) {
    err << emit_char(*cursor++);
  }
  return err;
}

CHECK_RETURN Err MtCursor::emit_ws_to_newline() {
  Err err;
  while (cursor < current_source->source_end && isspace(*cursor)) {
    auto c = *cursor++;
    err << emit_char(c);
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
    err << ERR("Skipping over null node");
  } else {
    if (echo) {
      LOG_C(0x8080FF, "%s", n.text().c_str());
    }

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
    if (echo) {
      LOG_C(0x8080FF, "^H");
    }
    str_out->pop_back();
  }

  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::comment_out(MnNode n) {
  Err err;
  assert(cursor == n.start());
  err << emit_print("/*");
  err << emit_text(n);
  err << emit_print("*/");
  assert(cursor == n.end());
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_span(const char* a, const char* b) {
  Err err;
  assert(a != b);
  assert(cursor >= current_source->source);
  assert(cursor <= current_source->source_end);
  for (auto c = a; c < b; c++) {
    err << emit_char(*c);
  }
  return err;
}

//----------------------------------------

CHECK_RETURN Err MtCursor::emit_text(MnNode n) {
  Err err;
  assert(cursor == n.start());
  err << emit_span(n.start(), n.end());
  cursor = n.end();
  return err;
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
  Err err;
  assert(cursor == n.start());

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
    err << emit_char(buf[i], 0x80FFFF);
  }
  delete[] buf;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_initializer_list(MnNode node) {
  Err err;
  assert(cursor == node.start());
  assert(node.sym == sym_initializer_list);

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);
    err << emit_ws();
    switch (child.sym) {
      case sym_identifier:
        err << emit_identifier(child);
        break;
      case sym_assignment_expression:
        err << emit_expression(child);
        break;
      default:
        err << emit_default(child);
        break;
    }
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// Replace "#include" with "`include" and ".h" with ".sv"

CHECK_RETURN Err MtCursor::emit_sym_preproc_include(MnPreprocInclude n) {
  Err err;

  assert(cursor == n.start());

  err << emit_replacement(n.child(0), "`include") << emit_ws();

  auto path = n.path_node().text();
  assert(path.ends_with(".h\""));
  path.resize(path.size() - 3);
  path = path + ".sv\"";
  err << emit_print(path.c_str());

  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// Change '=' to '<=' if lhs is a field and we're inside a sequential block.

CHECK_RETURN Err MtCursor::emit_sym_assignment_expression(MnNode node) {
  Err err;
  assert(cursor == node.start());
  assert(node.sym == sym_assignment_expression);

  for (int i = 0; i < node.child_count(); i++) {
    auto child = node.child(i);
    err << emit_ws();
    switch (child.field) {
      case field_left:
      case field_right:
        err << emit_expression(child);
        break;
      default:
        err << emit_default(child);
        break;
    }
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_static_bit_extract(MnCallExpr call,
                                                   int bx_width) {
  Err err;
  assert(cursor == call.start());

  int arg_count = call.args().named_child_count();

  auto arg0 = call.args().named_child(0);
  auto arg1 = call.args().named_child(1);

  if (arg_count == 1) {
    if (arg0.sym == sym_number_literal) {
      // Explicitly sized literal - 8'd10

      cursor = arg0.start();
      err << emit_sym_number_literal(MnNumberLiteral(arg0), bx_width);
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
        err << emit_print("%d'(", bx_width) << emit_expression(arg0)
            << emit_print(")");
        cursor = call.end();
      }

    } else {
      // Size-casting expression
      cursor = arg0.start();
      err << emit_print("%d'(", bx_width) << emit_expression(arg0)
          << emit_print(")");
      cursor = call.end();
    }
  } else if (arg_count == 2) {
    // Slicing logic array - foo[7:2]

    if (arg1.sym == sym_number_literal) {
      // Slice at offset
      if (bx_width == 1) {
        err << emit_replacement(call, "%s[%s]", arg0.text().c_str(),
                                arg1.text().c_str());
      } else {
        int offset = atoi(arg1.start());
        err << emit_replacement(call, "%s[%d:%d]", arg0.text().c_str(),
                                bx_width - 1 + offset, offset);
      }
    } else {
      if (bx_width == 1) {
        err << emit_replacement(call, "%s[%s]", arg0.text().c_str(),
                                arg1.text().c_str());
        ;
      } else {
        err << emit_replacement(call, "%s[%d + %s : %s]", arg0.text().c_str(),
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

CHECK_RETURN Err MtCursor::emit_dynamic_bit_extract(MnCallExpr call,
                                                    MnNode bx_node) {
  Err err;

  assert(cursor == call.start());

  int arg_count = call.args().named_child_count();

  auto arg0 = call.args().named_child(0);
  auto arg1 = call.args().named_child(1);

  if (arg_count == 1) {
    // Non-literal size-casting expression - bits'(expression)
    if (arg0.text() == "DONTCARE") {
      cursor = bx_node.start();
      err << emit_expression(bx_node);
      err << emit_print("'(1'bx)");
      cursor = call.end();
    } else {
      cursor = bx_node.start();
      err << emit_print("(");
      err << emit_expression(bx_node);
      err << emit_print(")");
      err << emit_print("'(");
      cursor = arg0.start();
      err << emit_expression(arg0);
      err << emit_print(")");
      cursor = call.end();
    }

  } else if (arg_count == 2) {
    // Non-literal slice expression - expression[bits+offset-1:offset];

    cursor = arg0.start();
    err << emit_expression(arg0);

    if (arg1.sym != sym_number_literal) debugbreak();
    int offset = atoi(arg1.start());

    err << emit_print("[%s+%d:%d]", bx_node.text().c_str(), offset - 1, offset);
    cursor = call.end();
  } else {
    debugbreak();
  }

  return err;
}

//------------------------------------------------------------------------------
// Replace function names with macro names where needed, comment out explicit
// init/tick/tock calls.

CHECK_RETURN Err MtCursor::emit_sym_call_expression(MnCallExpr n) {
  Err err;

  MnFunc func = n.func();
  MnArgList args = n.args();

  // If we're calling a member function, look at the name of the member
  // function and not the whole foo.bar().

  std::string func_name = func.name();

  auto method = current_mod->get_method(func_name);

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

  assert(cursor == n.start());

  if (func_name == "coerce") {
    // Convert to cast? We probably shouldn't be calling coerce() directly.
    debugbreak();

  } else if (func_name == "sra") {
    auto lhs = args.named_child(0);
    auto rhs = args.named_child(1);

    err << emit_print("($signed(");
    cursor = lhs.start();
    err << emit_expression(lhs);
    err << emit_print(") >>> ");
    cursor = rhs.start();
    err << emit_expression(rhs);
    err << emit_print(")");
    cursor = n.end();

  } else if (func_name == "signed") {
    err << emit_replacement(func, "$signed");
    err << emit_sym_argument_list(args);
  } else if (func_name == "unsigned") {
    err << emit_replacement(func, "$unsigned");
    err << emit_sym_argument_list(args);
  } else if (func_name == "clog2") {
    err << emit_replacement(func, "$clog2");
    err << emit_sym_argument_list(args);
  } else if (func_name == "pow2") {
    err << emit_replacement(func, "2**");
    err << emit_sym_argument_list(args);
  } else if (func_name == "readmemh") {
    err << emit_replacement(func, "$readmemh");
    err << emit_sym_argument_list(args);
  } else if (func_name == "value_plusargs") {
    err << emit_replacement(func, "$value$plusargs");
    err << emit_sym_argument_list(args);
  } else if (func_name == "write") {
    err << emit_replacement(func, "$write");
    err << emit_sym_argument_list(args);
  } else if (func_name == "sign_extend") {
    err << emit_replacement(func, "$signed");
    err << emit_sym_argument_list(args);
  } else if (method && method->in_init) {
    err << comment_out(n);
  } else if (method && method->in_tick) {
    // Local call to tick() we already bound args, comment out the call.
    err << comment_out(n);

  } else if (method && method->in_tock) {
    // Local call to private tock() - bind output if needed.
    auto method = current_mod->get_method(func_name);
    if (method->has_return()) {
      err << emit_replacement(n, "%s", func_name.c_str());
    } else {
      err << comment_out(n);
    }

  } else if (func_name == "bx") {
    // Bit extract.
    auto template_arg = func.as_templ().args().named_child(0);
    err << emit_dynamic_bit_extract(n, template_arg);
  } else if (func_name == "cat") {
    // Remove "cat" and replace parens with brackets
    err << skip_over(func);
    auto child_count = args.child_count();
    // for (const auto& arg : (MtNode&)args) {
    for (int i = 0; i < child_count; i++) {
      const auto& arg = args.child(i);
      switch (arg.sym) {
        case anon_sym_LPAREN: {
          err << emit_replacement(arg, "{");
          break;
        }
        case anon_sym_RPAREN: {
          err << emit_replacement(arg, "}");
          break;
        }
        default:
          err << emit_expression(arg);
          break;
      }
      if (i < child_count - 1) {
        err << emit_ws();
      }
    }
  } else if (func_name == "dup") {
    // Convert "dup<15>(x)" to "{15 {x}}"

    assert(args.named_child_count() == 1);

    err << skip_over(func);

    auto template_arg = func.as_templ().args().named_child(0);
    int dup_count = atoi(template_arg.start());
    err << emit_print("{%d ", dup_count);
    err << emit_print("{");

    auto func_arg = args.named_child(0);
    cursor = func_arg.start();
    err << emit_expression(func_arg);

    err << emit_print("}}");
    cursor = n.end();
  } else if (func.sym == sym_field_expression) {
    // Component method call - just translate the field expression, not the
    // args.
    err << emit_expression(func);
    cursor = n.end();
  } else if (method && method->in_func) {
    // Local call to private func() - bind output if needed.
    auto method = current_mod->get_method(func_name);
    if (method->has_return()) {
      err << emit_replacement(n, "%s", func_name.c_str());
    } else {
      err << comment_out(n);
    }
  } else {
    // All other function/task calls go through normally.
    n.error();
    err << emit_expression(n);
  }

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Replace "logic blah = x;" with "logic blah;"

CHECK_RETURN Err MtCursor::emit_init_declarator_as_decl(MnDecl n) {
  Err err;
  assert(cursor == n.start());

  err << emit_type(n.get_field(field_type));
  err << emit_ws();
  err << emit_identifier(
      n.get_field(field_declarator).get_field(field_declarator));
  err << prune_trailing_ws();
  err << emit_print(";");

  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// Replace "logic blah = x;" with "blah = x;"

CHECK_RETURN Err MtCursor::emit_init_declarator_as_assign(MnDecl n) {
  Err err;

  assert(cursor == n.start());

  // We don't need to emit anything for decls without initialization values.
  if (!n.is_init_decl()) {
    // err << comment_out(n);
    err << skip_over(n);
    return err;
  }

  assert(n.is_init_decl());
  cursor = n._init_decl().start();
  err << emit_declarator(n._init_decl());
  err << prune_trailing_ws();
  err << emit_print(";");
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
      bool is_localparam = c.sym == sym_declaration && c.is_const();

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
      bool is_localparam = c.sym == sym_declaration && c.is_const();

      if (is_localparam) {
      } else {
        cursor = c.start();

        auto d = MnDecl(c);

        if (d.is_init_decl()) {
          err << emit_indent();
          err << emit_init_declarator_as_decl(d);
          err << emit_newline();
        } else {
          err << emit_indent();
          err << emit_sym_declaration(d);
          err << emit_newline();
        }
      }
    }
  }
  *this = old_cursor;

  err << emit_newline();

  assert(at_newline);
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_input_port_bindings(MnNode n) {
  Err err;
  auto old_cursor = cursor;

  // Emit bindings for child nodes first, but do _not_ recurse into compound
  // blocks.

  for (int i = 0; i < n.child_count(); i++) {
    auto c = n.child(i);
    if (n.sym != sym_compound_statement) {
      err << emit_input_port_bindings(c);
    }
  }

  // OK, now we can emit bindings for the call we're at.

  if (n.sym == sym_call_expression) {
    auto old_replacements = id_replacements;
    for (auto p : current_method->param_nodes) {
      auto param_name = p.get_field(field_declarator).text();
      id_replacements[param_name] = current_method->name() + "_" + param_name;
    }

    auto func_node = n.get_field(field_function);
    auto args_node = n.get_field(field_arguments);

    if (func_node.sym == sym_field_expression) {
      if (args_node.named_child_count() != 0) {
        auto inst_id = func_node.get_field(field_argument);
        auto meth_id = func_node.get_field(field_field);

        auto component = current_mod->get_component(inst_id.text());
        assert(component);

        auto component_mod = lib->get_module(component->type_name());

        auto component_method = component_mod->get_method(meth_id.text());
        assert(component_method);

        for (int i = 0; i < component_method->param_nodes.size(); i++) {
          auto& param = component_method->param_nodes[i];

          err << emit_print("%s_%s_%s = ", inst_id.text().c_str(),
                            component_method->name().c_str(),
                            param.get_field(field_declarator).text().c_str());

          auto arg_node = args_node.named_child(i);
          cursor = arg_node.start();
          err << emit_expression(arg_node);
          cursor = arg_node.end();

          err << prune_trailing_ws();
          err << emit_print(";");
          err << emit_newline();
          err << emit_indent();
        }
      }
    } else if (func_node.sym == sym_identifier) {
      auto method = current_mod->get_method(func_node.text().c_str());

      if (method && method->in_tick) {
        for (int i = 0; i < method->param_nodes.size(); i++) {
          auto& param = method->param_nodes[i];
          auto param_name = param.get_field(field_declarator).text();

          err << emit_print("%s_%s = ", func_node.text().c_str(),
                            param_name.c_str());

          auto arg_node = args_node.named_child(i);
          cursor = arg_node.start();
          err << emit_expression(arg_node);
          cursor = arg_node.end();

          err << prune_trailing_ws();
          err << emit_print(";");
          err << emit_newline();
          err << emit_indent();
        }
      }
    }

    id_replacements = old_replacements;
  }

  cursor = old_cursor;
  return err;
}

//------------------------------------------------------------------------------
// Change "init/tick/tock" to "initial begin / always_comb / always_ff", change
// void methods to tasks, and change const methods to funcs.

// func_def = { field_type, field_declarator, field_body }

CHECK_RETURN Err MtCursor::emit_sym_function_definition(MnFuncDefinition n) {
  Err err;

  assert(cursor == n.start());

  auto return_type = n.get_field(field_type);
  auto func_decl = n.decl();

  current_method = current_mod->get_method(n.name4());
  assert(current_method);

  auto old_replacements = id_replacements;

  for (auto p : current_method->param_nodes) {
    id_replacements[p.text()] = current_method->name() + "_" + p.text();
  }

  //----------
  // Local function input port decls go _before_ the function definition.

  if (!current_method->is_public() &&
      (current_method->in_tick || current_method->in_tock)) {
    for (auto n : current_method->param_nodes) {
      auto param_type = n.get_field(field_type);
      auto param_name = n.get_field(field_declarator);

      MtCursor subcursor(lib, current_source, current_mod, str_out);
      subcursor.echo = echo;
      subcursor.in_ports = true;

      subcursor.cursor = param_type.start();
      err << subcursor.emit_type(param_type);
      err << subcursor.emit_ws();

      err << emit_print("%s_", current_method->name().c_str());

      subcursor.cursor = param_name.start();
      // err << subcursor.emit_declaration(param_name);
      err << subcursor.emit_identifier(param_name);

      err << prune_trailing_ws();
      err << emit_print(";");
      err << emit_newline();
      err << emit_indent();
    }

    if (current_method->has_return()) {
      auto return_type = n.get_field(field_type);

      MtCursor subcursor(lib, current_source, current_mod, str_out);
      subcursor.echo = echo;
      subcursor.in_ports = true;

      subcursor.cursor = return_type.start();
      err << subcursor.emit_type(return_type);
      err << subcursor.emit_ws();

      err << emit_print("%s", current_method->name().c_str());

      err << prune_trailing_ws();
      err << emit_print(";");
      err << emit_newline();
      err << emit_indent();
    }
  }

  //----------
  // Emit a block declaration for the type of function we're in.

  if (current_method->in_init) {
    if (!return_type.is_null()) err << skip_over(return_type);
    err << skip_ws();
    err << emit_replacement(func_decl, "initial");
  } else if (current_method->in_tick) {
    err << skip_over(return_type);
    err << skip_ws();
    err << emit_replacement(func_decl, "always_ff @(posedge clock)");
  } else if (current_method->in_tock) {
    err << skip_over(return_type);
    err << skip_ws();
    err << emit_replacement(func_decl, "always_comb");
  } /* else if (current_method->in_task) {
     err << skip_over(return_type);
     err << skip_ws();

     // FIXME do we want all tasks to be automatic? That would be closer to C
     // semantics...
     // err << emit_print("task automatic ");
     err << emit_print("task ");
     err << emit_dispatch(func_decl);
     err << prune_trailing_ws();
     err << emit_print(";");
   }*/
  else if (current_method->in_func) {
    err << emit_print("function ");
    err << emit_type(return_type);
    err << emit_ws();
    err << emit_declarator(func_decl);
    err << prune_trailing_ws();
    err << emit_print(";");
  } else {
    debugbreak();
  }

  err << emit_ws();

  //----------
  // Emit the function body.

  std::string delim_begin = "begin";
  std::string delim_end = "end";

  if (current_method->in_init)
    delim_begin = str_printf("begin /*%s*/", current_method->name().c_str());
  else if (current_method->in_tick) {
    delim_begin = str_printf("begin /*%s*/", current_method->name().c_str());
  } else if (current_method->in_tock)
    delim_begin = str_printf("begin /*%s*/", current_method->name().c_str());
  else if (current_method->in_func) {
    delim_begin = "";
    delim_end = "endfuction";
  } else {
    debugbreak();
  }

  auto func_body = n.body();
  err << emit_sym_compound_statement(func_body, delim_begin, delim_end);

  //----------

  assert(cursor == n.end());

  // Emit return value binding
  if (current_method->in_func && current_method->is_public() &&
      current_method->has_return()) {
    err << emit_newline();
    err << emit_indent();

    err << emit_print("always_comb %s_ret = %s(", current_method->cname(),
                      current_method->cname());

    int param_count = current_method->param_nodes.size();
    for (int i = 0; i < param_count; i++) {
      auto param = current_method->param_nodes[i];
      err << emit_print("%s_%s", current_method->cname(),
                        param.get_field(field_declarator).text().c_str());
      if (i < param_count - 1) {
        err << emit_print(", ");
      }
    }
    err << emit_print(");");
  }

  //----------

  current_method = nullptr;

  id_replacements.swap(old_replacements);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_field_as_component(MnFieldDecl n) {
  Err err;
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

  cursor = node_type.start();
  err << emit_type(node_type);
  err << emit_ws();
  err << emit_identifier(node_decl);
  err << emit_ws();

  err << emit_print("(");

  indent_stack.push_back(indent_stack.back() + "  ");

  err << emit_newline();
  err << emit_indent();
  err << emit_print("// Inputs");

  err << emit_newline();
  err << emit_indent();
  err << emit_print(".clock(clock)");

  int port_count = int(component_mod->input_signals.size() +
                       component_mod->input_method_params.size() +
                       component_mod->output_signals.size() +
                       component_mod->output_registers.size() +
                       component_mod->output_method_returns.size());

  if (port_count) {
    err << emit_print(",");
  }

  int port_index = 0;

  for (auto n : component_mod->input_signals) {
    auto key = inst_name + "." + n->name();

    err << emit_newline();
    err << emit_indent();
    err << emit_print(".%s(%s_%s)", n->cname(), inst_name.c_str(), n->cname());

    if (port_index++ < port_count - 1) {
      err << emit_print(", ");
    }
  }

  for (auto n : component_mod->input_method_params) {
    auto key = inst_name + "." + n->name();

    n->dump();

    err << emit_newline();
    err << emit_indent();
    err << emit_print(".%s_%s(%s_%s_%s)", n->func_name.c_str(), n->cname(),
                      inst_name.c_str(), n->func_name.c_str(), n->cname());

    if (port_index++ < port_count - 1) {
      err << emit_print(", ");
    }
  }

  err << emit_newline();
  err << emit_indent();
  err << emit_print("// Outputs");

  for (auto n : component_mod->output_signals) {
    err << emit_newline();
    err << emit_indent();
    err << emit_print(".%s(%s_%s)", n->cname(), inst_name.c_str(), n->cname());

    if (port_index++ < port_count - 1) {
      err << emit_print(", ");
    }
  }

  for (auto n : component_mod->output_registers) {
    err << emit_newline();
    err << emit_indent();
    err << emit_print(".%s(%s_%s)", n->cname(), inst_name.c_str(), n->cname());

    if (port_index++ < port_count - 1) {
      err << emit_print(", ");
    }
  }

  for (auto n : component_mod->output_method_returns) {
    err << emit_newline();
    err << emit_indent();
    err << emit_print(".%s_ret(%s_%s_ret)", n->name().c_str(),
                      inst_name.c_str(), n->cname());

    if (port_index++ < port_count - 1) {
      err << emit_print(", ");
    }
  }

  indent_stack.pop_back();

  err << emit_newline();
  err << emit_indent();
  err << emit_print(")");
  err << emit_text(node_semi);
  err << emit_newline();

  err << emit_port_decls(n);

  cursor = n.end();

  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_port_decls(MnFieldDecl component_decl) {
  Err err;

  if (current_mod->components.empty()) {
    return err;
  }

  assert(at_newline);

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

    MtCursor subcursor(lib, component_mod->source_file, component_mod, str_out);
    subcursor.echo = echo;
    subcursor.in_ports = true;
    subcursor.id_replacements = replacements;
    subcursor.cursor = output_type.start();

    err << emit_indent();
    err << subcursor.emit_type(output_type);
    err << subcursor.emit_ws();
    err << emit_print("%s_", component_cname);
    err << subcursor.emit_identifier(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");
    err << emit_newline();
  }

  for (auto n : component_mod->input_method_params) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    MtCursor subcursor(lib, component_mod->source_file, component_mod, str_out);
    subcursor.echo = echo;
    subcursor.in_ports = true;
    subcursor.id_replacements = replacements;
    subcursor.cursor = output_type.start();

    err << emit_indent();
    err << subcursor.emit_type(output_type);
    err << subcursor.emit_ws();
    err << emit_print("%s_%s_", component_cname, n->func_name.c_str());
    err << subcursor.emit_identifier(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");
    err << emit_newline();
  }

  for (auto n : component_mod->output_signals) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    MtCursor subcursor(lib, component_mod->source_file, component_mod, str_out);
    subcursor.echo = echo;
    subcursor.in_ports = true;
    subcursor.id_replacements = replacements;
    subcursor.cursor = output_type.start();

    err << emit_indent();
    err << subcursor.emit_type(output_type);
    err << subcursor.emit_ws();
    err << emit_print("%s_", component_cname);
    err << subcursor.emit_identifier(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");
    err << emit_newline();
  }

  for (auto n : component_mod->output_registers) {
    // field_declaration
    auto output_type = n->get_type_node();
    auto output_decl = n->get_decl_node();

    MtCursor subcursor(lib, component_mod->source_file, component_mod, str_out);
    subcursor.echo = echo;
    subcursor.in_ports = true;
    subcursor.id_replacements = replacements;
    subcursor.cursor = output_type.start();

    err << emit_indent();
    err << subcursor.emit_type(output_type);
    err << subcursor.emit_ws();
    err << emit_print("%s_", component_cname);
    err << subcursor.emit_identifier(output_decl);
    err << prune_trailing_ws();
    err << emit_print(";");
    err << emit_newline();
  }

  for (auto m : component_mod->output_method_returns) {
    auto getter_type = m->_node.get_field(field_type);
    auto getter_decl = m->_node.get_field(field_declarator);
    auto getter_name = getter_decl.get_field(field_declarator);

    MtCursor sub_cursor(lib, component_mod->source_file, component_mod,
                        str_out);
    sub_cursor.echo = echo;
    sub_cursor.in_ports = true;
    sub_cursor.id_replacements = replacements;

    sub_cursor.cursor = getter_type.start();

    err << emit_indent();
    err << sub_cursor.skip_ws();
    err << sub_cursor.emit_type(getter_type);
    err << sub_cursor.emit_ws();
    err << emit_print("%s_", component_cname);
    err << sub_cursor.emit_identifier(getter_name);
    err << prune_trailing_ws();
    err << emit_print("_ret;");
    err << emit_newline();
  }

  return err;
}

//------------------------------------------------------------------------------
// enum { FOO, BAR } e; => enum { FOO, BAR } e;

/*
enum { FOO, BAR, BAZ } blom;

========== tree dump begin
█ field_declaration =
▒══█ type: enum_specifier =
▒  ▒══■ lit = "enum"
▒  ▒══█ body: enumerator_list =
▒     ▒══■ lit = "{"
▒     ▒══█ enumerator =
▒     ▒  ▒══■ name: identifier = "FOO"
▒     ▒══■ lit = ","
▒     ▒══█ enumerator =
▒     ▒  ▒══■ name: identifier = "BAR"
▒     ▒══■ lit = ","
▒     ▒══█ enumerator =
▒     ▒  ▒══■ name: identifier = "BAZ"
▒     ▒══■ lit = "}"
▒══■ declarator: field_identifier = "blom"
▒══■ lit = ";"
========== tree dump end
*/

CHECK_RETURN Err MtCursor::emit_anonymous_enum(MnFieldDecl n) {
  Err err;
  assert(cursor == n.start());

  int child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    err << emit_ws();
    auto child = n.child(i);
    switch (child.field) {
      case field_type:
        err << emit_type(child);
        break;
      case field_declarator:
        err << emit_declarator(child);
        break;
      default:
        err << emit_default(child);
        break;
    }
  }

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// enum e { FOO, BAR }; => typedef enum { FOO, BAR } e;

/*
enum blom { FOO, BAR, BAZ };

========== tree dump begin
█ field_declaration =
▒══█ type: enum_specifier =
▒  ▒══■ lit = "enum"
▒  ▒══■ name: type_identifier = "blom"
▒══█ default_value: initializer_list =
▒  ▒══■ lit = "{"
▒  ▒══■ identifier = "FOO"
▒  ▒══■ lit = ","
▒  ▒══■ identifier = "BAR"
▒  ▒══■ lit = ","
▒  ▒══■ identifier = "BAZ"
▒  ▒══■ lit = "}"
▒══■ lit = ";"
========== tree dump end
*/

CHECK_RETURN Err MtCursor::emit_simple_enum(MnFieldDecl n) {
  Err err;

  assert(cursor == n.start());

  int child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = n.child(i);
    err << emit_ws();
    switch (child.field) {
      case field_type:
        err << emit_type(child);
        break;
      case field_default_value:
        err << emit_sym_initializer_list(child);
        break;
      default:
        err << emit_default(child);
        break;
    }
  }

  assert(cursor == n.end());
  return err;

  /*
  auto node_type = n.get_field(field_type);
  auto node_name = node_type.get_field(field_name);
  auto node_vals = n.get_field(field_default_value);

  MnNode node_prim_type;

  n.visit_tree([&](MnNode c) {
    if (c.sym == sym_primitive_type) node_prim_type = c;
  });

  err << emit_print("typedef enum ");
  if (node_prim_type) {
    push_cursor(node_prim_type);
    err << emit_type(node_prim_type);
    pop_cursor(node_prim_type);
    err << emit_print(" ");
  }

  push_cursor(node_vals);
  err << emit_sym_initializer_list(node_vals);
  pop_cursor(node_vals);
  err << emit_print(" ");

  push_cursor(node_name);
  err << emit_identifier(node_name);
  pop_cursor(node_name);
  err << emit_print(";");
  cursor = n.end();
  */
}

//------------------------------------------------------------------------------

/*
========== tree dump begin
█ field_declaration =
▒══█ type: enum_specifier =
▒  ▒══■ lit = "enum"
▒  ▒══■ lit = "class"
▒  ▒══■ name: type_identifier = "sized_enum"
▒══█ bitfield_clause =
▒  ▒══■ lit = ":"
▒  ▒══█ compound_literal_expression =
▒     ▒══█ type: qualified_identifier =
▒     ▒  ▒══█ scope: template_type =
▒     ▒  ▒  ▒══■ name: type_identifier = "logic"
▒     ▒  ▒  ▒══█ arguments: template_argument_list =
▒     ▒  ▒     ▒══■ lit = "<"
▒     ▒  ▒     ▒══■ number_literal = "8"
▒     ▒  ▒     ▒══■ lit = ">"
▒     ▒  ▒══■ lit = "::"
▒     ▒  ▒══■ name: type_identifier = "BASE"
▒     ▒══█ value: initializer_list =
▒        ▒══■ lit = "{"
▒        ▒══█ assignment_expression =
▒        ▒  ▒══■ left: identifier = "A8"
▒        ▒  ▒══■ operator: lit = "="
▒        ▒  ▒══■ right: number_literal = "0b01"
▒        ▒══■ lit = ","
▒        ▒══█ assignment_expression =
▒        ▒  ▒══■ left: identifier = "B8"
▒        ▒  ▒══■ operator: lit = "="
▒        ▒  ▒══■ right: number_literal = "0x02"
▒        ▒══■ lit = ","
▒        ▒══█ assignment_expression =
▒        ▒  ▒══■ left: identifier = "C8"
▒        ▒  ▒══■ operator: lit = "="
▒        ▒  ▒══■ right: number_literal = "3"
▒        ▒══■ lit = "}"
▒══■ lit = ";"
========== tree dump end
*/

CHECK_RETURN Err MtCursor::emit_broken_enum(MnFieldDecl n,
                                            MnNode node_bitfield) {
  Err err;

  auto node_type = n.get_field(field_type);
  auto node_name = node_type.get_field(field_name);

  auto node_type2 = node_bitfield.child(1).get_field(field_type);

  node_type2 = node_type2.get_field(field_scope)
                   .get_field(field_arguments)
                   .named_child(0);

  int enum_width = atoi(node_type2.start());

  if (!enum_width) err << ERR("Enum is 0 bits wide?");

  auto node_vals = node_bitfield.child(1).get_field(field_value);

  err << emit_print("typedef enum logic[%d:0] ", enum_width - 1);

  push_cursor(node_vals);
  err << emit_sym_initializer_list(node_vals);
  pop_cursor(node_vals);
  err << emit_print(" ");

  push_cursor(node_name);
  err << emit_identifier(node_name);
  pop_cursor(node_name);
  err << emit_print(";");

  cursor = n.end();

  return err;
}

//------------------------------------------------------------------------------

/*

========== tree dump begin
█ field_declaration =
▒══█ type: enum_specifier =
▒  ▒══■ lit = "enum"
▒  ▒══█ body: enumerator_list =
▒     ▒══■ lit = "{"
▒     ▒══█ enumerator =
▒     ▒  ▒══■ name: identifier = "A3"
▒     ▒══■ lit = ","
▒     ▒══█ enumerator =
▒     ▒  ▒══■ name: identifier = "B3"
▒     ▒══■ lit = ","
▒     ▒══█ enumerator =
▒     ▒  ▒══■ name: identifier = "C3"
▒     ▒══■ lit = "}"
▒══■ declarator: field_identifier = "anon_enum_field1"
▒══■ lit = ";"
========== tree dump end

*/

CHECK_RETURN Err MtCursor::emit_enum(MnFieldDecl n) {
  Err err;

  auto node_type = n.get_field(field_type);
  assert(node_type.sym == sym_enum_specifier);

  // TreeSitter is broken for "enum foo : logic<8>::BASE {}".
  // Work around it by manually extracting the required field
  MnNode node_bitfield;

  n.visit_tree([&](MnNode c) {
    if (c.sym == sym_bitfield_clause) node_bitfield = c;
  });

  if (node_bitfield) {
    err << emit_broken_enum(n, node_bitfield);
  } else if (node_type.get_field(field_name)) {
    err << emit_simple_enum(n);
  } else {
    err << emit_anonymous_enum(n);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_type_definition(MnNode node) {
  Err err;
  assert(cursor == node.start());
  assert(node.sym == sym_type_definition);

  int child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);
    err << emit_ws();
    switch (child.field) {
      case field_type:
        err << emit_type(child);
        break;
      case field_declarator:
        err << emit_declarator(child);
        break;
      default:
        err << emit_default(child);
        break;
    }
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_type(MnNode node) {
  Err err;
  assert(cursor == node.start());

  switch (node.sym) {
    case sym_qualified_identifier:
      // does this belong here?
      err << emit_sym_qualified_identifier(node);
      break;
    case sym_type_descriptor:
      err << emit_type(node.get_field(field_type));
      break;
    case sym_template_type:
      err << emit_sym_template_type(node);
      break;
    case sym_primitive_type:
      err << emit_sym_primitive_type(node);
      break;
    case sym_sized_type_specifier:
      err << emit_sym_sized_type_specifier(node);
      break;
    case alias_sym_type_identifier:
      err << emit_sym_type_identifier(node);
      break;
    case sym_type_definition:
      err << emit_sym_type_definition(node);
      break;
    case sym_struct_specifier:
      err << emit_sym_struct_specifier(node);
      break;
    case sym_template_declaration:
      err << emit_sym_template_declaration(node);
      break;
    case sym_class_specifier:
      err << emit_sym_class_specifier(node);
      break;
    case sym_enum_specifier:
      err << emit_sym_enum_specifier(node);
      break;
    default:
      err << emit_default(node);
      break;
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_identifier(MnNode node) {
  Err err;
  assert(cursor == node.start());

  switch (node.sym) {
    case alias_sym_namespace_identifier:
      err << emit_text(node);
      break;
    case alias_sym_type_identifier:
      err << emit_sym_type_identifier(node);
      break;
    case sym_identifier:
      err << emit_sym_identifier(node);
      break;
    case alias_sym_field_identifier:
      err << emit_sym_field_identifier(node);
      break;
    case sym_qualified_identifier:
      err << emit_sym_qualified_identifier(node);
      break;
    default:
      err << emit_default(node);
      break;
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// FIXME break these out

CHECK_RETURN Err MtCursor::emit_declarator(MnNode node) {
  Err err;
  assert(cursor == node.start());

  switch (node.sym) {
    case alias_sym_type_identifier:
      err << emit_sym_type_identifier(node);
      break;
    case sym_identifier:
    case alias_sym_field_identifier:
      err << emit_identifier(node);
      break;
    case sym_init_declarator:
      err << emit_identifier(node.child(0)) << emit_ws();
      err << emit_text(node.child(1)) << emit_ws();
      err << emit_expression(node.child(2));
      break;
    case sym_array_declarator:
      err << emit_identifier(node.child(0)) << emit_ws();
      err << emit_text(node.child(1)) << emit_ws();
      err << emit_expression(node.child(2)) << emit_ws();
      err << emit_text(node.child(3));
      break;
    case sym_function_declarator:
      err << emit_declarator(node.get_field(field_declarator)) << emit_ws();
      err << emit_sym_parameter_list(node.get_field(field_parameters));
      if (node.child_count() == 3) {
        err << emit_ws();
        // err << emit_sym_type_qualifier(node.child(2));
        err << skip_over(node.child(2));
      }
      break;
    default:
      err << emit_default(node);
      break;
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// FIXME break these out

CHECK_RETURN Err MtCursor::emit_declaration(MnNode node) {
  Err err;
  assert(cursor == node.start());

  switch (node.sym) {
    case sym_optional_parameter_declaration:
      err << emit_type(node.child(0)) << emit_ws();
      err << emit_identifier(node.child(1)) << emit_ws();
      err << emit_text(node.child(2)) << emit_ws();
      err << emit_expression(node.child(3));
      break;
    case sym_parameter_declaration:
      err << emit_type(node.child(0)) << emit_ws();
      err << emit_identifier(node.child(1));
      break;
    default:
      err << emit_default(node);
      break;
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// Emit field declarations. For components, also emit glue declarations and
// append the glue parameter list to the field.

// field_declaration = { type:type_identifier, declarator:field_identifier+ }
// field_declaration = { type:template_type,   declarator:field_identifier+ }
// field_declaration = { type:enum_specifier,  bitfield_clause (TREESITTER BUG)
// }

CHECK_RETURN Err MtCursor::emit_sym_field_decl(MnFieldDecl n) {
  Err err;

  if (n.is_const()) {
    auto node_static = n.child(0);
    auto node_const = n.child(1);
    auto node_type = n.child(2);
    auto node_decl = n.child(3);
    auto node_equals = n.child(4);
    auto node_value = n.child(5);
    auto node_semi = n.child(6);

    // err << emit_sym_storage_class_specifier(node_static) << emit_ws();
    // err << emit_sym_type_qualifier(node_const) << emit_ws();
    err << emit_print("localparam ");
    cursor = node_type.start();
    err << emit_type(node_type) << emit_ws();
    err << emit_identifier(node_decl) << emit_ws();
    err << emit_text(node_equals) << emit_ws();
    err << emit_expression(node_value) << emit_ws();
    err << emit_text(node_semi);
    return err;
  }

  assert(cursor == n.start());

  auto node_type = n.get_field(field_type);
  assert(node_type);

  if (node_type.sym == sym_enum_specifier) {
    return emit_enum(n);
  }

  std::string type_name = n.type5();

  if (current_mod == nullptr) {
    // Struct outside of class

    auto node_type = n.get_field(field_type);
    auto node_decl = n.get_field(field_declarator);
    auto node_semi = n.child(2);

    err << emit_type(node_type) << emit_ws();
    err << emit_declarator(node_decl) << emit_ws();
    err << emit_text(node_semi);
  } else if (lib->get_module(type_name)) {
    err << emit_field_as_component(n);
  } else if (current_mod->get_input_signal(n.name().text())) {
    err << skip_over(n);
  } else if (current_mod->get_output_signal(n.name().text())) {
    err << skip_over(n);
  } else if (current_mod->get_output_register(n.name().text())) {
    err << skip_over(n);
  } else if (n.is_const()) {
    err << emit_child_expressions(n);
  } /*else if (current_mod->get_enum(type_name)) {
    err << emit_child_expressions(n);
  }*/
  else if (type_name == "logic") {
    auto node_type = n.get_field(field_type);
    auto node_decl = n.get_field(field_declarator);
    auto node_semi = n.child(2);

    err << emit_type(node_type) << emit_ws();
    err << emit_declarator(node_decl) << emit_ws();
    err << emit_text(node_semi);
  } else if (type_name == "int") {
    auto node_type = n.get_field(field_type);
    auto node_decl = n.get_field(field_declarator);
    auto node_semi = n.child(2);

    err << emit_type(node_type) << emit_ws();
    err << emit_declarator(node_decl) << emit_ws();
    err << emit_text(node_semi);
  } else {
    debugbreak();
  }
  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_struct_specifier(MnNode n) {
  Err err;

  // FIXME - Do we _have_ to mark it as packed?

  auto node_name = n.get_field(field_name);
  auto node_body = n.get_field(field_body);

  if (node_name) {
    // struct Foo {};

    err << emit_print("typedef ");
    err << emit_text(n.child(0)) << emit_ws();
    err << emit_print("packed ");
    cursor = node_body.start();
    err << emit_sym_field_declaration_list(node_body);
    err << emit_print(" ");

    push_cursor(node_name);
    err << emit_identifier(node_name);
    pop_cursor(node_name);

    err << emit_print(";");
    cursor = n.end();
  } else {
    // typedef struct {} Foo;

    err << emit_text(n.child(0)) << emit_ws();
    err << emit_print("packed ");
    err << emit_sym_field_declaration_list(node_body);
  }

  return err;
}

//------------------------------------------------------------------------------
// Change class/struct to module, add default clk/rst inputs, add input and
// ouptut ports to module param list.

CHECK_RETURN Err MtCursor::emit_sym_class_specifier(MnClassSpecifier n) {
  Err err;

  assert(cursor == n.start());

  auto class_lit = n.child(0);
  auto class_name = n.get_field(field_name);
  auto class_body = n.get_field(field_body);

  //----------

  auto old_mod = current_mod;
  current_mod = current_source->get_module(class_name.text());
  assert(current_mod);

  err << emit_indent();
  err << emit_replacement(class_lit, "module");
  err << emit_ws();
  err << emit_type(class_name);
  err << emit_newline();

  // Patch the template parameter list in after the module declaration, before
  // the port list.

  if (current_mod->mod_param_list) {
    err << emit_indent();
    MtCursor sub_cursor = *this;
    sub_cursor.cursor = current_mod->mod_param_list.start();
    for (const auto& c : (MnNode&)current_mod->mod_param_list) {
      bool handled = false;
      switch (c.sym) {
        case anon_sym_LT:
          err << sub_cursor.emit_replacement(c, "#(");
          err << sub_cursor.emit_ws();
          handled = true;
          break;
        case anon_sym_GT:
          err << sub_cursor.emit_replacement(c, ")");
          err << sub_cursor.skip_ws();
          handled = true;
          break;

        case sym_optional_parameter_declaration:
        case sym_parameter_declaration:
          err << sub_cursor.emit_print("parameter ");
          err << sub_cursor.emit_declaration(c);
          err << sub_cursor.emit_ws();
          handled = true;
          break;

        case anon_sym_COMMA:
          err << sub_cursor.emit_text(c);
          err << sub_cursor.emit_ws();
          handled = true;
          break;
      }
      assert(handled);
    }
    err << emit_newline();
  }

  err << emit_indent();
  err << emit_print("(");
  err << emit_newline();

  {
    // Save the indentation level of the struct body so we can use it in the
    // port list.
    push_indent(class_body);

    in_ports = true;
    trim_namespaces = false;

    int port_count = int(current_mod->input_signals.size() +
                         current_mod->output_signals.size() +
                         current_mod->output_registers.size() +
                         current_mod->input_method_params.size() +
                         current_mod->output_method_returns.size());

    int port_index = 0;

    err << emit_indent();
    err << emit_print("input logic clock");
    if (port_count) {
      err << emit_print(",");
    }
    err << emit_newline();

    // We emit input signals, output signals, and output registers together so
    // that if the source was ported over to Metron from Verilog and has
    // explicit ports, the ports stay in the same order.

    for (auto f : current_mod->all_fields) {
      if (!f->is_public()) continue;
      if (f->is_param()) continue;

      if (!(f->is_public_input() || f->is_public_signal() ||
            f->is_public_register()))
        continue;

      err << emit_indent();

      if (f->is_public_input()) {
        err << emit_print("input ");
      } else if (f->is_public_signal()) {
        err << emit_print("output ");
      } else if (f->is_public_register()) {
        err << emit_print("output ");
      } else {
        debugbreak();
      }

      auto node_type = f->node.get_field(field_type);
      auto node_decl = f->node.get_field(field_declarator);

      MtCursor sub_cursor = *this;
      sub_cursor.cursor = node_type.start();
      err << sub_cursor.emit_type(node_type);
      err << sub_cursor.emit_ws();
      err << sub_cursor.emit_identifier(node_decl);

      if (port_index++ < port_count - 1) {
        err << emit_print(",");
      }
      err << emit_newline();
    }

    for (auto input : current_mod->input_method_params) {
      err << emit_indent();
      err << emit_print("input ");

      auto node_type = input->node.get_field(field_type);
      auto node_decl = input->node.get_field(field_declarator);

      MtCursor sub_cursor = *this;
      sub_cursor.cursor = node_type.start();
      err << sub_cursor.emit_type(node_type);
      err << sub_cursor.emit_ws();
      err << sub_cursor.emit_print("%s_", input->func_name.c_str());
      err << sub_cursor.emit_identifier(node_decl);

      if (port_index++ < port_count - 1) {
        err << emit_print(",");
      }
      err << emit_newline();
    }

    for (auto m : current_mod->output_method_returns) {
      err << emit_indent();
      err << emit_print("output ");

      MtCursor sub_cursor = *this;

      auto getter_type = m->_node.get_field(field_type);
      auto getter_decl = m->_node.get_field(field_declarator);
      auto getter_name = getter_decl.get_field(field_declarator);

      sub_cursor.cursor = getter_type.start();
      err << sub_cursor.emit_type(getter_type);
      err << sub_cursor.emit_ws();
      sub_cursor.cursor = getter_name.start();
      err << sub_cursor.emit_declarator(getter_name);
      err << emit_print("_ret");

      if (port_index++ < port_count - 1) {
        err << emit_print(",");
      }
      err << emit_newline();
    }

    pop_indent(class_body);
    err << emit_indent();
    err << emit_print(");");
    trim_namespaces = true;
    in_ports = false;
  }

  // Whitespace between the end of the port list and the module body.
  err << skip_ws();

  err << emit_sym_field_declaration_list(class_body);

  cursor = n.end();

  current_mod = old_mod;

  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------
// Emit the module body, with a few modifications.
// Discard the opening brace
// Replace the closing brace with "endmodule"

CHECK_RETURN Err MtCursor::emit_sym_field_declaration_list(MnNode n) {
  Err err;
  assert(cursor == n.start());
  assert(n.sym == sym_field_declaration_list);

  push_indent(n);

  auto child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = n.child(i);
    err << emit_ws();
    switch (child.sym) {
      case anon_sym_LBRACE:
        err << skip_over(child);
        err << emit_ws();
        break;
      case sym_access_specifier:
        err << comment_out(child);
        break;
      case sym_field_declaration:
        err << emit_sym_field_decl(child);
        break;
      case sym_function_definition:
        err << emit_sym_function_definition(child);
        break;
      case anon_sym_RBRACE:
        err << emit_replacement(child, "endmodule");
        break;
      default:
        err << emit_default(child);
        break;
    }
  }

  pop_indent(n);
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_expression_statement(MnExprStatement node) {
  Err err;
  assert(node.sym == sym_expression_statement);
  assert(cursor == node.start());

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);
    err << emit_ws();
    switch (child.sym) {
      case anon_sym_SEMI:
        err << emit_text(child);
        break;
      case sym_comment:
        err << emit_sym_comment(child);
        break;
      default:
        if (child.sym == sym_call_expression) {
          // If this is a call expression, this node is a block-level call to a
          // component - since there's nothing on the LHS to bind the output to,
          // we just comment out the whole call.
          err << comment_out(child);
        } else {
          err << emit_expression(child);
        }
        break;
    }
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_template_argument(MnNode node) {
  Err err;
  assert(cursor == node.start());

  switch (node.sym) {
    case sym_number_literal:
      err << emit_sym_number_literal(node);
      break;
    case sym_type_descriptor:
      err << emit_type(node);
      break;
    case sym_binary_expression:
      err << emit_expression(node);
      break;
    default:
      err << ERR("emit_template_argument: No handler for %s\n",
                 node.ts_node_type());
      debugbreak();
      break;
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// Change logic<N> to logic[N-1:0]

CHECK_RETURN Err MtCursor::emit_sym_template_type(MnTemplateType n) {
  Err err;
  assert(n.sym == sym_template_type);
  assert(cursor == n.start());

  err << emit_sym_type_identifier(n.name());
  err << emit_ws();

  auto args = n.args();

  bool is_logic = n.name().match("logic");
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
        err << emit_template_argument(logic_size);
        err << emit_print("-1:0]");
        break;
    }
  } else {
    err << emit_sym_template_argument_list(args);
  }

  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// Change <param, param> to #(param, param)

CHECK_RETURN Err
MtCursor::emit_sym_template_argument_list(MnTemplateArgList n) {
  Err err;
  assert(n.sym == sym_template_argument_list);
  assert(cursor == n.start());

  auto child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    auto c = n.child(i);
    err << emit_ws();
    switch (c.sym) {
      case anon_sym_LT:
        err << emit_replacement(c, " #(");
        break;
      case anon_sym_GT:
        err << emit_replacement(c, ")");
        break;
      case anon_sym_COMMA:
        err << emit_text(c);
        break;
      default:
        err << emit_template_argument(c);
        break;
    }
  }
  if (cursor != n.end()) err << ERR("Cursor not at end");
  return err;
}

//------------------------------------------------------------------------------
// Enum lists do _not_ turn braces into begin/end.

CHECK_RETURN Err MtCursor::emit_sym_enumerator_list(MnEnumeratorList node) {
  Err err;
  assert(node.sym == sym_enumerator_list);
  assert(cursor == node.start());

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);
    err << emit_ws();
    switch (child.sym) {
      case anon_sym_LBRACE:
      case anon_sym_RBRACE:
      case anon_sym_COMMA:
        err << emit_text(child);
        break;
      case sym_enumerator:
        err << emit_text(child);
        break;
      default:
        err << ERR("FIXME finish emit_sym_enumerator_list\n");
        child.error();
        break;
    }
  }
  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_everything() {
  Err err;

  cursor = current_source->source;
  err << emit_ws();
  err << emit_sym_translation_unit(current_source->root_node);
  err << emit_print("\n");

  return err;
}

//------------------------------------------------------------------------------
// Discard any trailing semicolons in the translation unit.

CHECK_RETURN Err MtCursor::emit_sym_translation_unit(MnTranslationUnit n) {
  Err err;
  assert(n.sym == sym_translation_unit);
  assert(cursor == n.start());

  auto child_count = n.child_count();
  for (int i = 0; i < child_count; i++) {
    auto c = n.child(i);
    err << emit_ws();
    switch (c.sym) {
      case sym_preproc_def:
      case sym_preproc_if:
      case sym_preproc_ifdef:
      case sym_preproc_else:
      case sym_preproc_call:
      case sym_preproc_arg:
      case sym_preproc_include:
        err << emit_preproc(c);
        break;

      case sym_comment:
        err << emit_sym_comment(c);
        break;

      case sym_class_specifier:
      case sym_struct_specifier:
      case sym_type_definition:
      case sym_template_declaration:
        err << emit_type(c);
        break;

      case sym_namespace_definition:
        err << emit_sym_namespace_definition(c);
        break;

      case anon_sym_SEMI:
        err << skip_over(c);
        break;
      case sym_expression_statement:
        if (c.text() != ";") {
          err << ERR(
              "Found unexpected expression statement in translation unit\n");
        }
        err << skip_over(c);
        break;

      default:
        err << ERR("emit_translation_unit - No handler for %s\n",
                   c.ts_node_type());
        n.error();
        break;
    }
  }

  if (cursor < current_source->source_end) {
    err << emit_span(cursor, current_source->source_end);
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_namespace_definition(MnNamespaceDef n) {
  Err err;
  assert(n.sym == sym_namespace_definition);
  assert(cursor == n.start());

  auto node_name = n.get_field(field_name);
  auto node_body = n.get_field(field_body);

  err << emit_print("package %s;", node_name.text().c_str());
  cursor = node_body.start();

  auto child_count = node_body.child_count();
  for (int i = 0; i < child_count; i++) {
    auto c = node_body.child(i);
    err << emit_ws();
    switch (c.sym) {
      case anon_sym_LBRACE:
        err << emit_replacement(c, "");
        break;
      case anon_sym_RBRACE:
        err << emit_replacement(c, "");
        break;
      case sym_comment:
        err << emit_sym_comment(c);
        break;

      case sym_struct_specifier:
      case sym_class_specifier:
      case sym_template_declaration:
        err << emit_type(c);
        break;

      case sym_namespace_definition:
        err << emit_sym_namespace_definition(c);
        break;
      case sym_declaration:
        err << emit_sym_declaration(c);
        break;
      default:
        err << ERR("emit_namespace_def - No handler for %s\n",
                   c.ts_node_type());
        c.error();
        break;
    }
  }

  err << emit_print("endpackage");
  err << emit_indent();
  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// Replace "0x" prefixes with "'h"
// Replace "0b" prefixes with "'b"
// Add an explicit size prefix if needed.

CHECK_RETURN Err MtCursor::emit_sym_number_literal(MnNumberLiteral n,
                                                   int size_cast) {
  Err err;
  assert(n.sym == sym_number_literal);
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
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Change "return x" to "(funcname) = x" to match old Verilog return style.

CHECK_RETURN Err MtCursor::emit_sym_return(MnReturnStatement n) {
  Err err;
  assert(cursor == n.start());
  assert(n.sym == sym_return_statement);

  auto node_lit = n.child(0);
  auto node_expr = n.child(1);

  cursor = node_expr.start();

  if (current_method->in_tock) {
    err << emit_print("%s_ret = ", current_method->name().c_str());
  } else {
    err << emit_print("%s = ", current_method->name().c_str());
  }

  err << emit_expression(node_expr);
  err << prune_trailing_ws();
  err << emit_print(";");

  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// FIXME translate types here

CHECK_RETURN Err MtCursor::emit_sym_primitive_type(MnDataType n) {
  Err err;
  assert(n.sym == sym_primitive_type);
  assert(cursor == n.start());
  err << emit_text(n);
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// FIXME translate types here

CHECK_RETURN Err MtCursor::emit_sym_identifier(MnIdentifier n) {
  Err err;
  assert(n.sym == sym_identifier);
  assert(cursor == n.start());

  auto name = n.name4();
  auto it = id_replacements.find(name);
  if (it != id_replacements.end()) {
    err << emit_replacement(n, it->second.c_str());
  } else {
    if (preproc_vars.contains(name)) {
      err << emit_print("`");
      err << emit_text(n);
    } else {
      err << emit_text(n);
    }
  }
  assert(cursor == n.end());
  return err;
}

CHECK_RETURN Err MtCursor::emit_sym_type_identifier(MnTypeIdentifier n) {
  Err err;
  assert(n.sym == alias_sym_type_identifier);
  assert(cursor == n.start());

  auto name = n.name4();
  auto it = id_replacements.find(name);
  if (it != id_replacements.end()) {
    err << emit_replacement(n, it->second.c_str());
  } else {
    err << emit_text(n);
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// For some reason the class's trailing semicolon ends up with the template
// field_decl, so we prune it here.

CHECK_RETURN Err MtCursor::emit_sym_template_declaration(MnTemplateDecl n) {
  Err err;
  assert(n.sym == sym_template_declaration);
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

  auto old_mod = current_mod;
  current_mod = lib->get_module(class_name);

  cursor = class_specifier.start();
  err << emit_type(class_specifier);
  cursor = n.end();

  current_mod = old_mod;
  assert(cursor == n.end());

  return err;
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz, so that a field expression instead
// refers to a glue expression.

CHECK_RETURN Err MtCursor::emit_sym_field_expression(MnFieldExpr n) {
  Err err;
  assert(n.sym == sym_field_expression);
  assert(cursor == n.start());

  auto component_name = n.get_field(field_argument).text();
  auto component_field = n.get_field(field_field).text();

  auto component = current_mod->get_component(component_name);

  if (component) {
    auto field = n.text();
    for (auto& c : field) {
      if (c == '.') c = '_';
    }
    err << emit_replacement(n, field.c_str());

    if (component->_type_mod->get_method(component_field)) {
      err << emit_print("_ret");
    }

  } else {
    // Local struct reference
    err << emit_text(n);
  }

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_case_statement(MnCaseStatement n) {
  Err err;
  assert(n.sym == sym_case_statement);
  assert(cursor == n.start());

  auto child_count = n.child_count();

  bool anything_after_colon = false;
  int colon_hit = false;

  for (int i = 0; i < child_count; i++) {
    auto c = n.child(i);
    if (c.sym == anon_sym_COLON) {
      colon_hit = true;
    } else {
      if (colon_hit) {
        if (c.sym != sym_comment) anything_after_colon = true;
        break;
      }
    }
  }

  for (int i = 0; i < child_count; i++) {
    auto child = n.child(i);

    switch (child.sym) {
      case sym_break_statement:
        err << skip_over(child);
        break;
      case sym_identifier:
        err << emit_sym_identifier(child);
        break;
      case anon_sym_case:
        err << skip_over(child);
        break;
      case anon_sym_COLON:
        if (anything_after_colon) {
          err << emit_text(child);
        } else {
          err << emit_replacement(child, ",");
        }
        break;
      case sym_compound_statement:
      case sym_expression_statement:
        err << emit_statement(child);
        break;
      default:
        err << emit_default(child);
        break;
    }

    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_switch_statement(MnSwitchStatement node) {
  Err err;
  assert(node.sym == sym_switch_statement);
  assert(cursor == node.start());

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);
    switch (child.field) {
      case field_condition:
        err << emit_sym_condition_clause(child);
        break;
      case field_body:
        err << emit_statement(child);
        break;
      default:
        err << emit_default(child);
        break;
    }
    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// Unwrap magic /*#foo#*/ comments to pass arbitrary text to Verilog.

CHECK_RETURN Err MtCursor::emit_sym_comment(MnComment n) {
  Err err;
  assert(n.sym == sym_comment);
  assert(cursor == n.start());

  auto body = n.text();
  if (body.starts_with("/*#") && body.ends_with("#*/")) {
    body.erase(body.size() - 3, 3);
    body.erase(0, 3);
    err << emit_replacement(n, body.c_str());
  } else {
    err << emit_text(n);
  }
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Verilog doesn't use "break"

CHECK_RETURN Err MtCursor::emit_sym_break_statement(MnBreakStatement n) {
  Err err;
  assert(n.sym == sym_break_statement);
  assert(cursor == n.start());

  err << skip_over(n);

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// TreeSitter nodes slightly broken for "a = b ? c : d;"...

CHECK_RETURN Err MtCursor::emit_sym_conditional_expression(MnCondExpr n) {
  Err err;
  assert(n.sym == sym_conditional_expression);
  assert(cursor == n.start());

  err << emit_child_expressions(n);

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// Static variables become localparams at module level.

CHECK_RETURN Err MtCursor::emit_sym_storage_class_specifier(MnStorageSpec n) {
  Err err;
  assert(n.sym == sym_storage_class_specifier);

  /*
  assert(cursor == n.start());
  if (n.match("static")) {
    err << emit_replacement(n, "localparam");
  } else {
    //err << comment_out(n);
    err << skip_over(n);
  }
  assert(cursor == n.end());
  */

  err << skip_over(n);
  err << skip_ws();

  return err;
}

//------------------------------------------------------------------------------
// Change "std::string" to "string".
// Change "enum::val" to "val" (SV doesn't support scoped enum values)

CHECK_RETURN Err MtCursor::emit_sym_qualified_identifier(MnQualifiedId node) {
  Err err;
  assert(node.sym == sym_qualified_identifier);
  assert(cursor == node.start());

  if (node.text() == "std::string") {
    err << emit_replacement(node, "string");
    return err;
  }

  auto node_scope = node.child(0);
  auto node_colon = node.child(1);
  auto node_name = node.child(2);

  err << emit_identifier(node_scope) << emit_ws();
  err << emit_text(node_colon) << emit_ws();
  err << emit_identifier(node_name);

  assert(cursor == node.end());
  return err;
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

CHECK_RETURN Err MtCursor::emit_statement(MnNode n) {
  Err err;
  assert(cursor == n.start());

  n.dump_tree();

  switch (n.sym) {
    case sym_declaration:
      err << emit_sym_declaration(n);
      assert(cursor == n.end());
      break;

    case sym_if_statement:
      err << emit_sym_if_statement(n);
      assert(cursor == n.end());
      break;
    case sym_compound_statement:
      err << emit_sym_compound_statement(n, "begin", "end");
      assert(cursor == n.end());
      break;
    case sym_expression_statement:
      err << emit_sym_expression_statement(n);
      assert(cursor == n.end());
      break;
    case sym_return_statement:
      err << emit_sym_return(n);
      assert(cursor == n.end());
      break;
    case sym_switch_statement:
      err << emit_sym_switch_statement(n);
      assert(cursor == n.end());
      break;
    case sym_using_declaration:
      err << emit_sym_using_declaration(n);
      assert(cursor == n.end());
      break;
    case sym_case_statement:
      err << emit_sym_case_statement(n);
      assert(cursor == n.end());
      break;
    case sym_break_statement:
      err << emit_sym_break_statement(n);
      assert(cursor == n.end());
      break;

    default:
      err << emit_default(n);
      assert(cursor == n.end());
      break;
  }

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_expression(MnNode n) {
  Err err;
  assert(cursor == n.start());

  if (!n.is_named()) {
    err << emit_text(n);
    assert(cursor == n.end());
    return err;
  }

  switch (n.sym) {
    case sym_identifier:
    case sym_qualified_identifier:
      err << emit_identifier(n);
      break;

    case sym_type_descriptor:
      err << emit_type(n);
      break;

    case sym_subscript_expression:
    case sym_binary_expression:
    case sym_parenthesized_expression:
    case sym_unary_expression:
      err << emit_child_expressions(n);
      break;

    case sym_field_expression:
      err << emit_sym_field_expression(n);
      break;
    case sym_call_expression:
      err << emit_sym_call_expression(n);
      break;
    case sym_assignment_expression:
      err << emit_sym_assignment_expression(n);
      break;
    case sym_conditional_expression:
      err << emit_sym_conditional_expression(n);
      break;
    case sym_update_expression:
      err << emit_sym_update_expression(n);
      break;

    default:
      err << emit_default(n);
      break;
  }

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_condition_clause(MnNode node) {
  Err err;
  assert(node.sym == sym_condition_clause);
  assert(cursor == node.start());

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);

    switch (child.field) {
      case field_value:
        err << emit_expression(child);
        break;
      default:
        err << emit_default(child);
        break;
    }

    if (err.has_err()) return err;
    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// If statements _must_ use {}, otherwise we have no place to insert component
// bindings if the branch contains a component method call.

CHECK_RETURN Err MtCursor::emit_sym_if_statement(MnIfStatement node) {
  Err err;
  assert(node.sym == sym_if_statement);
  assert(cursor == node.start());

  // There could be comments between the sections of the if/elses, so we just
  // walk over everything.

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);

    switch (child.sym) {
      case anon_sym_if:
      case anon_sym_else:
        err << emit_text(child);
        break;
      case sym_comment:
        err << emit_sym_comment(child);
        break;
      case sym_condition_clause:
        err << emit_sym_condition_clause(child);
        break;
      case sym_compound_statement:
        err << emit_statement(child);
        break;
      case sym_expression_statement:
        if (branch_contains_component_call(child)) {
          err << ERR("If branches that contain component calls must use {}.");
        }
        err << emit_statement(child);
        break;
      default:
        err << ERR("%s : No handler for %s\n", __func__, child.ts_node_type());
        child.error();
        break;
    }

    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------
// Enums are broken.

CHECK_RETURN Err MtCursor::emit_sym_enum_specifier(MnEnumSpecifier node) {
  Err err;
  assert(cursor == node.start());
  assert(node.sym == sym_enum_specifier);

  int child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);
    switch (child.field) {
      case field_name:
        err << emit_declarator(child);
        break;
      case field_body:
        err << emit_sym_enumerator_list(child);
        break;
      default:
        err << emit_default(child);
        break;
    }
    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_using_declaration(MnUsingDecl n) {
  Err err;
  assert(cursor == n.start());
  auto name = n.child(2).text();
  err << emit_replacement(n, "import %s::*;", name.c_str());
  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// FIXME - When do we hit this? It doesn't look finished.
// It's for namespace decls

CHECK_RETURN Err MtCursor::emit_sym_declaration(MnDecl n) {
  Err err;
  assert(n.sym == sym_declaration);
  assert(cursor == n.start());

  bool is_static = false;
  bool is_const = false;

  MnNode node_type;
  MnNode node_decl;
  MnNode node_semi;

  for (auto child : (MnNode)n) {
    if (child.sym == sym_storage_class_specifier && child.text() == "static")
      is_static = true;
    if (child.sym == sym_type_qualifier && child.text() == "const")
      is_const = true;

    if (child.field == field_type) node_type = child;
    if (child.field == field_declarator) node_decl = child;
    if (child.sym == anon_sym_SEMI) node_semi = child;
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

  cursor = node_type.start();
  err << emit_type(node_type) << emit_ws();
  err << emit_declarator(node_decl) << emit_ws();
  err << emit_expression(node_semi);

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------
// "unsigned int" -> "int unsigned"

CHECK_RETURN Err MtCursor::emit_sym_sized_type_specifier(MnSizedTypeSpec n) {
  Err err;
  assert(n.sym == sym_sized_type_specifier);
  assert(cursor == n.start());

  auto node_lit = n.child(0);
  auto node_type = n.child(1);

  err << emit_text(node_lit) << emit_ws();
  err << emit_type(node_type);

  cursor = n.end();
  return err;
}

//------------------------------------------------------------------------------
// Arg lists are the same in C and Verilog.

CHECK_RETURN Err MtCursor::emit_sym_argument_list(MnArgList node) {
  Err err;
  assert(cursor == node.start());

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);

    switch (child.sym) {
      case anon_sym_LPAREN:
      case anon_sym_RPAREN:
      case anon_sym_COMMA:
        err << emit_text(child);
        break;
      default:
        err << emit_expression(child);
        break;
    }
    if (err.has_err()) return err;
    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_parameter_list(MnParameterList node) {
  Err err;
  assert(node.sym == sym_parameter_list);
  assert(cursor == node.start());

  auto child_count = node.child_count();
  for (int i = 0; i < child_count; i++) {
    auto child = node.child(i);

    switch (child.sym) {
      case anon_sym_LPAREN:
      case anon_sym_RPAREN:
      case anon_sym_COMMA:
        err << emit_text(child);
        break;
      default:
        err << emit_declaration(child);
        break;
    }
    if (err.has_err()) return err;
    if (i != child_count - 1) err << emit_ws();
  }

  assert(cursor == node.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_field_identifier(MnFieldIdentifier n) {
  Err err;
  assert(n.sym == alias_sym_field_identifier);
  assert(cursor == n.start());
  err << emit_text(n);
  return err;
}

//------------------------------------------------------------------------------
// FIXME need to do smarter stuff here...

CHECK_RETURN Err MtCursor::emit_preproc(MnNode n) {
  Err err;
  switch (n.sym) {
    case sym_preproc_def: {
      auto node_lit = n.child(0);
      auto node_name = n.get_field(field_name);
      auto node_value = n.get_field(field_value);

      err << emit_replacement(node_lit, "`define");
      err << emit_ws();
      err << emit_text(node_name);
      if (!node_value.is_null()) {
        err << emit_ws();
        err << emit_text(node_value);
      }

      preproc_vars[node_name.text()] = node_value;
      break;
    }

    case sym_preproc_if:
      err << skip_over(n);
      break;

    case sym_preproc_ifdef: {
      debugbreak();
      // err << emit_chil dren(n);
      break;
    }

    case sym_preproc_else: {
      debugbreak();
      // err << emit_chil dren(n);
      break;
    }

    case sym_preproc_call:
      err << skip_over(n);
      break;

    case sym_preproc_arg: {
      // If we see any other #defined constants inside a #defined value,
      // prefix them with '`'
      std::string arg = n.text();
      std::string new_arg;

      for (int i = 0; i < arg.size(); i++) {
        for (const auto& def_pair : preproc_vars) {
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
      break;
    }
    case sym_preproc_include: {
      err << emit_sym_preproc_include(MnPreprocInclude(n));
      break;
    }
  }
  return err;
}

//------------------------------------------------------------------------------
// Emit the block with the correct type of "begin/end" pair, hoisting locals
// to the top of the body scope.

CHECK_RETURN Err MtCursor::emit_sym_compound_statement(
    MnNode node, const std::string& delim_begin, const std::string& delim_end) {
  Err err;
  assert(node.sym == sym_compound_statement);
  assert(cursor == node.start());
  push_indent(node);

  auto body_count = node.child_count();
  for (int i = 0; i < body_count; i++) {
    auto child = node.child(i);

    err << emit_input_port_bindings(child);

    switch (child.sym) {
      case anon_sym_LBRACE:
        err << emit_replacement(child, "%s", delim_begin.c_str());
        err << emit_ws_to_newline();
        err << emit_hoisted_decls(node);
        err << emit_ws();
        break;

      case sym_comment:
        err << emit_sym_comment(child);
        break;

      case sym_declaration:
        err << emit_sym_declaration(child);
        break;

      case sym_using_declaration:
        err << emit_sym_using_declaration(child);
        break;

      case sym_break_statement:
      case sym_if_statement:
      case sym_compound_statement:
      case sym_expression_statement:
      case sym_return_statement:
      case sym_switch_statement:
      case sym_case_statement:
        err << emit_statement(child);
        break;

      case anon_sym_RBRACE:
        err << emit_replacement(child, "%s", delim_end.c_str());
        break;

      default:
        err << ERR("emit_block: No handler for %s\n", child.ts_node_type());
        debugbreak();
        break;
    }
    if (err.has_err()) return err;
    if (i != body_count - 1) err << emit_ws();
  }

  pop_indent(node);
  assert(cursor == node.end());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_update_expression(MnNode n) {
  Err err;
  assert(cursor == n.start());

  auto id = n.get_field(field_argument);
  auto op = n.get_field(field_operator);

  if (n.get_field(field_operator).text() == "++") {
    push_cursor(id);
    err << emit_identifier(id);
    pop_cursor(id);
    err << emit_print(" = ");
    push_cursor(id);
    err << emit_identifier(id);
    pop_cursor(id);
    err << emit_print(" + 1");
  } else if (n.get_field(field_operator).text() == "--") {
    push_cursor(id);
    err << emit_identifier(id);
    pop_cursor(id);
    err << emit_print(" = ");
    push_cursor(id);
    err << emit_identifier(id);
    pop_cursor(id);
    err << emit_print(" - 1");
  } else {
    debugbreak();
  }

  cursor = n.end();

  assert(cursor == n.end());
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::check_done(MnNode n) {
  Err err;
  if (cursor < n.end()) {
    err << ERR("Cursor was left inside the current node\n");
  }

  if (cursor > n.end()) {
    for (auto c = n.end(); c < cursor; c++) {
      if (!isspace(*c)) {
        err << ERR("Cursor ended up protruding into the next node\n");
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_child_expressions(MnNode n) {
  Err err;

  assert(cursor == n.start());
  auto count = n.child_count();
  for (auto i = 0; i < count; i++) {
    err << emit_expression(n.child(i));
    if (i != count - 1) {
      err << emit_ws();
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_default(MnNode node) {
  Err err;
  assert(cursor == node.start());

  if (!node.is_named()) {
    err << emit_text(node);
    return err;
  }

  switch (node.sym) {
    case sym_comment:
      err << emit_sym_comment(node);
      break;
    case sym_string_literal:
      err << emit_text(node);
      break;
    case sym_number_literal:
      err << emit_sym_number_literal(node);
      break;
    default:
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
  }

  return err;
}
