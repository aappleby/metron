#include "CNodeStatement.hpp"

#include "NodeTypes.hpp"
#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeStatement::debug_color() const {
  return COL_TEAL;
}

//------------------------------------------------------------------------------

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
// For "x[y] = z;", we don't want to allow two writes to x as that would imply
// two write ports on a block mem. So we log a read on x before the write if
// there's an array suffix.

Err CNodeAssignment::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto rhs    = child("rhs");
  auto op     = child("op");
  auto lhs    = child("lhs")->req<CNodeLValue>();
  auto suffix = lhs->child("suffix");

  err << rhs->trace(inst, stack);

  if (suffix) {
    err << suffix->trace(inst, stack);
  }

  auto inst_lhs = inst->resolve(lhs->child("name"));
  if (!inst_lhs) return err;

  if (op->get_text() != "=" || suffix) {
    err << inst_lhs->log_action(this, ACT_READ, stack);
  }

  err << inst_lhs->log_action(this, ACT_WRITE, stack);

  return err;
}

//----------------------------------------

// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

Err CNodeAssignment::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_class = ancestor<CNodeClass>();
  auto node_func  = ancestor<CNodeFunction>();
  auto node_lhs   = child("lhs")->req<CNodeLValue>();
  auto node_op    = child("op");
  auto node_rhs   = child("rhs");
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

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeLValue::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto c : this) {
    err << c->trace(inst, stack);
  }
  return err;
}

CHECK_RETURN Err CNodeLValue::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

//------------------------------------------------------------------------------

uint32_t CNodeUsing::debug_color() const {
  return 0x00DFFF;
}

std::string_view CNodeUsing::get_name() const {
  return child("name")->get_text();
}

CHECK_RETURN Err CNodeUsing::emit(Cursor& cursor) {
  Err err;

  err << cursor.skip_over(this);

  err << cursor.emit_print("import ");
  err << cursor.emit_splice(child("name"));
  err << cursor.emit_print("::*;");

  return err;
}

CHECK_RETURN Err CNodeUsing::trace(CInstance* inst, call_stack& stack) {
  return Err();
};

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeFor::trace(CInstance* inst, call_stack& stack) {
  Err err;
  err << child("init")->trace(inst, stack);
  err << child("condition")->trace(inst, stack);
  err << child("body")->trace(inst, stack);
  err << child("step")->trace(inst, stack);
  return err;
}

CHECK_RETURN Err CNodeFor::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  for (auto child : this) {
    err << cursor.emit(child);
    if (child->node_next) err << cursor.emit_gap();
  }

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

CHECK_RETURN Err CNodeIf::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto body_true = child("body_true");

  auto body_false = child("body_false");

  if (auto exp_true = body_true->as<CNodeExpStatement>()) {
    if (auto node_call = exp_true->node_exp->as<CNodeCall>()) {
      auto node_name = node_call->child("func_name");
      if (auto node_field = node_name->as<CNodeFieldExpression>()) {
        return ERR("If branches that contain component calls must use {}.\n");
      }
    }
  }

  if (auto exp_false = body_false->as<CNodeExpStatement>()) {
    if (auto node_call = exp_false->node_exp->as<CNodeCall>()) {
      auto node_name = node_call->child("func_name");
      if (auto node_field = node_name->as<CNodeFieldExpression>()) {
        return ERR("Else branches that contain component calls must use {}.\n");
      }
    }
  }

  err << child("condition")->trace(inst, stack);

  auto root_inst = inst->get_root();

  root_inst->push_state();
  if (body_true) err << body_true->trace(inst, stack);
  root_inst->swap_state();
  if (body_false) err << body_false->trace(inst, stack);
  root_inst->merge_state();

  return err;
}

//----------------------------------------

CHECK_RETURN Err CNodeIf::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

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

void CNodeSwitch::init(const char* match_tag, SpanType span, uint64_t flags) {
  node_condition = child("condition")->req<CNodeList>();
  node_body = child("body")->req<CNode>();

  assert(node_condition);
  assert(node_body);
}

//----------------------------------------

CHECK_RETURN Err CNodeSwitch::trace(CInstance* inst, call_stack& stack) {
  Err err;

  err << node_condition->trace(inst, stack);

  auto root_inst = inst->get_root();

  int case_count = 0;
  bool has_default = false;

  for (auto node_child : node_body) {
    // Skip cases without bodies
    if (!node_child->child("body")) continue;

    auto node_case = node_child->as<CNodeCase>();
    auto node_default = node_child->as<CNodeDefault>();
    if (!node_case && !node_default) continue;

    if (node_default) has_default = true;

    root_inst->push_state();
    case_count++;
    err << node_child->trace(inst, stack);
    root_inst->swap_state();
  }

  if (has_default) {
    root_inst->pop_state();
    case_count--;
  }

  for (int i = 0; i < case_count; i++) {
    root_inst->merge_state();
  }

  return err;
}

//----------------------------------------

CHECK_RETURN Err CNodeSwitch::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

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

void CNodeCase::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_case  = child("case")->req<CNode>();
  node_cond  = child("condition")->req<CNode>();
  node_colon = child("colon");
  node_body  = child("body");
}

//----------------------------------------

bool ends_with_break(CNode* node) {
  if (!node) return false;
  if (node->get_text() == "break") return true;

  if (auto node_compound = node->as<CNodeCompound>()) {
    assert(node_compound->child_tail->tag_is("rdelim"));
    return ends_with_break(node_compound->child_tail->node_prev);
  }

  if (auto node_if = node->as<CNodeIf>()) {
    if (node_if->node_false == nullptr) return false;
    return ends_with_break(node_if->node_true) && ends_with_break(node_if->node_false);
  }

  if (auto node_list = node->as<CNodeList>()) {
    return ends_with_break(node_list->child_tail);
  }

  if (auto node_expstatement = node->as<CNodeExpStatement>()) {
    return ends_with_break(node_expstatement->node_exp);
  }

  //node->dump_parse_tree();
  //assert(false);

  return false;
}

//----------------------------------------

CHECK_RETURN Err CNodeCase::trace(CInstance* inst, call_stack& stack) {
  if (node_body && !ends_with_break(node_body)) {
    return ERR("All non-empty case statements must end with break (no fallthroughs)");
  }

  return node_body->trace(inst, stack);
}

//----------------------------------------

CHECK_RETURN Err CNodeCase::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

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

CHECK_RETURN Err CNodeDefault::trace(CInstance* inst, call_stack& stack) {
  return child("body")->trace(inst, stack);
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

Err CNodeCompound::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto c : this) err << c->trace(inst, stack);
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
    if (auto exp = child->as<CNodeExpStatement>()) {
      if (auto decl = exp->child("exp")->as<CNodeDeclaration>()) {

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
    }

    if (auto node_for = child->as<CNodeFor>()) {
      if (auto node_decl = node_for->child("init")->as<CNodeDeclaration>()) {
        auto name = node_decl->get_namestr();
        auto decl_type = node_decl->child("type");
        auto decl_name = node_decl->child("name");

        err << cursor.start_line();

        cursor.tok_cursor = decl_type->tok_begin();

        err << cursor.emit(decl_type);
        err << cursor.emit_gap();
        err << cursor.emit(decl_name);
        err << cursor.emit_print(";");

      }
    }

  }

  cursor.elide_type.pop();
  cursor.elide_value.pop();

  cursor.tok_cursor = old_cursor;

  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::trace(CInstance* inst, call_stack& stack) {
  Err err;

  if (auto node_value = child("value")) {
    err << node_value->trace(inst, stack);
  }

  auto inst_return = inst->resolve("@return");
  assert(inst_return);
  err << inst_return->log_action(this, ACT_WRITE, stack);

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
