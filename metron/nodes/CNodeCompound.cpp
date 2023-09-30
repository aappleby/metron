#include "CNodeCompound.hpp"

#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeExpStatement.hpp"
#include "metron/nodes/CNodeFor.hpp"

//==============================================================================

void CNodeCompound::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  for (auto child : this) {
    if (!child->tag_is("ldelim") && !child->tag_is("rdelim")) {
      statements.push_back(child);
    }
  }
}

//------------------------------------------------------------------------------

Err CNodeCompound::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto statement : statements) {
    err << statement->trace(inst, stack);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeCompound::emit(Cursor& cursor) {
  return emit_block(cursor, "begin", "end");
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

Err CNodeCompound::emit_block(Cursor& cursor, std::string ldelim, std::string rdelim) {
  Err err;

  cursor.elide_type.push(true);
  cursor.elide_value.push(false);

  if (auto child = this->child("ldelim")) {
    err << cursor.emit_replacement(child, "%s", ldelim.c_str());
    cursor.indent_level++;
    cursor.elide_type.push(false);
    cursor.elide_value.push(true);
    err << emit_hoisted_decls(cursor);
    cursor.elide_type.pop();
    cursor.elide_value.pop();
    err << cursor.emit_gap();
  }

  for (auto child : statements) {
    // We may need to insert input port bindings before any statement that
    // could include a call expression. We search the tree for calls and emit
    // those bindings here.
    if (!child->as<CNodeCompound>()) {
      err << emit_call_arg_bindings(child, cursor);
    }
    err << cursor.emit(child);
    err << cursor.emit_gap();
  }

  if (auto child = this->child("rdelim")) {
    cursor.indent_level--;
    err << cursor.emit_replacement(child, "%s", rdelim.c_str());
  }

  cursor.elide_type.pop();
  cursor.elide_value.pop();

  return err;
}

//------------------------------------------------------------------------------

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

//==============================================================================
