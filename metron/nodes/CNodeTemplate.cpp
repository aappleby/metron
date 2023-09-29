#include "CNodeTemplate.hpp"

#include "metron/nodes/CNodeDeclaration.hpp"

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
