#include "CNodeCompound.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeExpStatement.hpp"
#include "metron/nodes/CNodeFor.hpp"

//==============================================================================

void CNodeCompound::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  for (auto child : this) {
    if (!child->tag_is("ldelim") && !child->tag_is("rdelim")) {
      statements.push_back(child);
    }
  }
}

//------------------------------------------------------------------------------

Err CNodeCompound::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto statement : statements) {
    err << statement->trace(inst, stack);
  }
  return err;
}

//==============================================================================
