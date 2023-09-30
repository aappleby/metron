#include "CNodeCall.hpp"

#include "metron/Cursor.hpp"
#include "metron/Emitter.hpp"

#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeConstant.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"

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
