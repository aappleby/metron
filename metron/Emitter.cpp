#include "Emitter.hpp"

#include "metron/nodes/CNodeReturn.hpp"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeAssignment.hpp"
#include "metron/nodes/CNodeBuiltinType.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeConstant.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeDoWhile.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeExpStatement.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeFor.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeIf.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeReturn.hpp"
#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeSwitch.hpp"
#include "metron/nodes/CNodeTemplate.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeWhile.hpp"
#include "metron/nodes/CNodePreproc.hpp"
#include "metron/nodes/CNodeQualifiedIdentifier.hpp"
#include "metron/nodes/CNodeTranslationUnit.hpp"
#include "metron/nodes/CNodeUsing.hpp"

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
  auto node_func = node->ancestor<CNodeFunction>();
  auto node_lhs = node->child("lhs")->req<CNodeLValue>();
  auto node_op = node->child("op");
  auto node_rhs = node->child("rhs");
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
  } else {
    auto lhs_text = node_lhs->get_text();

    err << cursor.skip_over(node_op);
    err << cursor.emit_print("=");
    err << cursor.emit_gap();
    err << cursor.emit_print("%.*s %c ", lhs_text.size(), lhs_text.data(),
                             node_op->get_text()[0]);
  }

  err << cursor.emit(node_rhs);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeBuiltinType* node) {
  Err err = cursor.check_at(node);

  auto node_name = node->child("name");
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

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeCall* node) {
  Err err = cursor.check_at(node);

  if (node->is_bit_extract()) return node->emit_bit_extract(cursor);

  auto src_class = node->ancestor<CNodeClass>();
  auto src_func = node->ancestor<CNodeFunction>();

  if (auto func_path = node->node_name->as<CNodeFieldExpression>()) {
    // We don't actually "call" into submodules, so we can just comment this
    // call out.

    auto field_name = func_path->node_path->get_text();
    auto func_name = func_path->node_name->get_text();

    auto src_field = src_class->get_field(field_name);
    auto dst_class = src_class->repo->get_class(src_field->get_type_name());
    auto dst_func = dst_class->get_function(func_name);

    auto rtype = dst_func->child("return_type");

    if (rtype->get_text() == "void") {
      err << cursor.comment_out(node);
      return err << cursor.check_done(node);
    } else {
      err << cursor.emit_replacement(node, "%.*s_%.*s_ret", field_name.size(),
                                     field_name.data(), func_name.size(),
                                     func_name.data());
      return err << cursor.check_done(node);
    }
  }

  if (auto func_id = node->node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_textstr();

    if (cursor.id_map.top().contains(func_name)) {
      auto replacement = cursor.id_map.top()[func_name];

      err << cursor.emit_replacement(node->node_name, "%s",
                                     replacement.c_str());
      err << cursor.emit_gap();

      if (node->node_targs) {
        err << cursor.skip_over(node->node_targs);
        err << cursor.skip_gap();
      }

      err << cursor.emit(node->node_args);

      return err << cursor.check_done(node);
      // return cursor.emit_default(this);
    }

    if (func_name == "cat") {
      err << cursor.skip_to(node->node_args);
      for (auto child : node->node_args) {
        if (child->tag_is("ldelim")) {
          err << cursor.emit_replacement(child, "{");
          err << cursor.emit_gap();
        } else if (child->tag_is("rdelim")) {
          err << cursor.emit_replacement(child, "}");
        } else {
          err << cursor.emit(child);
          if (child->node_next) err << cursor.emit_gap();
        }
      }
      return err << cursor.check_done(node);
    }

    //----------
    // Convert "dup<15>(x)" to "{15 {x}}"

    if (func_name == "dup") {
      auto node_val = node->child("func_targs")->child("arg");
      auto node_exp = node->child("func_args")->child("exp");

      err << cursor.skip_over(node);

      err << cursor.emit_print("{");
      err << cursor.emit_splice(node_val);
      err << cursor.emit_print(" {");
      err << cursor.emit_splice(node_exp);
      err << cursor.emit_print("}}");

      return err;
    }

    if (func_name == "sra") {
      auto args = node->child("func_args")->as<CNodeList>();
      auto lhs = args->items[0];
      auto rhs = args->items[1];

      err << cursor.skip_over(node);

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
      err << cursor.skip_over(node);
      err << cursor.emit_print("%s_ret", dst_name.c_str());
      return err;
    }

    auto src_mtype = src_func->method_type;
    auto dst_mtype = dst_func->method_type;

    if (dst_mtype == MT_INIT) {
      assert(src_mtype == MT_INIT);
      return cursor.emit_default(node);
    }

    if (dst_mtype == MT_FUNC) {
      return cursor.emit_default(node);
    }

    if (src_mtype == MT_TICK && dst_mtype == MT_TICK) {
      return cursor.emit_default(node);
    }

    if (src_mtype == MT_TOCK &&
        (dst_mtype == MT_TOCK || dst_mtype == MT_TICK)) {
      err << cursor.comment_out(node);

      auto param = dst_params->child_head;
      auto arg = node->node_args->child_head;

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
        err << cursor.emit_print("%s_%s = %s", dst_func->get_namestr().c_str(),
                                 param->child("name")->get_textstr().c_str(),
                                 arg->get_textstr().c_str());

        first_arg = false;

        param = param->node_next;
        arg = arg->node_next;
      }

      assert(param == nullptr);
      assert(arg == nullptr);
      return err << cursor.check_done(node);
    }
  }

  if (auto func_path = node->node_name->as<CNodeFieldExpression>()) {
    auto field_name = func_path->child("field_path")->get_text();
    auto func_name = func_path->child("identifier")->get_text();

    auto src_class = node->ancestor<CNodeClass>();
    auto field = src_class->get_field(field_name);
    auto dst_class = field->node_decl->_type_class;
    auto dst_func = dst_class->get_function(func_name);

    if (dst_func->child("return_type")->get_text() == "void") {
      err << cursor.comment_out(node);
    } else {
      err << cursor.skip_over(node);
      err << cursor.emit_print("%.*s_%.*s_ret", field_name.size(),
                               field_name.data(), func_name.size(),
                               func_name.data());
    }

    return err << cursor.check_done(node);
  }

  assert(false);
  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeClass* node) {
  Err err = cursor.check_at(node);

  auto node_class = node->child<CNodeKeyword>();
  auto node_name = node->child("name");
  auto node_body = node->child("body");
  auto node_semi = node->child("semi");

  err << cursor.emit_replacements(node_class, "class", "module");
  err << cursor.emit_gap();
  err << cursor.emit(node_name);
  err << cursor.emit_gap();

  err << cursor.emit_print("(");
  cursor.indent_level++;
  err << node->emit_module_ports(cursor);
  cursor.indent_level--;
  err << cursor.start_line();
  err << cursor.emit_print(");");

  for (auto child : node_body) {
    if (child->get_text() == "{") {
      cursor.indent_level++;
      err << cursor.skip_over(child);
      err << node->emit_template_parameter_list(cursor);
    } else if (child->get_text() == "}") {
      cursor.indent_level--;
      err << cursor.emit_replacement(child, "endmodule");
    } else {
      err << cursor.emit(child);
    }
    if (child->node_next) err << cursor.emit_gap();
  }

  err << cursor.skip_gap();
  err << cursor.skip_over(node_semi);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeClassType* node) {
  Err err;
  auto targs = node->child("template_args");

  if (targs) {
    err << cursor.emit(node->child("name"));
    err << cursor.emit_gap();
    err << cursor.skip_over(targs);
  } else {
    err << cursor.emit_default(node);
  }
  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeCompound* node) {
  return node->emit_block(cursor, "begin", "end");
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeConstant* node) {
  Err err = cursor.check_at(node);

  std::string body = node->get_textstr();

  // FIXME what was this for?
  // This was for forcing enum constants to the size of the enum type
  int size_cast = cursor.override_size.top();
  // int size_cast = 0;

  // Count how many 's are in the number
  int spacer_count = 0;
  int prefix_count = 0;

  for (auto& c : body)
    if (c == '\'') {
      c = '_';
      spacer_count++;
    }

  if (body.starts_with("0x")) {
    prefix_count = 2;
    if (!size_cast) size_cast = ((int)body.size() - 2 - spacer_count) * 4;
    err << cursor.emit_print("%d'h", size_cast);
  } else if (body.starts_with("0b")) {
    prefix_count = 2;
    if (!size_cast) size_cast = (int)body.size() - 2 - spacer_count;
    err << cursor.emit_print("%d'b", size_cast);
  } else {
    if (size_cast) {
      err << cursor.emit_print("%d'd", size_cast);
    }
  }

  err << cursor.emit_print("%s", body.c_str() + prefix_count);

  cursor.gap_emitted = false;
  cursor.tok_cursor = node->tok_end();

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeConstructor* node) {
  assert(false);
  return Err();
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeDeclaration* node) {
  Err err = cursor.check_at(node);

  // Check for const char*
  if (node->node_const) {
    if (node->node_type->child("name")->get_text() == "char") {
      if (node->node_type->child("star")) {
        return cursor.emit_replacement(node, "{{const char*}}");
      }
    }

    err << cursor.emit_print("parameter ");

    for (auto c = node->child_head; c; c = c->node_next) {
      if (c->as<CNodeType>()) {
        err << cursor.skip_over(c);
        if (c->node_next) err << cursor.skip_gap();
      } else {
        err << cursor.emit(c);
        if (c->node_next) err << cursor.emit_gap();
      }
    }

    return err << cursor.check_done(node);
  }

  for (auto child : node) {
    if (cursor.elide_type.top()) {
      if (child->as<CNodeType>()) {
        err << cursor.skip_over(child);
        if (child->node_next) err << cursor.skip_gap();
        continue;
      }
    }

    err << cursor.emit(child);
    if (child->node_next) err << cursor.emit_gap();
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeDoWhile* node) {
  assert(false);
  return Err();
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeEnum* node) {
  Err err = cursor.check_at(node);

  // Extract enum bit width, if present.
  cursor.override_size.push(32);
  if (node->node_type) {
    if (auto targs = node->node_type->child("template_args")->as<CNodeList>()) {
      cursor.override_size.top() = atoi(targs->items[0]->text_begin());
    }
  }

  if (!node->node_decl) {
    err << cursor.emit_print("typedef ");
  }

  err << cursor.emit(node->node_enum);
  err << cursor.emit_gap();

  if (node->node_class) {
    err << cursor.skip_over(node->node_class);
    err << cursor.skip_gap();
  }

  if (node->node_name) {
    err << cursor.skip_over(node->node_name);
    err << cursor.skip_gap();
  }

  if (node->node_colon) {
    err << cursor.skip_over(node->node_colon);
    err << cursor.skip_gap();
    err << cursor.emit(node->node_type);
    err << cursor.emit_gap();
  }

  err << cursor.emit(node->node_body);
  err << cursor.emit_gap();

  if (node->node_decl) {
    err << cursor.emit(node->node_decl);
    err << cursor.emit_gap();
  }

  if (node->node_name) {
    err << cursor.emit_print(" ");
    err << cursor.emit_splice(node->node_name);
  }

  err << cursor.emit(node->node_semi);

  cursor.override_size.pop();

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeEnumerator* node) { return cursor.emit_default(node); }

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeEnumType* node) { return cursor.emit_default(node); }

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeExpression* node) { return cursor.emit_default(node); }

//------------------------------------------------------------------------------

Err Emitter::emit(CNodePrefixExp* node) {
  Err err = cursor.check_at(node);

  auto node_op = node->child("prefix");
  auto op = node_op->get_text();

  if (op != "++" && op != "--") {
    return cursor.emit_default(node);
  }

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func = node->ancestor<CNodeFunction>();
  auto node_rhs = node->child("rhs");
  auto node_field = node_class->get_field(node_rhs);

  err << cursor.skip_over(node);
  err << cursor.emit_splice(node_rhs);
  if (node_func->method_type == MT_TICK && node_field) {
    err << cursor.emit_print(" <= ");
  } else {
    err << cursor.emit_print(" = ");
  }
  err << cursor.emit_splice(node_rhs);
  if (op == "++") {
    err << cursor.emit_print(" + 1");
  } else {
    err << cursor.emit_print(" - 1");
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeSuffixExp* node) {
  Err err = cursor.check_at(node);

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func = node->ancestor<CNodeFunction>();
  auto node_op = node->child("suffix");
  auto node_lhs = node->child("lhs");
  auto node_field = node_class->get_field(node_lhs);

  auto op = node_op->get_text();

  if (op == "++" || op == "--") {
    err << cursor.skip_over(node);
    err << cursor.emit_splice(node_lhs);
    if (node_func->method_type == MT_TICK && node_field) {
      err << cursor.emit_print(" <= ");
    } else {
      err << cursor.emit_print(" = ");
    }
    err << cursor.emit_splice(node_lhs);
    if (op == "++") {
      err << cursor.emit_print(" + 1");
    } else {
      err << cursor.emit_print(" - 1");
    }
  } else {
    err << cursor.emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeOperator* node) { return cursor.emit_default(node); }

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeExpStatement* node) {
  Err err = cursor.check_at(node);

  CNode* node_exp = node->child("exp");
  CNode* node_semi = node->child("semi");

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
      err << cursor.comment_out(node);
      return err << cursor.check_done(node);
    }
  }

  if (auto keyword = node_exp->as<CNodeKeyword>()) {
    if (keyword->get_text() == "break") {
      err << cursor.comment_out(node);
      return err << cursor.check_done(node);
    }
  }

  if (auto decl = node_exp->as<CNodeDeclaration>()) {
    if (cursor.elide_type.top()) {
      if (decl->node_value == nullptr) {
        err << cursor.skip_over(node);
        return err << cursor.check_done(node);
      }
    }
  }

  err << cursor.emit_default(node);
  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeField* node) {
  Err err = cursor.check_at(node);

  //----------------------------------------
  // Ports don't go in the class body.
  // FIXME should we disallow public components?

  if (node->is_public && !node->is_component() &&
      !node->node_decl->is_localparam()) {
    return err << cursor.skip_over(node);
  }

  //----------------------------------------

  if (node->is_const_char()) {
    err << cursor.emit_print("localparam string ");
    err << cursor.skip_to(node->node_decl->node_name);
    err << cursor.emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << cursor.emit(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node->node_decl->node_eq);
    err << cursor.emit_gap();
    err << cursor.emit(node->node_decl->node_value);
    err << cursor.emit_gap();

    err << cursor.emit(node->node_semi);

    return err << cursor.check_done(node);
  }

  //----------------------------------------

  if (node->node_decl->node_const) {
    // Localparam
    bool in_namespace = node->ancestor<CNodeNamespace>() != nullptr;
    err << cursor.emit_print(in_namespace ? "parameter " : "localparam ");

    if (node->node_decl->node_static) {
      err << cursor.comment_out(node->node_decl->node_static);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_const) {
      err << cursor.comment_out(node->node_decl->node_const);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node->node_decl->node_type);
    err << cursor.emit_gap();

    err << cursor.emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << cursor.emit(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_eq) {
      err << cursor.emit(node->node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_value) {
      err << cursor.emit(node->node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node->node_semi);
    return err << cursor.check_done(node);
  }

  //----------------------------------------

  auto node_builtin = node->node_decl->node_type->as<CNodeBuiltinType>();
  auto node_targs = node->node_decl->node_type->child("template_args");

  if (node_builtin && node_targs) {
    err << cursor.skip_to(node->node_decl->node_type);
    err << cursor.emit(node->node_decl->node_type);
    err << cursor.emit_gap();

    err << cursor.emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << cursor.emit(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_eq) {
      err << cursor.emit(node->node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_value) {
      err << cursor.emit(node->node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node->node_semi);

    return err << cursor.check_done(node);
  }

  //----------------------------------------

  if (node_builtin) {
    err << cursor.skip_to(node->node_decl->node_type);
    err << cursor.emit(node->node_decl->node_type);
    err << cursor.emit_gap();

    err << cursor.emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << cursor.emit(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_eq) {
      err << cursor.emit(node->node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_value) {
      err << cursor.emit(node->node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << cursor.emit(node->node_semi);
    return err << cursor.check_done(node);
  }

  //----------------------------------------

  if (node->is_struct()) {
    return cursor.emit_raw(node);
  }

  //----------------------------------------

  if (node->is_component()) {
    return node->emit_component(cursor);
  }

  //----------------------------------------

  assert(false);
  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeFieldExpression* node) {
  Err err = cursor.check_at(node);

  auto node_func = node->ancestor<CNodeFunction>();
  auto node_class = node->ancestor<CNodeClass>();
  auto node_path = node->child("field_path");
  auto node_name = node->child("identifier");

  auto field = node_class->get_field(node_path->get_text());

  if (field && field->node_decl->_type_struct) {
    err << cursor.emit_default(node);
    return err;
  }

  // FIXME this needs the submod_ prefix for submod ports

  if (field) {
    auto field = node->get_textstr();
    for (auto& c : field) {
      if (c == '.') c = '_';
    }
    err << cursor.emit_replacement(node, field.c_str());
  } else {
    err << cursor.emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeFor* node) { return cursor.emit_default(node); }

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeFunction* node) {
  Err err = cursor.check_at(node);

  // FIXME this should happen somewhere else
  bool called_by_init = false;
  bool called_by_tick = false;
  bool called_by_tock = false;
  bool called_by_func = false;

  node->visit_internal_callers([&](CNodeFunction* f) {
    if (f->method_type == MT_INIT) called_by_init = true;
    if (f->method_type == MT_TICK) called_by_tick = true;
    if (f->method_type == MT_TOCK) called_by_tock = true;
    if (f->method_type == MT_FUNC) called_by_func = true;
  });

  //----------

  if (node->method_type == MT_INIT) {
    err << (node->as<CNodeConstructor>() ? node->emit_init(cursor)
                                         : node->emit_task(cursor));
  } else if (node->method_type == MT_TOCK) {
    err << node->emit_always_comb(cursor);
  } else if (node->method_type == MT_TICK) {
    err << (called_by_tick ? node->emit_task(cursor)
                           : node->emit_always_ff(cursor));
  } else if (node->method_type == MT_FUNC) {
    err << (node->internal_callers.size() ? node->emit_func(cursor)
                                          : node->emit_always_comb(cursor));
  } else {
    node->dump_tree();
    assert(false);
  }

  //----------

  if (node->needs_binding()) {
    err << node->emit_func_binding_vars(cursor);
  }

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeIdentifier* node) {
  Err err = cursor.check_at(node);
  auto text = node->get_textstr();

  auto& id_map = cursor.id_map.top();
  auto found = id_map.find(text);

  if (found != id_map.end()) {
    auto replacement = (*found).second;
    err << cursor.emit_replacement(node, "%s", replacement.c_str());
  }
  else if (cursor.preproc_vars.contains(text)) {
    err << cursor.emit_print("`");
    err << cursor.emit_default(node);
  }
  else {
    err << cursor.emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeIf* node) {
  Err err = cursor.check_at(node);

  err << cursor.emit(node->node_kw_if);
  err << cursor.emit_gap();

  err << cursor.emit(node->node_cond);
  err << cursor.emit_gap();

  err << cursor.emit(node->node_then);

  if (node->node_kw_else) {
    err << cursor.emit_gap();
    err << cursor.emit(node->node_kw_else);
  }

  if (node->node_else) {
    err << cursor.emit_gap();
    err << cursor.emit(node->node_else);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeKeyword* node) {
  auto text = node->get_text();

  if (text == "static" || text == "const" || text == "break") {
    return cursor.comment_out(node);
  }

  if (text == "nullptr") {
    return cursor.emit_replacement(node, "\"\"");
  }

  if (text == "if" || text == "else" || text == "default" || text == "for" || "enum") {
    return cursor.emit_raw(node);
  }

  assert(false);
  return ERR("Don't know how to handle this keyword - %s\n", node->get_textstr().c_str());
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeList* node) {
  return cursor.emit_default(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeLValue* node) {
  return cursor.emit_default(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeNamespace* node) {
  Err err = cursor.check_at(node);
  auto node_namespace = node->child("namespace");
  auto node_name      = node->child("name");
  auto node_fields    = node->child("fields");
  auto node_semi      = node->child("semi");

  err << cursor.emit_replacement(node_namespace, "package");
  err << cursor.emit_gap();

  err << cursor.emit_raw(node_name);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  for (auto f : node_fields) {
    if (f->tag_is("ldelim")) {
      err << cursor.skip_over(f);
      err << cursor.emit_gap();
      continue;
    }
    else if (f->tag_is("rdelim")) {
      err << cursor.emit_replacement(f, "endpackage");
      err << cursor.emit_gap();
      continue;
    }
    else {
      err << f->emit(cursor);
      err << cursor.emit_gap();
      continue;
    }
  }

  // Don't need semi after namespace in Verilog
  err << cursor.skip_over(node_semi);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodePreproc* node) {
  Err err = cursor.check_at(node);

  if (node->tag_is("preproc_define")) {
    err << cursor.emit_default(node);
    auto name = node->child("name")->get_textstr();
    cursor.preproc_vars.insert(name);
  }

  else if (node->tag_is("preproc_include")) {
    err << cursor.emit_replacements(node, "#include", "`include", ".h", ".sv");
  }

  else {
    err << cursor.emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodePunct* node) {
  return cursor.emit_default(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeQualifiedIdentifier* node) {
  Err err = cursor.check_at(node);

  auto node_scope = node->child("scope_path");
  auto node_colon = node->child("colon");
  auto node_name  = node->child("identifier");

  bool elide_scope = false;

  if (node_scope->get_text() == "std") elide_scope = true;

  auto node_class = node->ancestor<CNodeClass>();

  // Verilog doesn't like the scope on the enums
  if (node_class && node_class->get_enum(node_scope->get_text())) {
    elide_scope = true;
  }

  if (elide_scope) {
    err << cursor.skip_to(node_name);
    err << cursor.emit(node_name);
  }
  else {
    err << cursor.emit(node_scope);
    err << cursor.emit_gap();
    err << cursor.emit(node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node_name);
  }

  return err << cursor.check_done(node);
}


//------------------------------------------------------------------------------

Err Emitter::emit(CNodeReturn* node) {
  Err err = cursor.check_at(node);

  auto func = node->ancestor<CNodeFunction>();
  auto fname = func->get_namestr();

  auto node_ret  = node->child("return");
  auto node_val  = node->child("value");
  auto node_semi = node->child("semi");

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

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeStruct* node) {
  Err err = cursor.check_at(node);

  auto node_struct = node->child("struct");
  auto node_name   = node->child("name");
  auto node_body   = node->child("body");
  auto node_semi   = node->child("semi");

  err << cursor.emit_replacement(node_struct, "typedef struct packed");
  err << cursor.emit_gap();
  err << cursor.skip_over(node_name);
  err << cursor.skip_gap();
  err << cursor.emit_default(node_body);
  err << cursor.emit_print(" ");
  err << cursor.emit_splice(node_name);
  err << cursor.emit_gap();
  err << cursor.emit(node_semi);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeSwitch* node) {
  Err err = cursor.check_at(node);

  auto node_switch = node->child("switch");
  auto node_cond   = node->child("condition");
  auto node_body   = node->child("body");

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

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeCase* node) {
  Err err = cursor.check_at(node);

  err << cursor.skip_over(node->node_case);
  err << cursor.skip_gap();
  err << cursor.emit(node->node_cond);
  err << cursor.emit_gap();

  if (node->node_body) {
    err << cursor.emit(node->node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node->node_body);
  }
  else {
    err << cursor.emit_replacement(node->node_colon, ",");
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeDefault* node) {

  Err err = cursor.check_at(node);

  auto node_default = node->child("default");
  auto node_colon   = node->child("colon");
  auto node_body    = node->child("body");

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

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeTemplate* node) {
  Err err = cursor.check_at(node);

  err << cursor.skip_over(node->node_template);
  err << cursor.skip_gap();

  err << cursor.skip_over(node->node_params);
  err << cursor.skip_gap();

  err << cursor.emit(node->node_class);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeTranslationUnit* node) {
  return cursor.emit_default(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeType* node) {
  return ERR("Don't know how to handle this type\n");
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeTypedef* node) {
  return ERR("Don't know how to handle this type\n");
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeUsing* node) {
  Err err = cursor.check_at(node);

  err << cursor.skip_over(node);

  err << cursor.emit_print("import ");
  err << cursor.emit_splice(node->child("name"));
  err << cursor.emit_print("::*;");

  return err << cursor.check_done(node);
}

/*
//------------------------------------------------------------------------------

Err Emitter::emit(CNodeFor* node) {
}
*/

//==============================================================================
