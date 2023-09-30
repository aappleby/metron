#include "CNodeDeclaration.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"

//------------------------------------------------------------------------------

void CNodeDeclaration::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_static = child("static")->as<CNodeKeyword>();
  node_const  = child("const")->as<CNodeKeyword>();
  node_type   = child("type")->as<CNodeType>();
  node_name   = child("name")->as<CNodeIdentifier>();
  node_array  = child("array")->as<CNodeList>();
  node_eq     = child("eq")->as<CNodePunct>();
  node_value  = child("value")->as<CNode>();
}

//------------------------------------------------------------------------------

uint32_t CNodeDeclaration::debug_color() const { return 0xFF00FF; }

std::string_view CNodeDeclaration::get_name() const {
  return node_name->get_name();
}

std::string_view CNodeDeclaration::get_type_name() const {
  return node_type->child_head->get_text();
}

bool CNodeDeclaration::is_array() const {
  return node_array != nullptr;
}

//------------------------------------------------------------------------------

Err CNodeDeclaration::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeDeclaration::trace(CInstance* inst, call_stack& stack) {
  Err err;
  if (node_value) {
    err << node_value->trace(inst, stack);
  }
  return err;
}

//------------------------------------------------------------------------------

void CNodeDeclaration::dump() const {
  auto text = get_text();
  LOG_G("Declaration `%.*s`\n", text.size(), text.data());
}

//------------------------------------------------------------------------------
