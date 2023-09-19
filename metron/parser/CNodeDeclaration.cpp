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
  Err err = cursor.check_at(this);

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
        if (c->node_next) err << cursor.skip_gap();
      }
      else {
        err << cursor.emit(c);
        if (c->node_next) err << cursor.emit_gap();
      }
    }

    return err << cursor.check_done(this);
  }



  for (auto child : this) {
    if (cursor.elide_type.top()) {
      if (child->as<CNodeType>()) {
        err << cursor.skip_over(child);
        if (child->node_next) err << cursor.skip_gap();
        continue;
      }
    }

    err << cursor.emit(child);
    if (child->node_next) err << cursor.emit_gap();
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeDeclaration::trace(CInstance* inst) {
  Err err;
  if (node_value) {
    err << node_value->trace(inst);
  }
  return err;
}

//------------------------------------------------------------------------------

void CNodeDeclaration::dump() {
  auto text = get_text();
  LOG_G("Declaration `%.*s`\n", text.size(), text.data());
}

//------------------------------------------------------------------------------
