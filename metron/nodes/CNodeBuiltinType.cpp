#include "CNodeBuiltinType.hpp"

#include "metron/nodes/CNodeConstant.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"

//==============================================================================

std::string_view CNodeBuiltinType::get_name() const { return get_text(); }

//------------------------------------------------------------------------------

Err CNodeBuiltinType::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeBuiltinType::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_name = child("name");
  auto node_targs = child("template_args");
  auto node_scope = child("scope");

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
    err << cursor.emit_default(this);
  }

  return err << cursor.check_done(this);
}

//==============================================================================
