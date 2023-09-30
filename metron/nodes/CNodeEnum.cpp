#include "CNodeEnum.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeType.hpp"

//==============================================================================

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

//------------------------------------------------------------------------------

std::string_view CNodeEnum::get_name() const {
  auto n = child("name");
  return n ? n->get_text() : "<unnamed>";
}

//------------------------------------------------------------------------------

void CNodeEnum::dump() const {
  auto name = get_name();
  LOG_G("Enum %.*s\n", name.size(), name.data());
}

//==============================================================================

void CNodeEnumerator::init(const char* match_tag, SpanType span,
                           uint64_t flags) {
  CNode::init(match_tag, span, flags);
}

//==============================================================================
