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
  //dump_tree();

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

  //dump_tree();

  for (auto c : child("body")) {
    if (auto node_enum = c->as_a<CNodeEnum>()) {
      all_enums.push_back(node_enum);
    }
  }
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

CNodeEnum* CNodeClass::get_enum(std::string_view name) {
  for (auto e : all_enums) if (e->get_name() == name) return e;
  return nullptr;
}

//------------------------------------------------------------------------------
// FIXME move this to init()

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
      n->_public = is_public;

      n->_parent_class  = n->ancestor<CNodeClass>();
      n->_parent_struct = n->ancestor<CNodeStruct>();
      n->_type_class    = repo->get_class(n->get_type_name());
      n->_type_struct   = repo->get_struct(n->get_type_name());

      if (n->child("static") && n->child("const")) {
        all_localparams.push_back(n);
      }
      else {
        all_fields.push_back(n);
      }

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

  for (auto src_method : all_constructors) {
    visit_children(src_method, link_callers);
  }

  for (auto src_method : all_functions) {
    visit_children(src_method, link_callers);
  }

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
  for (auto f : all_functions) {
    if (f->method_type == MT_TICK) return true;
  }

  for (auto f : all_fields) {
    if (f->_type_class && f->_type_class->needs_tick()) return true;
  }

  return false;
}

//----------------------------------------

bool CNodeClass::needs_tock() {
  for (auto f : all_functions) {
    if (f->method_type == MT_TOCK) return true;
  }

  for (auto f : all_fields) {
    if (f->_type_class && f->_type_class->needs_tock()) return true;
  }

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

  err << cursor.emit_print("(");
  cursor.indent_level++;
  err << emit_module_ports(cursor);
  cursor.indent_level--;
  err << cursor.start_line();
  err << cursor.emit_print(");");

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

Err CNodeClass::emit_module_ports(Cursor& cursor) {
  Err err;

  if (needs_tick()) {
    err << cursor.start_line();
    err << cursor.emit_print("// global clock");
    err << cursor.start_line();
    err << cursor.emit_print("input logic clock,");
  }

  if (input_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// input signals");
    for (auto f : input_signals) {
      err << emit_field_ports(f, cursor);
    }
  }

  if (output_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// output signals");
    for (auto f : output_signals) {
      err << emit_field_ports(f, cursor);
    }
  }

  if (output_registers.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// output registers");
    for (auto f : output_registers) {
      err << emit_field_ports(f, cursor);
    }
  }

  for (auto f : all_functions) {
    err << emit_function_ports(f, cursor);
  }

  // Remove trailing comma from port list
  if (cursor.at_comma) {
    err << cursor.emit_backspace();
  }

  return err;
}

//----------------------------------------

Err CNodeClass::emit_function_ports(CNodeFunction* f, Cursor& cursor) {
  Err err;

  auto fname = f->get_namestr();
  auto rtype = f->child("return_type");

  if (!f->is_public_) return err;
  if (f->internal_callers.size()) return err;
  if (!f->params.size() && rtype->get_text() == "void") return err;

  err << cursor.start_line();
  err << cursor.emit_print("// %s() ports", fname.c_str());

  for (auto param : f->params) {
    auto pname = param->get_namestr();
    auto ptype = param->child("type");

    err << cursor.start_line();
    err << cursor.emit_print("input ");
    err << cursor.emit_splice(ptype);
    err << cursor.emit_print(" %s_%s", fname.c_str(), pname.c_str());
    err << cursor.emit_print(",");

  }

  if (rtype->get_text() != "void") {
    err << cursor.start_line();
    err << cursor.emit_print("output ");
    err << cursor.emit_splice(rtype);
    err << cursor.emit_print(" %s_ret,", fname.c_str());
  }

  return err;
}

//----------------------------------------

Err CNodeClass::emit_field_ports(CNodeField* f, Cursor& cursor) {
  Err err;

  auto fname = f->get_namestr();

  err << cursor.start_line();
  if (f->field_type == FT_INPUT) {
    err << cursor.emit_print("input ");
  }
  else if (f->field_type == FT_OUTPUT) {
    err << cursor.emit_print("output ");
  }

  err << cursor.emit_splice(f->child("type"));
  err << cursor.emit_print(" %s,", fname.c_str());

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
  LOG_INDENT_SCOPE();

  LOG_G("File %s\n", file->filename.c_str());
  LOG_G("Refcount %d\n", refcount);

  if (all_modparams.size()) {
    LOG_G("Modparams\n");
    LOG_INDENT_SCOPE();
    for (auto f : all_modparams) f->dump();
  }

  if (all_localparams.size()) {
    LOG_G("Localparams\n");
    LOG_INDENT_SCOPE();
    for (auto f : all_localparams) f->dump();
  }

  if (all_constructors.size()) {
    LOG_G("Constructors\n");
    LOG_INDENT_SCOPE();
    for (auto f : all_constructors) f->dump();
  }

  if (all_functions.size()) {
    LOG_G("Functions\n");
    LOG_INDENT_SCOPE();
    for (auto m : all_functions) m->dump();
  }

  if (all_fields.size()) {
    LOG_G("Fields\n");
    LOG_INDENT_SCOPE();
    for (auto f : all_fields) f->dump();
  }

  if (all_enums.size()) {
    LOG_G("Enums\n");
    LOG_INDENT_SCOPE();
    for (auto e : all_enums) e->dump();
  }
}

//------------------------------------------------------------------------------
