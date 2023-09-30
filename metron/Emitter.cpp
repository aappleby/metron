#include "Emitter.hpp"

#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeAssignment.hpp"
#include "metron/nodes/CNodeBuiltinType.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeConstant.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
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
    auto func_name  = func_path->node_name->get_text();

    auto src_field = src_class->get_field(field_name);
    auto dst_class = src_class->repo->get_class(src_field->get_type_name());
    auto dst_func  = dst_class->get_function(func_name);

    auto rtype = dst_func->child("return_type");

    if (rtype->get_text() == "void") {
      err << cursor.comment_out(node);
      return err << cursor.check_done(node);
    }
    else {
      err << cursor.emit_replacement(node,
        "%.*s_%.*s_ret",
        field_name.size(), field_name.data(),
        func_name.size(), func_name.data());
      return err << cursor.check_done(node);
    }
  }


  if (auto func_id = node->node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_textstr();

    if (cursor.id_map.top().contains(func_name)) {
      auto replacement = cursor.id_map.top()[func_name];

      err << cursor.emit_replacement(node->node_name, "%s", replacement.c_str());
      err << cursor.emit_gap();

      if (node->node_targs) {
        err << cursor.skip_over(node->node_targs);
        err << cursor.skip_gap();
      }

      err << cursor.emit(node->node_args);

      return err << cursor.check_done(node);
      //return cursor.emit_default(this);
    }

    if (func_name == "cat") {
      err << cursor.skip_to(node->node_args);
      for (auto child : node->node_args) {
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
      auto lhs  = args->items[0];
      auto rhs  = args->items[1];

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

    if (src_mtype == MT_TOCK && (dst_mtype == MT_TOCK || dst_mtype == MT_TICK)) {
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
      return err << cursor.check_done(node);
    }
  }

  if (auto func_path = node->node_name->as<CNodeFieldExpression>()) {
    auto field_name = func_path->child("field_path")->get_text();
    auto func_name  = func_path->child("identifier")->get_text();

    auto src_class = node->ancestor<CNodeClass>();
    auto field = src_class->get_field(field_name);
    auto dst_class = field->node_decl->_type_class;
    auto dst_func = dst_class->get_function(func_name);

    if (dst_func->child("return_type")->get_text() == "void") {
      err << cursor.comment_out(node);
    }
    else {
      err << cursor.skip_over(node);
      err << cursor.emit_print("%.*s_%.*s_ret", field_name.size(), field_name.data(), func_name.size(), func_name.data());
    }

    return err << cursor.check_done(node);
  }

  assert(false);
  return err << cursor.check_done(node);
}

//==============================================================================
