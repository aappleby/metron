#include "CNodeClass.hpp"

#include "NodeTypes.hpp"
#include "CNodeFunction.hpp"

//------------------------------------------------------------------------------

void CNodeClass::init(const char* match_name, SpanType span, uint64_t flags) {
  CNode::init(match_name, span, flags);
}

std::string_view CNodeClass::get_name() const {
  return child("name")->get_name();
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

Err CNodeClass::build_call_graph() {
  Err err;

  /*
  for (auto src_method : all_methods) {
    auto src_mod = this;

    src_method->_node.visit_tree([&](MnNode child) {
      if (child.sym != sym_call_expression) return;

      auto func = child.get_field(field_function);

      if (func.sym == sym_identifier) {
        auto dst_mod = this;
        auto dst_method = get_method(func.text());
        if (dst_method) {
          dst_method->internal_callers.insert(src_method);
          src_method->internal_callees.insert(dst_method);
        }
      }

      if (func.sym == sym_field_expression) {
        auto component_name = func.get_field(field_argument);
        auto component_method_name = func.get_field(field_field).text();

        auto component = get_field(component_name.name4());
        if (component) {
          auto dst_mod = source_file->lib->get_module(component->type_name());
          if (dst_mod) {
            auto dst_method = dst_mod->get_method(component_method_name);
            if (dst_method) {
              dst_method->external_callers.insert(src_method);
              src_method->external_callees.insert(dst_method);
            }
          }
        }
      }
    });
  }
  */

  return err;
}

//------------------------------------------------------------------------------

bool CNodeClass::needs_tick() {
  for (auto m : all_methods) {
    if (m->is_tick()) return true;
  }

  /*
  for (auto f : all_fields) {
    if (f->needs_tick()) return true;
  }
  */

  return false;
}

bool CNodeClass::needs_tock() {
  /*
  for (auto m : all_methods) {
    if (m->is_tock()) return true;
    if (m->is_func() && m->internal_callers.empty() && m->is_public()) return true;
  }

  for (auto f : all_fields) {
    if (f->is_module() && f->needs_tock()) return true;
  }
  */

  return false;
}


//------------------------------------------------------------------------------

Err CNodeClass::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto n_class = child("class");
  auto n_name  = child("name");
  auto n_body  = child("body");

  err << cursor.emit_replacements(n_class->get_text(), "class", "module");
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
