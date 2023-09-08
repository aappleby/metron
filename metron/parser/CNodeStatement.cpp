#include "CNodeStatement.hpp"

#include "CNodeExpression.hpp"

#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeStatement::debug_color() const {
  return COL_TEAL;
}

//------------------------------------------------------------------------------

Err CNodeExpStatement::emit(Cursor& c) {
  NODE_ERR("Can't emit statement base class");
  return Err();
}

Err CNodeExpStatement::trace(CCall* call) {
  Err err;
  for (auto c : this) err << c->trace(call);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeAssignment::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeAssignment::trace(CCall* call) {
  Err err;

  auto rhs = child("rhs");
  err << rhs->trace(call);

  auto lhs = child("lhs");
  auto inst_lhs = call->inst_class->resolve(lhs);

  if (inst_lhs) {
    auto op_text = child("op")->get_text();
    if (op_text != "=") err << inst_lhs->log_action(this, ACT_READ);
    err << inst_lhs->log_action(this, ACT_WRITE);
  }

  return Err();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeFor::trace(CCall* call) {
  Err err;
  err << child("init")->trace(call);
  err << child("condition")->trace(call);
  err << child("body")->trace(call);
  err << child("step")->trace(call);
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeIf::trace(CCall* call) {
  Err err;

  err << child("condition")->trace(call);

  auto inst = call->inst_class;

  inst->push_state();
  if (auto body_true = child("body_true")) err << body_true->trace(call);
  inst->swap_state();
  if (auto body_false = child("body_false")) err << body_false->trace(call);
  inst->merge_state();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeSwitch::trace(CCall* call) {
  Err err;

  err << child("condition")->trace(call);

  auto inst = call->inst_class;

  int case_count = 0;
  bool has_default = false;

  for (auto cursor = child("ldelim"); cursor; cursor = cursor->node_next) {
    if (cursor->tag_is("default")) has_default = true;

    // Skip cases without bodies
    if (!cursor->child("body")) continue;

    inst->push_state();
    case_count++;
    err << cursor->trace(call);
    inst->swap_state();
  }

  if (has_default) {
    inst->pop_state();
    case_count--;
  }

  for (int i = 0; i < case_count; i++) {
    inst->merge_state();
  }

  return err;
}

CHECK_RETURN Err CNodeCase::trace(CCall* call) {
  return child("body")->trace(call);
}

CHECK_RETURN Err CNodeDefault::trace(CCall* call) {
  return child("body")->trace(call);
}

//------------------------------------------------------------------------------

Err CNodeCompound::emit_block(Cursor& c, std::string ldelim, std::string rdelim) {
  dump_tree();
  /*
  Err err;
  block_prefix.push(prefix);
  block_suffix.push(suffix);
  err << emit_dispatch(n);
  block_prefix.pop();
  block_suffix.pop();
  return err;
  */
  return Err();
}

Err CNodeCompound::trace(CCall* call) {
  Err err;
  for (auto c : this) err << c->trace(call);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::trace(CCall* call) {
  Err err;

  if (auto node_value = child("value")) {
    err << node_value->trace(call);
  }

  return err;
}

//------------------------------------------------------------------------------
