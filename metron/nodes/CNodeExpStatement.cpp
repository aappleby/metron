#include "CNodeExpStatement.hpp"

#include "metron/Cursor.hpp"
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
  Err err = cursor.check_at(this);

  CNode* node_exp = child("exp");
  CNode* node_semi = child("semi");

  // Call expressions turn into either Verilog calls, or are replaced with
  // binding statements.
  if (auto call = node_exp->as<CNodeCall>()) {
    bool can_omit_call = false;
    // Calls into submodules always turn into bindings.
    if (auto func_path = call->node_name->as<CNodeFieldExpression>()) {
      can_omit_call = true;
    }

    // Calls into methods in the same module turn into bindings if needed.
    auto src_class = call->ancestor<CNodeClass>();
    auto dst_func = src_class->get_function(call->node_name->get_text());
    if (dst_func && dst_func->needs_binding()) {
      can_omit_call = true;
    }

    if (can_omit_call) {
      err << cursor.comment_out(this);
      return err << cursor.check_done(this);
    }
  }

  if (auto keyword = node_exp->as<CNodeKeyword>()) {
    if (keyword->get_text() == "break") {
      err << cursor.comment_out(this);
      return err << cursor.check_done(this);
    }
  }

  if (auto decl = node_exp->as<CNodeDeclaration>()) {
    if (cursor.elide_type.top()) {
      if (decl->node_value == nullptr) {
        err << cursor.skip_over(this);
        return err << cursor.check_done(this);
      }
    }
  }

  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
}

//==============================================================================
