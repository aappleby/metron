#include "CNodeType.hpp"

#include "metrolib/core/Log.h"
#include "NodeTypes.hpp"
#include "CNodeExpression.hpp"

uint32_t CNodeType::debug_color() const {
  return COL_VIOLET;
}

/*

//------------------------------------------------------------------------------
// Change logic<N> to logic[N-1:0]

// + type: template_type (348) =
// |--# name: type_identifier (444) = "logic"
// |--+ arguments: template_argument_list (351) =

CHECK_RETURN Err MtCursor::emit_sym_template_type(MnNode n) {
  Err err = check_at(sym_template_type, n);

  auto node_name = n.get_field(field_name);
  auto node_args = n.get_field(field_arguments);

  bool is_logic = node_name.match("logic");

  err << emit_dispatch(node_name);
  err << emit_gap(node_name, node_args);

  if (is_logic) {
    auto logic_size = node_args.first_named_child();
    if (logic_size.sym == sym_number_literal) {
      int width = atoi(logic_size.start());
      if (width > 1) {
        err << emit_replacement(node_args, "[%d:0]", width - 1);
      }
      else {
        err << skip_over(node_args);
      }
    }
    else {
      err << skip_over(node_args);
      err << emit_print("[");
      err << emit_splice(logic_size);
      err << emit_print("-1:0]");
    }
  }
  else {
    err << skip_over(node_args);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_type_identifier(MnNode n) {
  Err err = check_at(alias_sym_type_identifier, n);

  auto name = n.name4();
  auto& rep = id_map.top();

  if (auto it = rep.find(name); it != rep.end()) {
    err << emit_replacement(n, it->second.c_str());
  }
  else {
    err << emit_text(n);
  }

  return err << check_done(n);
}


//------------------------------------------------------------------------------
// "unsigned int" -> "int unsigned"

CHECK_RETURN Err MtCursor::emit_sym_sized_type_specifier(MnNode n) {
  Err err = check_at(sym_sized_type_specifier, n);

  MnNode node_size;
  MnNode node_type;

  for (auto c : n) {
    if (c.field == field_type) {
      node_type = c;
    }
    else if (c.sym == anon_sym_signed || c.sym == anon_sym_unsigned) {
      node_size = c;
    }
  }

  for (auto c : n) {
    err << emit_ws_to(c);

    if (c.field == field_type) {
      err << emit_splice(node_size);
    }
    else if (c.sym == anon_sym_signed || c.sym == anon_sym_unsigned) {
      err << emit_splice(node_type);
    }
    else {
      err << emit_dispatch(c);
    }
    cursor = c.end();
  }

  return err << check_done(n);
}


[000.012]  ┳━ CNodeType decl_type =
[000.012]  ┣━━━━ CNode builtin_name = "logic"
[000.012]  ┗━━┳━ CNode type_args =
[000.012]     ┗━━┳━ CNodeExpression exp =
[000.012]        ┗━━┳━ CNode unit =
[000.012]           ┗━━┳━ CNodeConstant constant =
[000.012]              ┗━━━━ CNode int = "8"

[000.004] __ ▆ CNodeField =
[000.004]  ┣━━╸▆ type : CNodeBuiltinType =
[000.004]  ┃   ┣━━╸▆ name : CNodeIdentifier = "logic"
[000.004]  ┃   ┗━━╸▆ template_args : CNodeList =
[000.004]  ┃       ┣━━╸▆ CNodePunct = "<"
[000.005]  ┃       ┣━━╸▆ CNodeConstInt = "1"
[000.005]  ┃       ┗━━╸▆ CNodePunct = ">"
[000.005]  ┗━━╸▆ name : CNodeIdentifier = "x"

*/

Err CNodeType::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_name  = child("name");
  auto node_targs = child("template_args");

  if (as_a<CNodeBuiltinType>() && node_targs) {
    auto node_ldelim = node_targs->child("ldelim");
    auto node_exp    = node_targs->child("arg");
    auto node_rdelim = node_targs->child("rdelim");

    err << cursor.emit_raw(node_name);
    err << cursor.emit_gap_after(node_name);

    if (auto node_const_int = node_exp->as_a<CNodeConstInt>()) {
      auto width = atoi(node_exp->text_begin());
      if (width == 1) {
        // logic<1> -> logic
        err << cursor.skip_over(node_targs);
        err << cursor.emit_gap_after(node_targs);
      }
      else {
        // logic<N> -> logic[N-1:0]
        err << cursor.emit_replacement(node_ldelim, "[");
        err << cursor.emit_gap_after(node_ldelim);
        err << cursor.skip_over(node_exp);
        err << cursor.emit_gap_after(node_exp);
        err << cursor.emit_print("%d:0", width - 1);
        err << cursor.emit_replacement(node_rdelim, "]");
        err << cursor.emit_gap_after(node_rdelim);
      }
    }
    else {
      // logic<exp> -> logic[(exp)-1:0]
      err << cursor.emit_replacement(node_ldelim, "[");
      err << cursor.emit_gap_after(node_ldelim);
      err << cursor.emit_print("(");
      err << cursor.emit(node_exp);
      err << cursor.emit_gap_after(node_exp);
      err << cursor.emit_print(")-1:0");
      err << cursor.emit_replacement(node_rdelim, "]");
    }
  }
  else if (as_a<CNodeBuiltinType>()) {
    err << cursor.emit_raw(this);
  }
  else if (auto node_struct = child("struct_name")) {
    err << CNode::emit(cursor);
  }
  else if (auto node_class = child("class_name")) {
    err << CNode::emit(cursor);
  }
  else {
    NODE_ERR("Don't know how to handle this type\n");
  }

  return err << cursor.check_done(this);
}


CHECK_RETURN Err CNodeStructType::emit(Cursor& cursor) {
  return cursor.emit_raw(this);
}
