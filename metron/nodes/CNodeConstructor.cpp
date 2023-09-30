#include "CNodeConstructor.hpp"

#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"

//------------------------------------------------------------------------------

void CNodeConstructor::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  //node_type   = child("return_type")->req<CNodeType>();
  node_type   = nullptr;
  node_name   = child("name")->req<CNodeIdentifier>();
  node_params = child("params")->req<CNodeList>();
  node_init   = child("init")->opt<CNodeList>();
  node_const  = child("const")->opt<CNodeKeyword>();
  node_body   = child("body")->req<CNodeCompound>();

  for (auto c : child("params")) {
    if (auto param = c->as<CNodeDeclaration>()) {
      params.push_back(param);
    }
  }
}

//------------------------------------------------------------------------------
