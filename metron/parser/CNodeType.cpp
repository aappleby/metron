#include "CNodeType.hpp"

#include "metrolib/core/Log.h"
#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeType::debug_color() const {
  return COL_VIOLET;
}

//------------------------------------------------------------------------------

Err CNodeType::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_name  = child("name");
  auto node_targs = child("template_args");
  auto node_scope = child("scope");

  if (as<CNodeBuiltinType>() && node_targs) {
    auto node_ldelim = node_targs->child("ldelim");
    auto node_exp    = node_targs->child("arg");
    auto node_rdelim = node_targs->child("rdelim");

    err << cursor.emit_raw(node_name);
    err << cursor.emit_gap();

    if (auto node_const_int = node_exp->as<CNodeConstInt>()) {
      auto width = atoi(node_exp->text_begin());
      if (width == 1) {
        // logic<1> -> logic
        err << cursor.skip_over(node_targs);
      }
      else {
        // logic<N> -> logic[N-1:0]
        err << cursor.emit_replacement(node_ldelim, "[");
        err << cursor.emit_gap();
        err << cursor.skip_over(node_exp);
        err << cursor.emit_gap();
        err << cursor.emit_print("%d:0", width - 1);
        err << cursor.emit_replacement(node_rdelim, "]");
      }
    }
    else if (auto node_identifier = node_exp->as<CNodeIdentifier>()) {
        // logic<CONSTANT> -> logic[CONSTANT-1:0]
        err << cursor.emit_replacement(node_ldelim, "[");
        err << cursor.emit_gap();
        err << cursor.skip_over(node_exp);
        err << cursor.emit_gap();
        err << cursor.emit_splice(node_identifier);
        err << cursor.emit_print("-1:0");
        err << cursor.emit_replacement(node_rdelim, "]");
    }
    else {
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
  }
  else if (as<CNodeBuiltinType>()) {
    err << cursor.emit_raw(this);
  }
  else if (auto node_struct = child("struct_name")) {
    err << CNode::emit(cursor);
  }
  else if (auto node_class = child("class_name")) {
    err << CNode::emit(cursor);
  }
  else if (auto node_enum = as<CNodeEnumType>()) {
    err << cursor.emit_default(this);
  }
  else if (auto node_class_type = as<CNodeClassType>()) {
    auto targs = child("template_args");

    if (targs) {
      err << cursor.emit(child("name"));
      err << cursor.emit_gap();
      err << cursor.skip_over(targs);
    }
    else {
      err << cursor.emit_default(this);
    }
  }
  else {
    NODE_ERR("Don't know how to handle this type\n");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeStructType::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_raw(this);
  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
