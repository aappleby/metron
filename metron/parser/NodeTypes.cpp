#include "NodeTypes.hpp"

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

void CNodeClass::init(const char* match_name, SpanType span, uint64_t flags) {
  CNode::init(match_name, span, flags);

  name = child("name")->as_string();
}

//------------------------------------------------------------------------------


Err CNodeClass::collect_fields_and_methods() {
  Err err;

  for (auto c : this) {
    if (auto n = c->as_a<CNodeDeclaration>()) {
      all_fields.push_back(n);
    }
    if (auto n = c->as_a<CNodeFunction>()) {
      all_methods.push_back(n);
    }
  }

  if (auto parent = node_parent->as_a<CNodeTemplate>()) {
    //LOG_B("CNodeClass has template parent\n");
    auto params = parent->child_as<CNodeTemplateParams>("template_params");
    for (auto param : params) {
      if (param->is_a<CNodeDeclaration>()) {
        //param->dump_tree(3);
        all_modparams.push_back(param->as_a<CNodeDeclaration>());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeClass::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto n_class = child("class");
  auto n_name  = child("name");
  auto n_body  = child("body");

  err << cursor.emit_replacements(n_class->as_string_view(), "class", "module");
  err << cursor.emit_gap(n_class, n_name);
  err << cursor.emit(n_name);
  err << cursor.emit_gap(n_name, n_body);

  err << cursor.emit_print("(\n");
  //cursor.push_indent(node_body);
  err << cursor.emit_print("{{port list}}");
  //err << cursor.emit_module_ports(node_body);
  //cursor.pop_indent(node_body);
  err << cursor.emit_line(");");


  err << cursor.emit(n_body);

  return err << cursor.check_done(this);
}

/*
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
*/

//------------------------------------------------------------------------------

Err CNodeStruct::collect_fields_and_methods() {
  Err err;
  return err;
}

//------------------------------------------------------------------------------

Err CNodeTemplate::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);


  // Template params have to go _inside_ the class definition in Verilog, so
  // we skip them here.
  auto class_node = child("template_class");
  err << cursor.skip_span(cursor.text_cursor, class_node->text_begin());
  err << cursor.emit(class_node);
  err << cursor.skip_span(cursor.text_cursor, text_end());
  //err << emit_rest(cursor);

  //cursor.current_mod.pop();
  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodePreproc::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto v = as_string_view();

  if (v.starts_with("#include")) {
    err << cursor.emit_replacements(v, "#include", "`include", ".h", ".sv");
  }
  else if (v.starts_with("#ifndef")) {
    err << cursor.emit_replacements(v, "#ifndef", "`ifndef");
  }
  else if (v.starts_with("#define")) {
    err << cursor.emit_replacements(v, "#define", "`define");
  }
  else if (v.starts_with("#endif")) {
    err << cursor.emit_replacements(v, "#endif", "`endif");
  }
  else {
    return ERR("Don't know how to handle this preproc");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeFieldList::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  for (auto c : (CNode*)this) {
    if (c->as_string_view() == "{") {
      err << cursor.skip_over(c);
      err << cursor.emit_gap_after(c);
      err << cursor.emit_print("{{template parameter list}}\n");
    }
    else if (c->as_string_view() == "}") {
      err << cursor.emit_replacement(c, "endmodule");
      err << cursor.emit_gap_after(c);
    }
    else {
      err << cursor.emit(c);
      err << cursor.emit_gap_after(c);
    }

  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
