#include "CNodeExpStatement.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeFunction.hpp"

//==============================================================================

void CNodeExpStatement::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_exp = child("exp")->req<CNode>();
}


Err CNodeExpStatement::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto c : this) err << c->trace(inst, stack);
  return err;
}

//----------------------------------------

Err CNodeExpStatement::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

//==============================================================================
