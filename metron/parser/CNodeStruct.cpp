#include "CNodeStruct.hpp"

#include "NodeTypes.hpp"
#include "metrolib/core/Log.h"

struct CSourceRepo;
struct CSourceFile;

//------------------------------------------------------------------------------

uint32_t CNodeStruct::debug_color() const {
  return 0xFFAAAA;
}

//------------------------------------------------------------------------------

std::string_view CNodeStruct::get_name() const {
  return child("name")->get_name();
}

//------------------------------------------------------------------------------

Err CNodeStruct::collect_fields_and_methods() {
  Err err;

  auto body = child("body");
  for (auto c : body) {
    if (auto n = c->as<CNodeField>()) {
      n->parent_struct = n->ancestor<CNodeStruct>();
      n->node_decl->_type_struct  = repo->get_struct(n->get_type_name());
      all_fields.push_back(n);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeStruct::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_struct = child("struct");
  auto node_name   = child("name");
  auto node_body   = child("body");
  auto node_semi   = child("semi");

  err << cursor.emit_replacement(node_struct, "typedef struct packed");
  err << cursor.emit_gap();
  err << cursor.skip_over(node_name);
  err << cursor.skip_gap();
  err << cursor.emit_default(node_body);
  err << cursor.emit_print(" ");
  err << cursor.emit_splice(node_name);
  err << cursor.emit_gap();
  err << cursor.emit(node_semi);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

void CNodeStruct::dump() const {
  auto name = get_name();
  LOG_B("Struct %.*s @ %p\n", name.size(), name.data(), this);
  LOG_INDENT();

  if (all_fields.size()) {
    for (auto f : all_fields) f->dump();
  }

  LOG_DEDENT();
}

//------------------------------------------------------------------------------

void CNodeEnum::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_enum  = child("enum")->as<CNodeKeyword>();
  node_class = child("class")->as<CNodeKeyword>();
  node_name  = child("name")->as<CNodeIdentifier>();
  node_colon = child("colon")->as<CNodePunct>();
  node_type  = child("base_type")->as<CNodeType>();
  node_body  = child("body")->as<CNodeList>();
  node_decl  = child("decl");
  node_semi  = child("semi")->as<CNodePunct>();
}

CHECK_RETURN Err CNodeEnum::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  dump_parse_tree();

  // Extract enum bit width, if present.
  cursor.override_size.push(32);
  if (node_type) {
    if (auto targs = node_type->child("template_args")->as<CNodeList>()) {
      cursor.override_size.top() = atoi(targs->items[0]->text_begin());
    }
  }

  if (!node_decl) {
    err << cursor.emit_print("typedef ");
  }

  err << cursor.emit(node_enum);
  err << cursor.emit_gap();

  if (node_class) {
    err << cursor.skip_over(node_class);
    err << cursor.skip_gap();
  }

  if (node_name) {
    err << cursor.skip_over(node_name);
    err << cursor.skip_gap();
  }

  if (node_colon) {
    err << cursor.skip_over(node_colon);
    err << cursor.skip_gap();
    err << cursor.emit(node_type);
    err << cursor.emit_gap();
  }

  err << cursor.emit(node_body);
  err << cursor.emit_gap();

  if (node_decl) {
    err << cursor.emit(node_decl);
    err << cursor.emit_gap();
  }

  if (node_name) {
    err << cursor.emit_print(" ");
    err << cursor.emit_splice(node_name);
    //err << cursor.skip_gap();
  }

  err << cursor.emit(node_semi);

  cursor.override_size.pop();

  return err << cursor.check_done(this);
}

std::string_view CNodeEnum::get_name() const {
  auto n = child("name");
  return n ? n->get_text() : "<unnamed>";
}

void CNodeEnum::dump() const {
  auto name = get_name();
  LOG_G("Enum %.*s\n", name.size(), name.data());
}

//------------------------------------------------------------------------------

void CNodeEnumerator::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
}

CHECK_RETURN Err CNodeEnumerator::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  err << cursor.emit_default(this);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
