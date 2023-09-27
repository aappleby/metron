#include "CNodeClass.hpp"

#include "NodeTypes.hpp"
#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

Err CNodeAccess::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.comment_out(this);
  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

void CNodeTemplate::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_template = child("template")->as<CNodeKeyword>();
  node_params   = child("params")->as<CNodeList>();
  node_class    = child("class")->as<CNodeClass>();

  for (auto child : node_params->items) {
    auto decl = child->as<CNodeDeclaration>();
    assert(decl);
    params.push_back(decl);
  }
}

//----------------------------------------

Err CNodeTemplate::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  err << cursor.skip_over(node_template);
  err << cursor.skip_gap();

  err << cursor.skip_over(node_params);
  err << cursor.skip_gap();

  err << cursor.emit(node_class);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

void CNodeClass::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_class = child("class")->as<CNodeKeyword>();
  node_name  = child("name")->as<CNodeIdentifier>();
  node_body  = child("body")->as<CNodeList>();

  name = node_name->get_namestr();

  for (auto child : node_body->items) {
    if (auto node_enum = child->as<CNodeEnum>()) {
      all_enums2.push_back(node_enum);
    }
  }
}

uint32_t CNodeClass::debug_color() const {
  return 0x00FF00;
}

std::string_view CNodeClass::get_name() const {
  return name;
}

CNodeField* CNodeClass::get_field(CNode* node_name) {
  if (node_name == nullptr) return nullptr;

  if (node_name->as<CNodeLValue>()) {
    return get_field(node_name->child("name"));
  }

  if (auto node_id = node_name->as<CNodeIdentifier>()) {
    return get_field(node_id->get_text());
  }

  if (auto node_field = node_name->as<CNodeFieldExpression>()) {
    return get_field(node_field->child("field_path"));
  }

  if (auto node_prefix = node_name->as<CNodeSuffixExp>()) {
    return get_field(node_prefix->child("rhs"));
  }

  if (auto node_suffix = node_name->as<CNodeSuffixExp>()) {
    return get_field(node_suffix->child("lhs"));
  }

  LOG_R("----------------------------------------\n");
  node_name->dump_tree();
  LOG_R("----------------------------------------\n");

  assert(false);
  return nullptr;
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
  for (auto e : all_enums2) if (e->get_name() == name) return e;
  return nullptr;
}

//------------------------------------------------------------------------------
// FIXME move this to init()

Err CNodeClass::collect_fields_and_methods() {
  Err err;

  bool is_public = false;

  for (auto child : node_body->items) {

    if (auto access = child->as<CNodeAccess>()) {
      is_public = child->get_text() == "public:";
      continue;
    }

    if (auto n = child->as<CNodeField>()) {
      n->is_public = is_public;

      n->parent_class  = this;
      n->parent_struct = nullptr;
      n->node_decl->_type_class    = repo->get_class(n->get_type_name());
      n->node_decl->_type_struct   = repo->get_struct(n->get_type_name());

      if (n->node_decl->node_static && n->node_decl->node_const) {
        all_localparams.push_back(n);
      }
      else {
        all_fields.push_back(n);
      }

      continue;
    }

    if (auto n = child->as<CNodeFunction>()) {
      n->is_public = is_public;

      if (auto constructor = child->as<CNodeConstructor>()) {
        assert(this->constructor == nullptr);
        this->constructor = constructor;
        constructor->method_type = MT_INIT;
      }
      else {
        all_functions.push_back(n);
      }

      // Hook up _type_struct on all struct params
      for (auto decl : n->params) {
        decl->_type_class = repo->get_class(decl->get_type_name());
        decl->_type_struct = repo->get_struct(decl->get_type_name());
      }
      continue;
    }

  }

  if (auto parent = node_parent->as<CNodeTemplate>()) {
    for (CNode*  param : parent->node_params->items) {
      if (param->as<CNodeDeclaration>()) {
        all_modparams.push_back(param->as<CNodeDeclaration>());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeClass::build_call_graph(CSourceRepo* repo) {
  Err err;

  node_visitor link_callers = [&](CNode* child) {
    auto call = child->as<CNodeCall>();
    if (!call) return;

    auto src_method = call->ancestor<CNodeFunction>();

    auto func_name = call->child("func_name");

    if (auto submod_path = func_name->as<CNodeFieldExpression>()) {
      auto submod_field = get_field(submod_path->child("field_path")->get_text());
      auto submod_class = repo->get_class(submod_field->get_type_name());
      auto submod_func  = submod_class->get_function(submod_path->child("identifier")->get_text());

      src_method->external_callees.insert(submod_func);
      submod_func->external_callers.insert(src_method);
    }
    else if (auto func_id = func_name->as<CNodeIdentifier>()) {
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

  if (constructor) {
    visit_children(constructor, link_callers);
  }

  for (auto src_method : all_functions) {
    visit_children(src_method, link_callers);
  }

  return err;
}

//------------------------------------------------------------------------------

void CNodeClass::dump_call_graph() {
  LOG_G("Class %s\n", name.c_str());

  LOG_INDENT();

  if (constructor) {
    LOG("Constructor\n");
    LOG_INDENT();
    constructor->dump_call_graph();
    LOG_DEDENT();
  }

  for (auto node_func : all_functions) {
    if (node_func->internal_callers.size()) {
      continue;
    }
    else {
      LOG("Func %s\n", node_func->name.c_str());
      LOG_INDENT();
      node_func->dump_call_graph();
      LOG_DEDENT();
    }
  }
  LOG_DEDENT();
}


//------------------------------------------------------------------------------

bool CNodeClass::needs_tick() {
  for (auto f : all_functions) {
    if (f->method_type == MT_TICK) return true;
  }

  for (auto f : all_fields) {
    if (f->node_decl->_type_class && f->node_decl->_type_class->needs_tick()) return true;
  }

  return false;
}

//----------------------------------------

bool CNodeClass::needs_tock() {
  for (auto f : all_functions) {
    if (f->method_type == MT_TOCK) return true;
  }

  for (auto f : all_fields) {
    if (f->node_decl->_type_class && f->node_decl->_type_class->needs_tock()) return true;
  }

  return false;
}

//------------------------------------------------------------------------------

Err CNodeClass::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_class = child<CNodeKeyword>();
  auto node_name  = child("name");
  auto node_body  = child("body");
  auto node_semi  = child("semi");

  err << cursor.emit_replacements(node_class, "class", "module");
  err << cursor.emit_gap();
  err << cursor.emit(node_name);
  err << cursor.emit_gap();

  err << cursor.emit_print("(");
  cursor.indent_level++;
  err << emit_module_ports(cursor);
  cursor.indent_level--;
  err << cursor.start_line();
  err << cursor.emit_print(");");

  for (auto child : node_body) {
    if (child->get_text() == "{") {
      cursor.indent_level++;
      err << cursor.skip_over(child);
      err << emit_template_parameter_list(cursor);
    }
    else if (child->get_text() == "}") {
      cursor.indent_level--;
      err << cursor.emit_replacement(child, "endmodule");
    }
    else {
      err << cursor.emit(child);
    }
    if (child->node_next) err << cursor.emit_gap();
  }

  err << cursor.skip_gap();
  err << cursor.skip_over(node_semi);

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
      err << emit_field_ports(f, false, cursor);
    }
  }

  if (output_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// output signals");
    for (auto f : output_signals) {
      err << emit_field_ports(f, true, cursor);
    }
  }

  if (output_registers.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// output registers");
    for (auto f : output_registers) {
      err << emit_field_ports(f, true, cursor);
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

  if (!f->is_public) return err;
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

Err CNodeClass::emit_field_ports(CNodeField* f, bool is_output, Cursor& cursor) {
  Err err;

  auto fname = f->get_namestr();

  err << cursor.start_line();
  if (is_output) {
    err << cursor.emit_print("output ");
  }
  else {
    err << cursor.emit_print("input ");
  }

  err << cursor.emit_splice(f->node_decl->child("type"));
  err << cursor.emit_print(" %s,", fname.c_str());

  return err;
}

//------------------------------------------------------------------------------

Err CNodeClass::emit_template_parameter_list(Cursor& cursor) {
  Err err;

  auto node_template = ancestor<CNodeTemplate>();
  if (!node_template) return err;

  auto old_cursor = cursor.tok_cursor;

  //err << cursor.emit_char('\n');
  //err << cursor.emit_indent();

  for (auto param : node_template->params) {
    err << cursor.start_line();
    err << cursor.emit_print("parameter ");

    //CNodeIdentifier* node_name = nullptr;
    //CNodeList*       node_array = nullptr;
    //CNodePunct*      node_eq = nullptr;
    //CNode*           node_value = nullptr;

    cursor.tok_cursor = param->node_name->tok_begin();
    err << cursor.emit(param->node_name);
    err << cursor.emit_gap();

    if (param->node_array) {
      err << cursor.emit(param->node_array);
      err << cursor.emit_gap();
    }

    err << cursor.emit(param->node_eq);
    err << cursor.emit_gap();

    err << cursor.emit(param->node_value);

    /*
    err << cursor.emit_print(" ");
    if (param->node_array) {
      err << cursor.emit_splice(param->node_array);
      err << cursor.emit_print(" ");
    }
    err << cursor.emit_splice(param->node_value);
    */

    err << cursor.emit_print(";");
  }

  err << cursor.emit_print("\n");

  cursor.tok_cursor = old_cursor;

  return err;
}

//------------------------------------------------------------------------------

void CNodeClass::dump() const {
  LOG_B("Class %s @ %s, refcount %d\n", name.c_str(), file->filename.c_str(), refcount);
  LOG_INDENT();
  {
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

    for (auto child : node_body) {
      child->dump();
    }
  }
  LOG_DEDENT();

  /*
  if (constructor) {
    LOG_G("Constructor\n");
    LOG_INDENT_SCOPE();
    constructor->dump();
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

  if (all_enums2.size()) {
    LOG_G("Enums\n");
    LOG_INDENT_SCOPE();
    for (auto e : all_enums2) e->dump();
  }
  */
}

//------------------------------------------------------------------------------
