#include "NodeTypes.hpp"

#include "CInstance.hpp"
#include "CNodeClass.hpp"
#include "CNodeType.hpp"
#include "CNodeCall.hpp"
#include "CNodeField.hpp"
#include "CNodeStruct.hpp"

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

uint32_t CNodeTranslationUnit::debug_color() const { return 0xFFFF00; }

std::string_view CNodeTranslationUnit::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTranslationUnit::emit(Cursor& cursor) {
  //return cursor.emit_default(this);

  Err err = cursor.check_at(this);

  err << cursor.emit_span(tok_begin(), child_head->tok_begin());
  for (auto c = child_head; c; c = c->node_next) {
    // Skip semicolons after classes
    if (c->get_text() == ";" && c->node_prev && c->node_prev->as_a<CNodeClass>()) {
      err << cursor.skip_over(c);
      err << cursor.emit_gap_after(c);
      continue;
    }

    err << cursor.emit(c);
    if (c->node_next) {
      err << cursor.emit_gap(c, c->node_next);
    }
  }
  err << cursor.emit_span(child_tail->tok_end(), tok_end());

  return err << cursor.check_done(this);
}

Err CNodeTranslationUnit::trace(CCall* call) {
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

Err CNodeNamespace::emit(Cursor& c) {
  Err err;
  auto node_namespace = child("namespace");
  auto node_name      = child("name");
  auto node_fields    = child("fields");

  err << c.emit_replacement(node_namespace, "package");
  err << c.emit_gap_after(node_namespace);

  err << c.emit_raw(node_name);
  err << c.emit_print(";");
  err << c.emit_gap_after(node_name);

  for (auto f : node_fields) {
    if (f->tag_is("ldelim")) {
      err << c.skip_over(f);
      err << c.emit_gap_after(f);
      continue;
    }
    else if (f->tag_is("rdelim")) {
      err << c.emit_replacement(f, "endpackage");
      err << c.emit_gap_after(f);
      continue;
    }
    else {
      err << f->emit(c);
      err << c.emit_gap_after(f);
      continue;
    }
  }

  return Err();
}

Err CNodeNamespace::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeNamespace::collect_fields_and_methods() {
  for (auto c : child("fields")) {
    if (auto field = c->as_a<CNodeField>()) {
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

void CNodeNamespace::dump() {
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

Err CNodePreproc::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeIdentifier::debug_color() const { return 0x80FF80; }

std::string_view CNodeIdentifier::get_name() const {
  return get_text();
}

Err CNodeIdentifier::emit(Cursor& cursor) {
  auto text = get_textstr();

  auto& id_map = cursor.id_map.top();
  auto found = id_map.find(text);

  if (found != id_map.end()) {
    auto replacement = (*found).second;
    return cursor.emit_replacement(this, "%s", replacement.c_str());
  }
  else {
    return cursor.emit_default(this);
  }

  //err << emit_span(n->tok_begin(), n->tok_end());

  //
}

Err CNodeIdentifier::trace(CCall* call) {
  if (auto inst_field = call->inst_class->resolve(this)) {
    return inst_field->log_action(this, ACT_READ);
  }
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodePunct::debug_color() const { return 0x88FF88; }

std::string_view CNodePunct::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodePunct::emit(Cursor& c) {
  return c.emit_default(this);
}

Err CNodePunct::trace(CCall* call) {
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeFieldExpression::debug_color() const { return 0x80FF80; }

std::string_view CNodeFieldExpression::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeFieldExpression::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeFieldExpression::trace(CCall* call) {
  Err err;

  auto inst = call->inst_class->resolve(this);
  if (inst) {
    err << inst->log_action(this, ACT_READ);
  }

  return err;
}

//==============================================================================

uint32_t CNodeQualifiedIdentifier::debug_color() const { return 0x80FF80; }

//----------------------------------------

std::string_view CNodeQualifiedIdentifier::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//----------------------------------------

Err CNodeQualifiedIdentifier::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

//----------------------------------------

Err CNodeQualifiedIdentifier::trace(CCall* call) {
  Err err;

  auto scope = child("scope_path")->get_name();
  auto field = child("identifier")->get_name();

  if (auto node_namespace = get_repo()->get_namespace(scope)) {
    LOG_R("namespace!\n");
    return node_namespace->get_field(field)->trace(call);
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

Err CNodeText::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeText::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeKeyword::debug_color() const { return 0xFFFF88; }

std::string_view CNodeKeyword::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeKeyword::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeKeyword::trace(CCall* call) {
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeTypedef::debug_color() const { return 0xFFFF88; }

std::string_view CNodeTypedef::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTypedef::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeTypedef::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeList::debug_color() const { return 0xCCCCCC; }

std::string_view CNodeList::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeList::emit(Cursor& c) {
  return c.emit_default(this);
}

Err CNodeList::trace(CCall* call) {
  Err err;
  for (auto child : this) {
    err << child->trace(call);
  }
  return err;
}

//------------------------------------------------------------------------------
