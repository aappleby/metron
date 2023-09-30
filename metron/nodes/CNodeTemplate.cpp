#include "CNodeTemplate.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeKeyword.hpp"

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
  return Emitter(cursor).emit(this);
}

//------------------------------------------------------------------------------
