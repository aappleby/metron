#include "CNodeFieldExpression.hpp"

#include "metron/CInstance.hpp"
#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"

//==============================================================================

void CNodeFieldExpression::init(const char* match_tag, SpanType span,
                                uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_path = child("field_path")->as<CNodeIdentifier>();
  node_name = child("identifier")->as<CNodeIdentifier>();
}

//------------------------------------------------------------------------------

uint32_t CNodeFieldExpression::debug_color() const { return 0x80FF80; }

//------------------------------------------------------------------------------

std::string_view CNodeFieldExpression::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//------------------------------------------------------------------------------

Err CNodeFieldExpression::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto inst_dst = inst->resolve(this);

  err << inst_dst->log_action(this, ACT_READ, stack);

  return err;
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz if the field refers to a submodule port,
// so that it instead refers to a glue expression.

Err CNodeFieldExpression::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

//==============================================================================
