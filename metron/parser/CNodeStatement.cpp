#include "CNodeStatement.hpp"

#include "NodeTypes.hpp"
#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeStatement::debug_color() const {
  return COL_TEAL;
}

//------------------------------------------------------------------------------

Err CNodeExpStatement::trace(CCall* call) {
  Err err;
  for (auto c : this) err << c->trace(call);
  return err;
}

//----------------------------------------

Err CNodeExpStatement::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  CNode* node_exp = child("exp");
  CNode* node_semi = child("semi");

  if (auto call = node_exp->as<CNodeCall>()) {
    if (call->can_omit_call()) {
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

//------------------------------------------------------------------------------

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

//----------------------------------------

// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

Err CNodeAssignment::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto func = ancestor<CNodeFunction>();

  auto node_lhs  = child("lhs");
  auto node_op   = child("op");
  auto node_rhs  = child("rhs");

  err << cursor.emit(node_lhs);
  err << cursor.emit_gap();

  // If we're in a tick, emit < to turn = into <=
  if (func->method_type == MT_TICK) {
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
    err << cursor.emit_print("%.*s %c ", lhs_text.size(), lhs_text.data(), node_op->get_text()[1]);
  }

  err << cursor.emit(node_rhs);

  return err << cursor.check_done(this);
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

CHECK_RETURN Err CNodeFor::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  //dump_debug();
  err << cursor.emit_default(this);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

void CNodeIf::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_if    = child("if")->req<CNodeKeyword>();
  node_cond  = child("condition")->req<CNodeList>();
  node_true  = child("body_true")->req<CNodeStatement>();
  node_else  = child("else")->as<CNodeKeyword>();
  node_false = child("body_false")->as<CNodeStatement>();
}

//----------------------------------------

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

//----------------------------------------

CHECK_RETURN Err CNodeIf::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  /*
  // FIXME sanity checks
  if (node_then.sym == sym_expression_statement && branch_contains_component_call(node_then)) {
    return err << ERR("If branches that contain component calls must use {}.\n");
  }

  if (node_alt && node_alt.sym == sym_expression_statement && branch_contains_component_call(node_alt)) {
    return err << ERR("Else branches that contain component calls must use {}.\n");
  }
  */

  err << cursor.emit(node_if);
  err << cursor.emit_gap();

  err << cursor.emit(node_cond);
  err << cursor.emit_gap();

  err << cursor.emit(node_true);

  if (node_else) {
    err << cursor.emit_gap();
    err << cursor.emit(node_else);
  }

  if (node_false) {
    err << cursor.emit_gap();
    err << cursor.emit(node_false);
  }

  return err << cursor.check_done(this);
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

CHECK_RETURN Err CNodeSwitch::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  //dump_debug();

  auto node_switch = child("switch");
  auto node_cond   = child("condition");
  auto node_body   = child("body");

  err << cursor.emit_replacement(node_switch, "case");
  err << cursor.emit_gap();
  err << cursor.emit(node_cond);
  err << cursor.emit_gap();

  for (auto child : node_body) {
    if (child->tag_is("ldelim")) {
      err << cursor.skip_over(child);
    }
    else if (child->tag_is("rdelim")) {
      err << cursor.emit_replacement(child, "endcase");
    }
    else {
      err << cursor.emit(child);
    }

    if (child->node_next) err << cursor.emit_gap();
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeCase::trace(CCall* call) {
  return child("body")->trace(call);
}

CHECK_RETURN Err CNodeCase::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_case  = child("case");
  auto node_cond  = child("condition");
  auto node_colon = child("colon");
  auto node_body  = child("body");

  err << cursor.skip_over(node_case);
  err << cursor.skip_gap();
  err << cursor.emit(node_cond);
  err << cursor.emit_gap();

  if (node_body) {
    err << cursor.emit(node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node_body);
  }
  else {
    err << cursor.emit_replacement(node_colon, ",");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeDefault::trace(CCall* call) {
  return child("body")->trace(call);
}

CHECK_RETURN Err CNodeDefault::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_default = child("default");
  auto node_colon   = child("colon");
  auto node_body    = child("body");

  err << cursor.emit(node_default);
  err << cursor.emit_gap();

  if (node_body) {
    err << cursor.emit(node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node_body);
  }
  else {
    err << cursor.emit_replacement(node_colon, ",");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeCompound::trace(CCall* call) {
  Err err;
  for (auto c : this) err << c->trace(call);
  return err;
}

//----------------------------------------

CHECK_RETURN Err CNodeCompound::emit(Cursor& cursor) {
  return emit_block(cursor, "begin", "end");
}

//----------------------------------------

CHECK_RETURN Err CNodeCompound::emit_call_arg_bindings(CNode* child, Cursor& cursor) {
  Err err;

  // Emit bindings for child nodes first, but do _not_ recurse into compound
  // blocks.

  for (auto gc : child) {
    if (!gc->as<CNodeCompound>()) {
      err << emit_call_arg_bindings(gc, cursor);
    }
  }

  // OK, now we can emit bindings for the statement we're at.

  bool any_bindings = false;

  auto call = child->as<CNodeCall>();
  if (!call) return err;

  if (call->node_args->items.empty()) return err;

  if (auto func_path = call->node_name->as<CNodeFieldExpression>()) {
    auto field_name = func_path->node_path->get_text();
    auto func_name  = func_path->node_name->get_text();

    auto src_class = ancestor<CNodeClass>();
    auto field = src_class->get_field(field_name);
    auto dst_class = field->node_decl->_type_class;
    auto dst_func = dst_class->get_function(func_name);

    int arg_count = call->node_args->items.size();
    int param_count = dst_func->params.size();
    assert(arg_count == param_count);

    for (int i = 0; i < arg_count; i++) {
      auto param_name = dst_func->params[i]->child("name")->get_text();

      err << cursor.start_line();
      err << cursor.emit_print("%.*s_%.*s_%.*s = ",
        field_name.size(), field_name.data(),
        func_name.size(), func_name.data(),
        param_name.size(), param_name.data());
      err << cursor.emit_splice(call->node_args->items[i]);
      err << cursor.emit_print(";");

      any_bindings = true;
    }
  }

  if (auto func_id = call->node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_text();

    auto src_class = ancestor<CNodeClass>();
    auto dst_func = src_class->get_function(func_id->get_text());

    // FIXME we should have an is_builtin or something here...

    if (dst_func) {
      if (dst_func->needs_binding()) {
        int arg_count = call->node_args->items.size();
        int param_count = dst_func->params.size();
        assert(arg_count == param_count);

        for (int i = 0; i < arg_count; i++) {
          auto param_name = dst_func->params[i]->child("name")->get_text();
          err << cursor.start_line();
          err << cursor.emit_print("%.*s_%.*s = ",
            func_name.size(), func_name.data(),
            param_name.size(), param_name.data());
          err << cursor.emit_splice(call->node_args->items[i]);
          err << cursor.emit_print(";");

          any_bindings = true;
        }
      }
    }
  }

  if (any_bindings) {
    err << cursor.start_line();
  }

  return err;
}

//----------------------------------------

Err CNodeCompound::emit_block(Cursor& cursor, std::string ldelim, std::string rdelim) {
  Err err;

  cursor.elide_type.push(true);
  cursor.elide_value.push(false);

  for (auto child : this) {
    if (child->tag_is("ldelim")) {
      err << cursor.emit_replacement(child, "%s", ldelim.c_str());
      cursor.indent_level++;
      cursor.elide_type.push(false);
      cursor.elide_value.push(true);
      err << emit_hoisted_decls(cursor);
      cursor.elide_type.pop();
      cursor.elide_value.pop();
    }
    else if (child->tag_is("rdelim")) {
      cursor.indent_level--;
      err << cursor.emit_replacement(child, "%s", rdelim.c_str());
    }
    else {
      // We may need to insert input port bindings before any statement that
      // could include a call expression. We search the tree for calls and emit
      // those bindings here.
      if (!child->as<CNodeCompound>()) {
        err << emit_call_arg_bindings(child, cursor);
      }
      err << cursor.emit(child);
    }

    if (child->node_next) err << cursor.emit_gap();
  }

  cursor.elide_type.pop();
  cursor.elide_value.pop();

  return err;
}

//----------------------------------------7

Err CNodeCompound::emit_hoisted_decls(Cursor& cursor) {
  Err err;

  auto old_cursor = cursor.tok_cursor;

  cursor.elide_type.push(false);
  cursor.elide_value.push(true);

  for (auto child : this) {
    auto exp = child->as<CNodeExpStatement>();
    if (!exp) continue;
    auto decl = exp->child("exp")->as<CNodeDeclaration>();
    if (!decl) continue;

    // Don't emit decls for localparams
    if (decl->child("const")) continue;

    auto name = decl->get_namestr();

    auto decl_type = decl->child("type");
    auto decl_name = decl->child("name");

    err << cursor.start_line();

    //err << cursor.emit_print("DECL %s", name.c_str());

    cursor.tok_cursor = decl_type->tok_begin();

    err << cursor.emit(decl_type);
    err << cursor.emit_gap();
    err << cursor.emit(decl_name);
    err << cursor.emit_print(";");
  }

  cursor.elide_type.pop();
  cursor.elide_value.pop();

  cursor.tok_cursor = old_cursor;

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

//----------------------------------------7

Err CNodeReturn::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto func = ancestor<CNodeFunction>();
  auto fname = func->get_namestr();

  auto node_ret  = child("return");
  auto node_val  = child("value");
  auto node_semi = child("semi");

  assert(node_val);

  err << cursor.skip_over(node_ret);
  err << cursor.skip_gap();

  if (func->emit_as_task() || func->emit_as_func()) {
    err << cursor.emit_print("%s = ", fname.c_str());
  }
  else {
    err << cursor.emit_print("%s_ret = ", fname.c_str());
  }

  err << cursor.emit(node_val);
  err << cursor.emit_gap();

  err << cursor.emit(node_semi);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
