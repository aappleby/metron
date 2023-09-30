#include "Emitter.hpp"

#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeAssignment.hpp"
#include "metron/nodes/CNodeBuiltinType.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeConstant.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeLValue.hpp"

//==============================================================================

Err Emitter::emit(CNodeAccess* node) {
  Err err = cursor.check_at(node);
  err << cursor.comment_out(node);
  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------
// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

Err Emitter::emit(CNodeAssignment* node) {
  Err err = cursor.check_at(node);

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func  = node->ancestor<CNodeFunction>();
  auto node_lhs   = node->child("lhs")->req<CNodeLValue>();
  auto node_op    = node->child("op");
  auto node_rhs   = node->child("rhs");
  auto node_field = node_class->get_field(node_lhs);

  err << cursor.emit(node_lhs);
  err << cursor.emit_gap();

  // If we're in a tick, emit < to turn = into <=
  if (node_func->method_type == MT_TICK && node_field) {
    err << cursor.emit_print("<");
  }

  if (node_op->get_text() == "=") {
    err << cursor.emit(node_op);
    err << cursor.emit_gap();
  }
  else {
    auto lhs_text = node_lhs->get_text();

    err << cursor.skip_over(node_op);
    err << cursor.emit_print("=");
    err << cursor.emit_gap();
    err << cursor.emit_print("%.*s %c ", lhs_text.size(), lhs_text.data(), node_op->get_text()[0]);
  }

  err << cursor.emit(node_rhs);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeBuiltinType* node) {
  Err err = cursor.check_at(node);

  auto node_name  = node->child("name");
  auto node_targs = node->child("template_args");
  auto node_scope = node->child("scope");

  if (node_targs) {
    auto node_ldelim = node_targs->child("ldelim");
    auto node_exp = node_targs->child("arg");
    auto node_rdelim = node_targs->child("rdelim");

    err << cursor.emit_raw(node_name);
    err << cursor.emit_gap();

    if (auto node_const_int = node_exp->as<CNodeConstInt>()) {
      auto width = atoi(node_exp->text_begin());
      if (width == 1) {
        // logic<1> -> logic
        err << cursor.skip_over(node_targs);
      } else {
        // logic<N> -> logic[N-1:0]
        err << cursor.emit_replacement(node_ldelim, "[");
        err << cursor.emit_gap();
        err << cursor.skip_over(node_exp);
        err << cursor.emit_gap();
        err << cursor.emit_print("%d:0", width - 1);
        err << cursor.emit_replacement(node_rdelim, "]");
      }
    } else if (auto node_identifier = node_exp->as<CNodeIdentifier>()) {
      // logic<CONSTANT> -> logic[CONSTANT-1:0]
      err << cursor.emit_replacement(node_ldelim, "[");
      err << cursor.emit_gap();
      err << cursor.skip_over(node_exp);
      err << cursor.emit_gap();
      err << cursor.emit_splice(node_identifier);
      err << cursor.emit_print("-1:0");
      err << cursor.emit_replacement(node_rdelim, "]");
    } else {
      // logic<exp> -> logic[(exp)-1:0]
      err << cursor.emit_replacement(node_ldelim, "[");
      err << cursor.emit_gap();
      err << cursor.emit_print("(");
      err << cursor.emit(node_exp);
      err << cursor.emit_gap();
      err << cursor.emit_print(")-1:0");
      err << cursor.emit_replacement(node_rdelim, "]");
    }

    if (node_scope) {
      err << cursor.skip_gap();
      err << cursor.skip_over(node_scope);
    }
  } else {
    err << cursor.emit_default(node);
  }

  return err << cursor.check_done(node);
}

//==============================================================================
