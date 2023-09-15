#include "CNodeCall.hpp"

#include "NodeTypes.hpp"

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

Err CNodeCall::trace(CCall* call) {
  Err err;
  auto src_inst  = call->inst_class;
  auto src_class = src_inst->node_class;

  err << node_args->trace(call);

  auto dst_name = node_name;

  if (auto field_exp = dst_name->as<CNodeFieldExpression>()) {
    auto dst_inst  = src_inst->inst_map[field_exp->child("field_path")->get_text()]->as<CInstClass>();
    auto dst_class = dst_inst->node_class;
    auto dst_func  = dst_class->get_function(field_exp->child("identifier")->get_text());

    auto dst_call  = new CCall(dst_inst, this, dst_func);
    call->call_map[this] = dst_call;
    err << dst_func->trace(dst_call);
  }
  else {
    auto dst_func = src_class->get_function(dst_name->get_text());
    if (dst_func) {
      auto dst_call = new CCall(src_inst, this, dst_func);
      call->call_map[this] = dst_call;
      err << dst_func->trace(dst_call);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

/*

CHECK_RETURN Err MtCursor::emit_sym_call_expression(MnNode n) {
  Err err = check_at(sym_call_expression, n);

  err << emit_ws_to(n);

  MnNode func = n.get_field(field_function);
  MnNode args = n.get_field(field_arguments);

  std::string func_name = func.name4();

  //----------
  // Handle bN

  if (func_name[0] == 'b' && func_name.size() == 2) {
    auto x = func_name[1];
    if (x >= '0' && x <= '9') {
      return emit_static_bit_extract(n, x - '0');
    }
  }

  if (func_name[0] == 'b' && func_name.size() == 3) {
    auto x = func_name[1];
    auto y = func_name[2];
    if (x >= '0' && x <= '9' && y >= '0' && y <= '9') {
      return emit_static_bit_extract(n, (x - '0') * 10 + (y - '0'));
    }
  }

  //----------

  if (func_name == "bx") {
    // Bit extract.
    auto template_arg = func.get_field(field_arguments).named_child(0);
    err << emit_ws_to(n);
    err << emit_dynamic_bit_extract(n, template_arg);
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Remove "cat" and replace parens with brackets

  if (func_name == "cat") {
    err << skip_over(func);
    for (const auto& arg : (MnNode&)args) {
      err << emit_ws_to(arg);

      if (arg.sym == anon_sym_LPAREN) {
        err << emit_replacement(arg, "{");
      }
      else if(arg.sym == anon_sym_RPAREN) {
        err << emit_replacement(arg, "}");
      }
      else {
        err << emit_dispatch(arg);
      }
    }
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Convert "dup<15>(x)" to "{15 {x}}"

  if (func_name == "dup") {

    // printf("{%d {%s}}", call.func.args[0], call.args[0]); ???

    if (args.named_child_count() != 1) return err << ERR("dup() had too many children\n");

    err << skip_over(func);

    auto template_arg = func.get_field(field_arguments).named_child(0);
    int dup_count = atoi(template_arg.start());
    auto func_arg = args.named_child(0);
    err << emit_print("{%d {", dup_count);
    err << emit_splice(func_arg);
    err << emit_print("}}");
    cursor = n.end();
    return err << check_done(n);
  }

  //----------

  if (func_name == "sra") {
    auto lhs = args.named_child(0);
    auto rhs = args.named_child(1);

    err << emit_print("($signed(");
    err << emit_splice(lhs);
    err << emit_print(") >>> ");
    err << emit_splice(rhs);
    err << emit_print(")");
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Component method call.

  if (func.sym == sym_field_expression) {

    auto node_component = n.get_field(field_function).get_field(field_argument);

    auto dst_component = current_mod.top()->get_field(node_component.text());
    auto dst_mod = lib->get_module(dst_component->type_name());
    if (!dst_mod) return err << ERR("dst_mod null\n");
    auto node_func = n.get_field(field_function).get_field(field_field);
    auto dst_method = dst_mod->get_method(node_func.text());

    if (!dst_method) return err << ERR("dst_method null\n");

    err << emit_print("%s_%s_ret", node_component.text().c_str(), dst_method->cname());
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Builtins

  string_to_string renames = {
    {"signed",         "$signed"},
    {"unsigned",       "$unsigned"},
    {"clog2",          "$clog2" },
    {"pow2",           "2**" },
    {"readmemh",       "$readmemh" },
    {"value_plusargs", "$value$plusargs" },
    {"write",          "$write" },
    {"sign_extend",    "$signed" },
    {"zero_extend",    "$unsigned" },
  };

  if (auto it = renames.find(func_name); it != renames.end()) {
    err << emit_replacement(func, (*it).second.c_str());
    err << emit_dispatch(args);
    cursor = n.end();
    return err << check_done(n);
  }

  //----------
  // Internal method call.

  auto method = current_mod.top()->get_method(func_name);
  if (method->needs_binding) {
    err << emit_replacement(n, "%s_ret", method->cname());
  }
  else {
    err << emit_children(n);
  }

  cursor = n.end();
  return err << check_done(n);
}
*/

Err CNodeCall::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  dump_debug();

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

    if (func_name == "bx") {
      // Bit extract.
      err << cursor.emit_print("(");
      err << cursor.emit_splice(node_targs->items[0]);
      err << cursor.emit_print(")'");
      err << cursor.skip_to(node_args);
      err << cursor.emit(node_args);
      return err << cursor.check_done(this);
    }

    if (func_name[0] == 'b' && func_name.size() == 2) {
      auto x = func_name[1];
      if (x >= '0' && x <= '9') {
        int width = (x - '0');
        err << cursor.emit_print("%d'", width);
        err << cursor.skip_to(node_args);
        err << cursor.emit(node_args);
        return err << cursor.check_done(this);
      }
    }

    if (func_name[0] == 'b' && func_name.size() == 3) {
      auto x = func_name[1];
      auto y = func_name[2];
      if (x >= '0' && x <= '9' && y >= '0' && y <= '9') {
        int width = (x - '0') * 10 + (y - '0');
        err << cursor.emit_print("%d'", width);
        err << cursor.skip_to(node_args);
        err << cursor.emit(node_args);
        return err << cursor.check_done(this);
      }
    }

    if (func_name == "cat") {
      err << cursor.skip_to(node_args);
      for (auto child : node_args) {
        if (child->tag_is("ldelim")) {
          err << cursor.emit_replacement(child, "{");
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

    //----------
    // Not a special builtin call

    auto dst_func = src_class->get_function(func_id->get_text());
    auto dst_params = dst_func->child("params");

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
