#include "NodeTypes.hpp"

#include "CInstance.hpp"

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

uint32_t CNodeTranslationUnit::debug_color() const { return 0xFFFF00; }

std::string_view CNodeTranslationUnit::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTranslationUnit::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  /*
  if (tok_begin() != child_head->tok_begin()) {
    err << cursor.emit_span(tok_begin(), child_head->tok_begin());
  }
  */

  for (auto c = child_head; c; c = c->node_next) {
    err << cursor.emit(c);
    if (c->node_next) err << cursor.emit_gap();
  }

  /*
  if (child_tail->tok_end() != tok_end()) {
    err << cursor.emit_span(child_tail->tok_end(), tok_end());
  }
  */

  return err << cursor.check_done(this);
}

Err CNodeTranslationUnit::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

/*
[000.001]  ┣━━╸▆ CNodeNamespace =
[000.001]  ┃   ┣━━╸▆ namespace : CNodeKeyword = "namespace"
[000.001]  ┃   ┣━━╸▆ name : CNodeIdentifier = "MyPackage"
[000.001]  ┃   ┗━━╸▆ fields : CNodeList =
[000.001]  ┃       ┣━━╸▆ ldelim : CNodePunct = "{"
[000.001]  ┃       ┣━━╸▆ CNodeField =
[000.001]  ┃       ┃   ┣━━╸▆ CNodeKeyword = "static"
[000.001]  ┃       ┃   ┣━━╸▆ CNodeKeyword = "const"
[000.001]  ┃       ┃   ┣━━╸▆ type : CNodeBuiltinType =
[000.002]  ┃       ┃   ┃   ┗━━╸▆ name : CNodeIdentifier = "int"
[000.002]  ┃       ┃   ┣━━╸▆ name : CNodeIdentifier = "foo"
[000.002]  ┃       ┃   ┣━━╸▆ CNodePunct = "="
[000.002]  ┃       ┃   ┗━━╸▆ value : CNodeConstInt = "3"
[000.002]  ┃       ┣━━╸▆ CNodePunct = ";"
[000.002]  ┃       ┗━━╸▆ rdelim : CNodePunct = "}"
*/

uint32_t CNodeNamespace::debug_color() const { return 0xFFFFFF; }

std::string_view CNodeNamespace::get_name() const {
  return child("name")->get_text();
}

Err CNodeNamespace::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto node_namespace = child("namespace");
  auto node_name      = child("name");
  auto node_fields    = child("fields");
  auto node_semi      = child("semi");

  err << cursor.emit_replacement(node_namespace, "package");
  err << cursor.emit_gap();

  err << cursor.emit_raw(node_name);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  for (auto f : node_fields) {
    if (f->tag_is("ldelim")) {
      err << cursor.skip_over(f);
      err << cursor.emit_gap();
      continue;
    }
    else if (f->tag_is("rdelim")) {
      err << cursor.emit_replacement(f, "endpackage");
      err << cursor.emit_gap();
      continue;
    }
    else {
      err << f->emit(cursor);
      err << cursor.emit_gap();
      continue;
    }
  }

  // Don't need semi after namespace in Verilog
  err << cursor.skip_over(node_semi);

  return err << cursor.check_done(this);
}

Err CNodeNamespace::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeNamespace::collect_fields_and_methods() {
  for (auto c : child("fields")) {
    if (auto field = c->as<CNodeField>()) {
      all_fields.push_back(field);
    }
  }

  return Err();
}

CNodeField* CNodeNamespace::get_field(std::string_view name) {
  for (auto f : all_fields) {
    if (f->get_name() == name) return f;
  }
  return nullptr;
}

void CNodeNamespace::dump() const {
  LOG_G("Fields:\n");
  LOG_INDENT_SCOPE();
  for (auto n : all_fields) n->dump();
}

//------------------------------------------------------------------------------

uint32_t CNodePreproc::debug_color() const { return 0x00BBBB; }

std::string_view CNodePreproc::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodePreproc::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto v = get_text();

  if (v.starts_with("#include")) {
    err << cursor.emit_replacements(this, "#include", "`include", ".h", ".sv");
  } else if (v.starts_with("#ifndef")) {
    err << cursor.emit_replacements(this, "#ifndef", "`ifndef");
  } else if (v.starts_with("#define")) {
    err << cursor.emit_replacements(this, "#define", "`define");
  } else if (v.starts_with("#endif")) {
    err << cursor.emit_replacements(this, "#endif", "`endif");
  } else {
    return ERR("Don't know how to handle this preproc");
  }

  return err << cursor.check_done(this);
}

Err CNodePreproc::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeIdentifier::debug_color() const { return 0x80FF80; }

std::string_view CNodeIdentifier::get_name() const {
  return get_text();
}

Err CNodeIdentifier::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto text = get_textstr();

  auto& id_map = cursor.id_map.top();
  auto found = id_map.find(text);

  if (found != id_map.end()) {
    auto replacement = (*found).second;
    err << cursor.emit_replacement(this, "%s", replacement.c_str());
  }
  else {
    err << cursor.emit_default(this);
  }

  //err << emit_span(n->tok_begin(), n->tok_end());

  return err << cursor.check_done(this);

  //
}

Err CNodeIdentifier::trace(CInstance* inst, call_stack& stack) {
  if (auto inst_field = inst->resolve(this)) {
    return inst_field->log_action(this, ACT_READ, stack);
  }
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodePunct::debug_color() const { return 0x88FF88; }

std::string_view CNodePunct::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodePunct::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
}

Err CNodePunct::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//------------------------------------------------------------------------------

void CNodeFieldExpression::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_path = child("field_path")->as<CNodeIdentifier>();
  node_name = child("identifier")->as<CNodeIdentifier>();
}

//----------------------------------------

uint32_t CNodeFieldExpression::debug_color() const { return 0x80FF80; }

//----------------------------------------

std::string_view CNodeFieldExpression::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//----------------------------------------

Err CNodeFieldExpression::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto inst_dst = inst->resolve(this);

  err << inst_dst->log_action(this, ACT_READ, stack);

  return err;
}

//----------------------------------------

// [000.004]  ▆ rhs : CNodeFieldExpression = 0x7ffff77ca168:0x7ffff77ca180
// [000.004]  ┣━━╸▆ field_path : CNodeIdentifier = 0x7ffff77ca168:0x7ffff77ca170 "tla"
// [000.004]  ┣━━╸▆ CNodePunct = 0x7ffff77ca170:0x7ffff77ca178 "."
// [000.004]  ┗━━╸▆ identifier : CNodeIdentifier = 0x7ffff77ca178:0x7ffff77ca180 "a_data"

// Replace foo.bar.baz with foo_bar_baz if the field refers to a submodule port,
// so that it instead refers to a glue expression.

Err CNodeFieldExpression::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_func = ancestor<CNodeFunction>();
  auto node_class = ancestor<CNodeClass>();
  auto node_path = child("field_path");
  auto node_name = child("identifier");

  auto field = node_class->get_field(node_path->get_text());

  if (field && field->node_decl->_type_struct) {
    err << cursor.emit_default(this);
    return err;
  }

  // FIXME this needs the submod_ prefix for submod ports

  if (field) {
    auto field = get_textstr();
    for (auto& c : field) {
      if (c == '.') c = '_';
    }
    err << cursor.emit_replacement(this, field.c_str());
  }
  else {
    err << cursor.emit_default(this);
  }

  return err << cursor.check_done(this);
}


/*
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
*/

//==============================================================================

uint32_t CNodeQualifiedIdentifier::debug_color() const { return 0x80FF80; }

//----------------------------------------

std::string_view CNodeQualifiedIdentifier::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//----------------------------------------

Err CNodeQualifiedIdentifier::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_scope = child("scope_path");
  auto node_colon = child("colon");
  auto node_name  = child("identifier");

  bool elide_scope = false;

  if (node_scope->get_text() == "std") elide_scope = true;

  // FIXME what was this for?
  // Verilog doesn't like the scope on the enums
  //if (current_mod.top()->get_enum(node_scope.text())) elide_scope = true;

  auto node_class = ancestor<CNodeClass>();

  if (node_class && node_class->get_enum(node_scope->get_text())) {
    elide_scope = true;
  }

  if (elide_scope) {
    err << cursor.skip_to(node_name);
    err << cursor.emit(node_name);
  }
  else {
    err << cursor.emit(node_scope);
    err << cursor.emit_gap();
    err << cursor.emit(node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node_name);
  }

  return err << cursor.check_done(this);
}

//----------------------------------------

Err CNodeQualifiedIdentifier::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto scope = child("scope_path")->get_name();
  auto field = child("identifier")->get_name();

  if (auto node_namespace = get_repo()->get_namespace(scope)) {
    LOG_R("namespace!\n");
    return node_namespace->get_field(field)->trace(inst, stack);
  }

  if (auto node_enum = get_repo()->get_enum(scope)) {
    return Err();
  }

  if (auto node_class = ancestor<CNodeClass>()) {
    if (auto node_enum = node_class->get_enum(scope)) {
      return Err();
    }
  }

  NODE_ERR("Don't know how to trace this qualified identifier");

  return err;
}

//==============================================================================

uint32_t CNodeText::debug_color() const { return 0x888888; }

std::string_view CNodeText::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeText::emit(Cursor& cursor) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeText::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeKeyword::debug_color() const { return 0xFFFF88; }

std::string_view CNodeKeyword::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeKeyword::emit(Cursor& cursor) {
  auto text = get_text();

  if (text == "static" || text == "const") {
    return cursor.comment_out(this);
  }
  if (text == "nullptr") {
    return cursor.emit_replacement(this, "\"\"");
  }
  if (text == "if" || text == "else") {
    return cursor.emit_raw(this);
  }
  if (text == "break") {
    return cursor.comment_out(this);
  }
  if (text == "default" || text == "for") {
    return cursor.emit_raw(this);
  }
  if (text == "enum") {
    return cursor.emit_raw(this);
  }
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeKeyword::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeTypedef::debug_color() const { return 0xFFFF88; }

std::string_view CNodeTypedef::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTypedef::emit(Cursor& cursor) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeTypedef::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

void CNodeList::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  for (auto child : this) {
    if (!child->as<CNodePunct>()) items.push_back(child);
  }
}

uint32_t CNodeList::debug_color() const { return 0xCCCCCC; }

std::string_view CNodeList::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeList::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
}

Err CNodeList::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto child : this) {
    err << child->trace(inst, stack);
  }
  return err;
}

//------------------------------------------------------------------------------
