#include "CNodeDeclaration.hpp"

#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

void CNodeDeclaration::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_static = child("static")->as<CNodeKeyword>();
  node_const  = child("const")->as<CNodeKeyword>();
  node_type   = child("type")->as<CNodeType>();
  node_name   = child("name")->as<CNodeIdentifier>();
  node_array  = child("array")->as<CNodeList>();
  node_eq     = child("eq")->as<CNodePunct>();
  node_value  = child("value")->as<CNodeExpression>();
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

/*
[000.014]  ▆ CNodeDeclaration = 0x7ffff7e1c288:0x7ffff7e1c2a8
[000.014]  ┣━━╸▆ type : CNodeBuiltinType = 0x7ffff7e1c288:0x7ffff7e1c290
[000.014]  ┃   ┗━━╸▆ name : CNodeIdentifier = 0x7ffff7e1c288:0x7ffff7e1c290 "int"
[000.014]  ┣━━╸▆ name : CNodeIdentifier = 0x7ffff7e1c290:0x7ffff7e1c298 "y"
[000.014]  ┣━━╸▆ eq : CNodePunct = 0x7ffff7e1c298:0x7ffff7e1c2a0 "="
[000.014]  ┗━━╸▆ value : CNodeIdentifier = 0x7ffff7e1c2a0:0x7ffff7e1c2a8 "my_sig3"
*/

Err CNodeDeclaration::emit(Cursor& cursor) {
  Err err;

  // Check for const char*
  if (node_const) {
    if (node_type->child("name")->get_text() == "char") {
      if (node_type->child("star")) {
        return cursor.emit_replacement(this, "{{const char*}}");
      }
    }

    err << cursor.emit_print("parameter ");

    for (auto c = child_head; c; c = c->node_next) {
      if (c->as<CNodeType>()) {
        err << cursor.skip_over(c);
        if (c->node_next) err << cursor.skip_gap(c, c->node_next);
      }
      else {
        err << cursor.emit(c);
        if (c->node_next) err << cursor.emit_gap(c, c->node_next);
      }
    }

    return err;
  }

  for (auto child : this) {
    if (child->as<CNodeType>()) {
      err << cursor.skip_over(child);
      err << cursor.skip_gap_after(child);
    }
    else {
      err << cursor.emit_default(child);
      err << cursor.emit_gap_after(child);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeDeclaration::trace(CCall* call) {
  Err err;
  if (node_value) {
    err << node_value->trace(call);
  }
  return err;
}

//------------------------------------------------------------------------------

void CNodeDeclaration::dump() {
  auto text = get_text();
  LOG_G("Declaration `%.*s`\n", text.size(), text.data());
}

//------------------------------------------------------------------------------
