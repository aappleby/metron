#include "CNodeCall.hpp"

#include "NodeTypes.hpp"

extern bool deep_trace;

//------------------------------------------------------------------------------

void CNodeCall::init(const char* match_tag, SpanType span, uint64_t flags) {
  node_name  = child("func_name")->req<CNode>();
  node_targs = child("func_targs")->opt<CNodeList>();
  node_args  = child("func_args")->req<CNodeList>();
}

//------------------------------------------------------------------------------

std::string_view CNodeCall::get_name() const {
  return child("func_name")->get_text();
}

//------------------------------------------------------------------------------

uint32_t CNodeCall::debug_color() const {
  return COL_SKY;
}

//------------------------------------------------------------------------------
// FIXME not dealing with nested submod paths right now

Err CNodeCall::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto src_class = ancestor<CNodeClass>();

  err << node_args->trace(inst, stack);

  auto dst_name = node_name;

  if (auto field_exp = dst_name->as<CNodeFieldExpression>()) {
    auto submod_name = field_exp->child("field_path")->get_textstr();
    auto submod_field = src_class->get_field(submod_name);
    auto func_inst = inst->resolve(field_exp)->as<CInstFunc>();
    auto node_func = func_inst->node_func;

    stack.push_back(func_inst->node_func);
    for (auto child : func_inst->params) {
      err << child->log_action(this, ACT_WRITE, stack);
    }

    if (deep_trace) {
      err << node_func->trace(func_inst, stack);
    }


    if (func_inst->inst_return) {
      err << func_inst->inst_return->log_action(this, ACT_READ, stack);
    }
    stack.pop_back();
  }
  else {
    auto func_name = dst_name->get_textstr();
    auto dst_func = src_class->get_function(func_name);

    auto func_inst = inst->resolve(func_name)->as<CInstFunc>();

    if (dst_func && func_inst) {
      stack.push_back(dst_func);
      err << dst_func->trace(func_inst, stack);
      stack.pop_back();
    }
  }

  return err;
}

//------------------------------------------------------------------------------

bool CNodeCall::is_bit_extract() {
  if (auto func_id = node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_textstr();

    auto args = node_args->items.size();

    if (func_name.size() == 2) {
      if (func_name == "bx") {
        return true;
      }
      else if (func_name[0] == 'b' && isdigit(func_name[1])) {
        return true;
      }
    }
    else if (func_name.size() == 3) {
      if (func_name[0] == 'b' && isdigit(func_name[1]) && isdigit(func_name[2])) {
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------

CHECK_RETURN Err CNodeCall::emit_bit_extract(Cursor& cursor) {
  Err err;

  auto func_name = node_name->get_textstr();

  auto& args = node_args->items;
  assert(args.size() == 1 || args.size() == 2);

  //----------------------------------------

  CNode* node_exp = args[0];
  bool bare_exp = false;
  bare_exp |= (node_exp->as<CNodeIdentifier>() != nullptr);
  bare_exp |= (node_exp->as<CNodeConstInt>() != nullptr);
  bare_exp |= (node_exp->as<CNodeFieldExpression>() != nullptr);

  //----------

  int    width      = 0;
  CNode* node_width = nullptr;
  bool   bare_width = false;

  if (func_name == "bx") {
    auto& targs = node_targs->items;
    assert(targs.size() == 1);

    if (auto const_width = targs[0]->as<CNodeConstInt>()) {
      width = atoi(const_width->get_textstr().c_str());
    }
    else {
      node_width = targs[0];
    }
  }
  else {
    width = atoi(&func_name[1]);
    bare_width = true;
  }

  bare_width |= (node_width->as<CNodeIdentifier>() != nullptr);
  bare_width |= (node_width->as<CNodeConstInt>() != nullptr);

  //----------

  int    offset = 0;
  CNode* node_offset = nullptr;

  if (args.size() == 2) {
    if (auto const_offset = args[1]->as<CNodeConstInt>()) {
      offset = atoi(const_offset->get_textstr().c_str());
    }
    else {
      node_offset = args[1];
    }
  }
  bool bare_offset = false;
  bare_offset |= (node_offset->as<CNodeIdentifier>() != nullptr);
  bare_offset |= (node_offset->as<CNodeConstInt>() != nullptr);

  //----------------------------------------

  err << cursor.skip_over(this);

  //----------------------------------------
  // Handle casting DONTCARE

  if (node_exp->get_text() == "DONTCARE") {
    //err << cursor.skip_over(this);

    if (node_width) {
      if (!bare_width) err << cursor.emit_print("(");
      err << cursor.emit_splice(node_width);
      if (!bare_width) err << cursor.emit_print(")");
      err << cursor.emit_print("'('x)");
    }
    else {
      err << cursor.emit_print("%d'bx", width);
    }

    return err;
  }

  //----------------------------------------
  // Size cast with no offsets

  if (offset == 0 && node_offset == nullptr) {

    if (bare_width && bare_exp && !node_width) {
      if (node_exp->as<CNodeConstInt>()) {
        cursor.override_size.push(width);
        err << cursor.emit_splice(node_exp);
        cursor.override_size.pop();
        return err;
      }
    }


    if (!bare_width) err << cursor.emit_print("(");
    if (node_width)  err << cursor.emit_splice(node_width);
    else             err << cursor.emit_print("%d", width);
    if (!bare_width) err << cursor.emit_print(")");

    err<< cursor.emit_print("'");
    err << cursor.emit_print("(");
    err << cursor.emit_splice(node_exp);
    err << cursor.emit_print(")");

    return err;
  }

  //----------------------------------------
  // Expression

  if (!bare_exp) err << cursor.emit_print("(");
  err << cursor.emit_splice(node_exp);
  if (!bare_exp) err << cursor.emit_print(")");

  //----------
  // Single bit extract

  if (width == 1) {
    err << cursor.emit_print("[");
    if (node_offset) {
      err << cursor.emit_splice(node_offset);
    }
    else {
      err << cursor.emit_print("%d", offset);
    }
    err << cursor.emit_print("]");
    return err;
  }

  //----------

  if (node_width && node_offset) {
    err << cursor.emit_print("[");
    if (!bare_width) err << cursor.emit_print("(");
    err << cursor.emit_splice(node_width);
    if (!bare_width) err << cursor.emit_print(")");
    err << cursor.emit_print("+");
    if (!bare_offset) err << cursor.emit_print("(");
    err << cursor.emit_splice(node_offset);
    if (!bare_offset) err << cursor.emit_print(")");
    err << cursor.emit_print("-1");
    err << cursor.emit_print(":");
    if (!bare_offset) err << cursor.emit_print("(");
    err << cursor.emit_splice(node_offset);
    if (!bare_offset) err << cursor.emit_print(")");
    err << cursor.emit_print("]");
    return err;
  }

  //----------

  else if (node_width) {
    err << cursor.emit_print("[");
    if (!bare_width) err << cursor.emit_print("(");
    err << cursor.emit_splice(node_width);
    if (!bare_width) err << cursor.emit_print(")");

    if (offset == 0) {
      err << cursor.emit_print("-1");
    }
    else if (offset == 1) {
    }
    else {
      err << cursor.emit_print("+");
      err << cursor.emit_print("%d", offset-1);
    }

    err << cursor.emit_print(":");
    err << cursor.emit_print("%d", offset);
    err << cursor.emit_print("]");
    return err;
  }

  //----------

  else if (node_offset) {
    err << cursor.emit_print("[");
    err << cursor.emit_print("%d", width - 1);
    err << cursor.emit_print("+");
    if (!bare_offset) err << cursor.emit_print("(");
    err << cursor.emit_splice(node_offset);
    if (!bare_offset) err << cursor.emit_print(")");
    err << cursor.emit_print(":");
    if (!bare_offset) err << cursor.emit_print("(");
    err << cursor.emit_splice(node_offset);
    if (!bare_offset) err << cursor.emit_print(")");
    err << cursor.emit_print("]");
    return err;
  }

  //----------

  else {
    err << cursor.emit_print("[%d:%d]", width+offset-1, offset);
    return err;
  }
}

//------------------------------------------------------------------------------

Err CNodeCall::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  if (is_bit_extract()) return emit_bit_extract(cursor);

  auto src_class = ancestor<CNodeClass>();
  auto src_func = ancestor<CNodeFunction>();

  if (auto func_path = node_name->as<CNodeFieldExpression>()) {
    // We don't actually "call" into submodules, so we can just comment this
    // call out.

    auto field_name = func_path->node_path->get_text();
    auto func_name  = func_path->node_name->get_text();

    auto src_field = src_class->get_field(field_name);
    auto dst_class = src_class->repo->get_class(src_field->get_type_name());
    auto dst_func  = dst_class->get_function(func_name);

    auto rtype = dst_func->child("return_type");

    if (rtype->get_text() == "void") {
      err << cursor.comment_out(this);
      return err << cursor.check_done(this);
    }
    else {
      err << cursor.emit_replacement(this,
        "%.*s_%.*s_ret",
        field_name.size(), field_name.data(),
        func_name.size(), func_name.data());
      return err << cursor.check_done(this);
    }
  }


  if (auto func_id = node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_textstr();

    if (cursor.id_map.top().contains(func_name)) {
      auto replacement = cursor.id_map.top()[func_name];

      err << cursor.emit_replacement(node_name, "%s", replacement.c_str());
      err << cursor.emit_gap();

      if (node_targs) {
        err << cursor.skip_over(node_targs);
        err << cursor.skip_gap();
      }

      err << cursor.emit(node_args);

      return err << cursor.check_done(this);
      //return cursor.emit_default(this);
    }

    if (func_name == "cat") {
      err << cursor.skip_to(node_args);
      for (auto child : node_args) {
        if (child->tag_is("ldelim")) {
          err << cursor.emit_replacement(child, "{");
          err << cursor.emit_gap();
        }
        else if (child->tag_is("rdelim")) {
          err << cursor.emit_replacement(child, "}");
        }
        else {
          err << cursor.emit(child);
          if (child->node_next) err << cursor.emit_gap();
        }
      }
      return err << cursor.check_done(this);
    }

    //----------
    // Convert "dup<15>(x)" to "{15 {x}}"

    if (func_name == "dup") {
      auto node_val = child("func_targs")->child("arg");
      auto node_exp = child("func_args")->child("exp");

      err << cursor.skip_over(this);

      err << cursor.emit_print("{");
      err << cursor.emit_splice(node_val);
      err << cursor.emit_print(" {");
      err << cursor.emit_splice(node_exp);
      err << cursor.emit_print("}}");

      return err;
    }


    if (func_name == "sra") {
      auto args = child("func_args")->as<CNodeList>();
      auto lhs  = args->items[0];
      auto rhs  = args->items[1];

      err << cursor.skip_over(this);

      err << cursor.emit_print("($signed(");
      err << cursor.emit_splice(lhs);
      err << cursor.emit_print(") >>> ");
      err << cursor.emit_splice(rhs);
      err << cursor.emit_print(")");

      return err;
    }


    //----------
    // Not a special builtin call

    auto dst_func = src_class->get_function(func_id->get_text());
    auto dst_params = dst_func->child("params");

    // Replace call with return binding variable if the callee is a tock

    if (dst_func->method_type == MT_TOCK) {
      auto dst_name = dst_func->get_namestr();
      err << cursor.skip_over(this);
      err << cursor.emit_print("%s_ret", dst_name.c_str());
      return err;
    }

    auto src_mtype = src_func->method_type;
    auto dst_mtype = dst_func->method_type;

    if (dst_mtype == MT_INIT) {
      assert(src_mtype == MT_INIT);
      return cursor.emit_default(this);
    }

    if (dst_mtype == MT_FUNC) {
      return cursor.emit_default(this);
    }

    if (src_mtype == MT_TICK && dst_mtype == MT_TICK) {
      return cursor.emit_default(this);
    }

    if (src_mtype == MT_TOCK && (dst_mtype == MT_TOCK || dst_mtype == MT_TICK)) {
      err << cursor.comment_out(this);

      auto param = dst_params->child_head;
      auto arg = node_args->child_head;

      bool first_arg = true;

      while (param && arg) {

        auto punct_a = param->as<CNodePunct>() != nullptr;
        auto punct_b = arg->as<CNodePunct>() != nullptr;
        assert(punct_a == punct_b);
        if (punct_a) {
          param = param->node_next;
          arg = arg->node_next;
          continue;
        }

        if (!first_arg) err << cursor.emit_print(";");

        err << cursor.start_line();
        err << cursor.emit_print("%s_%s = %s",
          dst_func->get_namestr().c_str(),
          param->child("name")->get_textstr().c_str(),
          arg->get_textstr().c_str());

        first_arg = false;

        param = param->node_next;
        arg = arg->node_next;
      }

      assert(param == nullptr);
      assert(arg == nullptr);
      return err << cursor.check_done(this);
    }
  }

  if (auto func_path = node_name->as<CNodeFieldExpression>()) {
    auto field_name = func_path->child("field_path")->get_text();
    auto func_name  = func_path->child("identifier")->get_text();

    auto src_class = ancestor<CNodeClass>();
    auto field = src_class->get_field(field_name);
    auto dst_class = field->node_decl->_type_class;
    auto dst_func = dst_class->get_function(func_name);

    if (dst_func->child("return_type")->get_text() == "void") {
      err << cursor.comment_out(this);
    }
    else {
      err << cursor.skip_over(this);
      err << cursor.emit_print("%.*s_%.*s_ret", field_name.size(), field_name.data(), func_name.size(), func_name.data());
    }

    return err << cursor.check_done(this);
  }


  err << CNode::emit(cursor);
  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
// Call expressions turn into either Verilog calls, or are replaced with binding
// statements.

bool CNodeCall::can_omit_call() {

  // Calls into submodules always turn into bindings.
  if (auto func_path = node_name->as<CNodeFieldExpression>()) {
    return true;
  }

  // Calls into methods in the same module turn into bindings if needed.
  auto src_class = ancestor<CNodeClass>();
  auto dst_func = src_class->get_function(node_name->get_text());
  if (dst_func && dst_func->needs_binding()) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
