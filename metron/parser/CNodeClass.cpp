#include "CNodeClass.hpp"

#include "CNodeCall.hpp"
#include "CNodeClass.hpp"
#include "CNodeDeclaration.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStruct.hpp"
#include "NodeTypes.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

Err CNodeAccess::emit(Cursor& cursor) {
  Err err;
  err << cursor.comment_out(this);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeTemplate::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_template = child("template");
  auto node_params   = child("params");
  auto node_class    = child("class");

  err << cursor.skip_over(node_template);
  err << cursor.skip_gap_after(node_template);

  err << cursor.skip_over(node_params);
  err << cursor.skip_gap_after(node_params);

  err << cursor.emit(node_class);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

void CNodeClass::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
}

uint32_t CNodeClass::debug_color() const {
  return 0x00FF00;
}

std::string_view CNodeClass::get_name() const {
  return child("name")->get_name();
}

CNodeField* CNodeClass::get_field(std::string_view name) {
  for (auto f : all_fields) if (f->get_name() == name) return f;
  return nullptr;
}

CNodeFunction* CNodeClass::get_function(std::string_view name) {
  for (auto m : all_functions) if (m->get_name() == name) return m;
  return nullptr;
}

CNodeDeclaration* CNodeClass::get_modparam(std::string_view name) {
  for (auto p : all_modparams) if (p->get_name() == name) return p;
  return nullptr;
}

//------------------------------------------------------------------------------

Err CNodeClass::collect_fields_and_methods() {
  Err err;

  auto body = child("body");

  bool is_public = false;

  for (auto c : body) {
    if (auto access = c->as_a<CNodeAccess>()) {
      is_public = c->get_text() == "public:";
      continue;
    }

    if (auto n = c->as_a<CNodeField>()) {
      n->_static = n->child("static") != nullptr;
      n->_const  = n->child("const")  != nullptr;
      n->_public = is_public;

      n->_parent_class  = n->ancestor<CNodeClass>();
      n->_parent_struct = n->ancestor<CNodeStruct>();
      n->_type_class    = repo->get_class(n->get_type_name());
      n->_type_struct   = repo->get_struct(n->get_type_name());


      all_fields.push_back(n);
      continue;
    }

    if (auto n = c->as_a<CNodeFunction>()) {
      n->is_public_ = is_public;

      if (auto constructor = c->as_a<CNodeConstructor>()) {
        all_constructors.push_back(constructor);
      }
      else {
        all_functions.push_back(n);
      }

      // Hook up _type_struct on all struct params
      auto params = n->child("params");
      for (auto p : params) {
        if (auto decl = p->as_a<CNodeDeclaration>()) {
          decl->_type_class = repo->get_class(decl->get_type_name());
          decl->_type_struct = repo->get_struct(decl->get_type_name());
        }
      }
      continue;
    }

  }

  if (auto parent = node_parent->as_a<CNodeTemplate>()) {
    //LOG_B("CNodeClass has template parent\n");
    CNode* params = parent->child("params");
    for (CNode*  param : params) {
      if (param->as_a<CNodeDeclaration>()) {
        //param->dump_tree(3);
        all_modparams.push_back(param->as_a<CNodeDeclaration>());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeClass::build_call_graph(CSourceRepo* repo) {
  Err err;

#if 0
  {
    using func_visitor = std::function<void(CNodeFunction*)>;

    std::function<void(CNodeFunction* node, func_visitor v)> visit;

    visit = [&visit](CNodeFunction* n, func_visitor v) {
      v(n);
      for (auto f : n->internal_callees) visit(f, v);
    };

    for (auto f : all_constructors) {
      visit(f, [](CNodeFunction* f) {
        auto name = f->get_name();
        LOG_R("Visited func %.*s is init\n", name.size(), name.data());

        //f->is_init_ = true;
      });
    }
  }

  for (auto src_method : all_constructors) {
    visit(src_method, [&](CNode* child) {
      auto call = child->as_a<CNodeCall>();
      if (!call) return;
      auto func_id = call->child("func_name")->as_a<CNodeIdentifier>();
      auto dst_method = get_function(func_id->get_text());
      dst_method->is_init_ = true;
      auto name = func_id->get_text();
      LOG_Y("Func %.*s is called by constructor\n", name.size(), name.data());
    });
  }
#endif

  node_visitor link_callers = [&](CNode* child) {
    auto call = child->as_a<CNodeCall>();
    if (!call) return;

    auto src_method = call->ancestor<CNodeFunction>();

    auto func_name = call->child("func_name");

    if (auto submod_path = func_name->as_a<CNodeFieldExpression>()) {
      auto submod_field = get_field(submod_path->child("field_path")->get_text());
      auto submod_class = repo->get_class(submod_field->get_type_name());
      auto submod_func  = submod_class->get_function(submod_path->child("identifier")->get_text());

      src_method->external_callees.insert(submod_func);
      submod_func->external_callers.insert(src_method);
    }
    else if (auto func_id = func_name->as_a<CNodeIdentifier>()) {
      auto dst_method = get_function(func_id->get_text());
      if (dst_method) {
        src_method->internal_callees.insert(dst_method);
        dst_method->internal_callers.insert(src_method);
      }
    }
    else {
      assert(false);
    }
  };

  for (auto src_method : all_functions) {
    visit_children(src_method, link_callers);
  }

  return err;
}


//------------------------------------------------------------------------------

Err CNodeClass::categorize_fields(bool verbose) {
  Err err;

  if (verbose) {
    auto name = get_name();
    LOG_G("Categorizing %.*s\n", int(name.size()), name.data());
  }

#if 0
  for (auto f : all_fields) {
    if (f->is_param()) {
      continue;
    }

    if (f->is_component()) {
      components.push_back(f);
    }
    else if (f->is_public() && f->is_input()) {
      input_signals.push_back(f);
    }
    else if (f->is_public() && f->is_signal()) {
      output_signals.push_back(f);
    }
    else if (f->is_public() && f->is_register()) {
      output_registers.push_back(f);
    }
    else if (f->is_private() && f->is_register()) {
      private_registers.push_back(f);
    }
    else if (f->is_private() && f->is_signal()) {
      private_signals.push_back(f);
    }
    else if (!f->is_public() && f->is_input()) {
      private_registers.push_back(f);
    }
    else if (f->is_enum()) {
    }
    else if (f->is_dead()) {
      dead_fields.push_back(f);
    }
    else {
      err << ERR("Don't know how to categorize %s = %s\n", f->cname(),
                 to_string(f->_state));
      f->error();
    }
  }
#endif

  return err;
}


//------------------------------------------------------------------------------

#if 0
CNode* CNodeClass::resolve(CNode* name) {
  if (!name) {
    assert(false);
    return nullptr;
  }

  //----------

  if (auto punct = name->as_a<CNodePunct>()) {
    return resolve(name->node_next);
  }

  if (auto field = name->as_a<CNodeFieldExpression>()) {
    return resolve(name->child_head);
  }

  if (auto qual = name->as_a<CNodeQualifiedIdentifier>()) {
    return resolve(name->child_head);
  }

  //----------

  if (auto id = name->as_a<CNodeIdentifier>()) {
    if (auto func = get_function(name->get_name())) {
      return func;
    }
    else if (auto field = get_field(name->get_name())) {
      if (field->node_next) {
        auto field_class = repo->get_class(field->get_type_name());
        return field_class->resolve(name->node_next);
      }
      else {
        return field;
      }
    }
    else {
      return nullptr;
    }
  }

  //----------

  /*
  if (auto id = name->as_a<CNodeIdentifier>()) {
    if (name->tag_is("scope")) {
      assert(false);
      return nullptr;
    }

    if (name->tag_is("field_path")) {
      auto field = get_field(name->get_text());
    }

    //printf("### %s ###\n", name->match_tag);
    return get_function(name->get_name());
  }
  */

  //----------

  assert(false && "Could not resolve function name");
  return nullptr;
}
#endif

//------------------------------------------------------------------------------

bool CNodeClass::needs_tick() {
  for (auto m : all_functions) {
    //if (m->is_tick()) return true;
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

  auto n_class = child<CNodeKeyword>();
  auto n_name  = child("name");
  auto n_body  = child("body");

  err << cursor.emit_replacements(n_class, "class", "module");
  err << cursor.emit_gap(n_class, n_name);

  err << cursor.emit(n_name);
  err << cursor.emit_gap(n_name, n_body);

  err << emit_module_ports(cursor);

  for (auto child : n_body) {
    if (child->get_text() == "{") {
      cursor.indent_level++;
      err << cursor.skip_over(child);
      err << emit_template_parameter_list(cursor);
      err << cursor.emit_gap_after(child);
    }
    else if (child->get_text() == "}") {
      cursor.indent_level--;
      err << cursor.emit_replacement(child, "endmodule");
      err << cursor.emit_gap_after(child);
    }
    else {
      err << cursor.emit(child);
      err << cursor.emit_gap_after(child);
    }
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

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

Err CNodeClass::emit_module_ports(Cursor& cursor) {
  Err err;
  err << cursor.emit_print("(\n");
  err << cursor.emit_print("{{port list}}\n");
  err << cursor.emit_print(");\n");
  return err;
}

//------------------------------------------------------------------------------

/*
CHECK_RETURN Err MtCursor::emit_template_params_as_modparams(MnNode n) {
  Err err;
  push_cursor(n);

  for (auto c : n) {
    switch (c.sym) {
      case anon_sym_LT:
      case anon_sym_GT:
      case anon_sym_COMMA:
        err << skip_over(c);
        err << skip_ws_inside(n);
        break;

      case sym_optional_parameter_declaration:
        err << emit_optional_param_as_modparam(c);
        err << emit_ws_inside(n);
        break;

      case sym_parameter_declaration:
        err << ERR("Parameter '%s' must have a default value\n", c.text().c_str());
        break;

      default:
        err << emit_dispatch(c);
        err << emit_ws_inside(n);
        break;
    }
  }

  err << start_line();
  pop_cursor();
  return err;
}
*/

Err CNodeClass::emit_template_parameter_list(Cursor& cursor) {
  Err err;

  auto node_template = ancestor<CNodeTemplate>();
  if (node_template) {
    err << cursor.emit_char('\n');
    err << cursor.emit_indent();
    err << cursor.emit_print("{{template parameter list}}");
  }

  return err;
}

//------------------------------------------------------------------------------

void CNodeClass::dump() {
  auto name = get_name();
  LOG_B("Class %.*s @ %p\n", name.size(), name.data(), this);
  LOG_INDENT();

  if (refcount) {
    LOG_G("Refcount %d\n", refcount);
  }
  else {
    LOG_G("Top module\n");
  }

  if (all_fields.size()) {
    for (auto f : all_fields) f->dump();
  }

  if (all_functions.size()) {
    for (auto m : all_functions) m->dump();
  }

  LOG_DEDENT();
}
