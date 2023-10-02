#include "Emitter.hpp"

#include <stdio.h>
#include <stdarg.h>

#include "metron/Cursor.hpp"
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
#include "metron/nodes/CNodeTypedef.hpp"
#include "metron/nodes/CNodeWhile.hpp"
#include "metron/nodes/CNodePreproc.hpp"
#include "metron/nodes/CNodeQualifiedIdentifier.hpp"
#include "metron/nodes/CNodeTranslationUnit.hpp"
#include "metron/nodes/CNodeUsing.hpp"


//------------------------------------------------------------------------------

bool class_needs_tick(CNodeClass* node_class) {
  for (auto f : node_class->all_functions) {
    if (f->method_type == MT_TICK) return true;
  }

  for (auto f : node_class->all_fields) {
    if (f->node_decl->_type_class && class_needs_tick(f->node_decl->_type_class)) return true;
  }

  return false;
}

//----------------------------------------

bool class_needs_tock(CNodeClass* node_class) {
  for (auto f : node_class->all_functions) {
    if (f->method_type == MT_TOCK) return true;
  }

  for (auto f : node_class->all_fields) {
    if (f->node_decl->_type_class && class_needs_tock(f->node_decl->_type_class)) return true;
  }

  return false;
}

//------------------------------------------------------------------------------

CNodeField* resolve_field(CNodeClass* node_class, CNode* node_name) {
  if (node_name == nullptr) return nullptr;

  if (auto node_id = node_name->as<CNodeIdentifier>()) {
    return node_class->get_field(node_id->get_text());
  }

  if (node_name->as<CNodeLValue>()) {
    return resolve_field(node_class, node_name->child("name"));
  }

  if (auto node_field = node_name->as<CNodeFieldExpression>()) {
    return resolve_field(node_class, node_field->child("field_path"));
  }

  if (auto node_prefix = node_name->as<CNodeSuffixExp>()) {
    return resolve_field(node_class, node_prefix->child("rhs"));
  }

  if (auto node_suffix = node_name->as<CNodeSuffixExp>()) {
    return resolve_field(node_class, node_suffix->child("lhs"));
  }

  LOG_R("----------------------------------------\n");
  LOG_R("Don't know how to get field for %s\n", node_name->get_textstr().c_str());
  LOG_R("----------------------------------------\n");

  assert(false);
  return nullptr;
}

//------------------------------------------------------------------------------

bool is_bit_extract(CNodeCall* node) {
  if (auto func_id = node->node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_textstr();

    auto args = node->node_args->items.size();

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



//==============================================================================

Err Emitter::emit_default(CNode* node) {
  Err err = cursor.check_at(node);

  if (node->child_head) {
    err << emit_children(node);
  }
  else {
    err << cursor.emit_span(node->tok_begin(), node->tok_end());
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_children(CNode* n) {
  Err err;
  for (auto c = n->child_head; c; c = c->node_next) {
    err << emit_dispatch(c);
    if (c->node_next) err << cursor.emit_gap();
  }
  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_everything() {
  Err err;

  // Emit header
  for (auto lex_cursor = cursor.lex_begin; lex_cursor < cursor.tok_begin->lex; lex_cursor++) {
    for (auto c = lex_cursor->text_begin; c < lex_cursor->text_end; c++) {
      err << cursor.emit_char(*c);
    }
  }

  // Emit body
  err << emit_dispatch(cursor.source_file->context.root_node);

  // Emit footer (everything in the gap after the translation unit)
  err << cursor.emit_gap();

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_dispatch(CNode* node) {
  Err err = cursor.check_at(node);

  if (auto n = node->as<CNodeAccess>()) return cursor.comment_out(node);

  if (auto n = node->as<CNodeAssignment>()) return emit(n);
  if (auto n = node->as<CNodeBuiltinType>()) return emit(n);
  if (auto n = node->as<CNodeCall>()) return emit(n);
  if (auto n = node->as<CNodeCase>()) return emit(n);
  if (auto n = node->as<CNodeClass>()) return emit(n);
  if (auto n = node->as<CNodeClassType>()) return emit(n);
  if (auto n = node->as<CNodeCompound>()) return emit(n);
  if (auto n = node->as<CNodeConstant>()) return emit(n);
  if (auto n = node->as<CNodeConstructor>()) return emit(n);
  if (auto n = node->as<CNodeDeclaration>()) return emit(n);
  if (auto n = node->as<CNodeDefault>()) return emit(n);
  if (auto n = node->as<CNodeEnum>()) return emit(n);
  if (auto n = node->as<CNodeExpStatement>()) return emit(n);
  if (auto n = node->as<CNodeField>()) return emit(n);
  if (auto n = node->as<CNodeFieldExpression>()) return emit(n);
  if (auto n = node->as<CNodeFunction>()) return emit(n);
  if (auto n = node->as<CNodeIdentifier>()) return emit(n);
  if (auto n = node->as<CNodeIf>()) return emit(n);
  if (auto n = node->as<CNodeKeyword>()) return emit(n);
  if (auto n = node->as<CNodeNamespace>()) return emit(n);
  if (auto n = node->as<CNodePrefixExp>()) return emit(n);
  if (auto n = node->as<CNodePreproc>()) return emit(n);
  if (auto n = node->as<CNodeQualifiedIdentifier>()) return emit(n);
  if (auto n = node->as<CNodeReturn>()) return emit(n);
  if (auto n = node->as<CNodeStruct>()) return emit(n);
  if (auto n = node->as<CNodeSuffixExp>()) return emit(n);
  if (auto n = node->as<CNodeSwitch>()) return emit(n);
  if (auto n = node->as<CNodeTemplate>()) return emit(n);
  if (auto n = node->as<CNodeUsing>()) return emit(n);

  return emit_default(node);
}

Err Emitter::emit_dispatch2(CNode* node) {
  Err err;
  if (node == nullptr) return err;
  err << emit_dispatch(node);
  err << cursor.emit_gap(node);
  return err;
}

//------------------------------------------------------------------------------
// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

Err Emitter::emit(CNodeAssignment* node) {
  Err err = cursor.check_at(node);

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func  = node->ancestor<CNodeFunction>();
  auto node_field = resolve_field(node_class, node->node_lhs);

  err << emit_dispatch2(node->node_lhs);

  // If we're in a tick, emit < to turn = into <=
  if (node_func->method_type == MT_TICK && node_field) {
    err << cursor.emit_print("<");
  }

  if (node->node_op->get_text() == "=") {
    err << emit_dispatch2(node->node_op);
  } else {
    auto lhs_text = node->node_lhs->get_textstr();
    err << cursor.skip_over2(node->node_op);
    err << cursor.emit_print("= %s %c ", lhs_text.c_str(), node->node_op->get_text()[0]);
  }

  err << emit_dispatch(node->node_rhs);

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
        err << cursor.emit_replacement2(node_ldelim, "[");
        err << cursor.skip_over2(node_exp);
        err << cursor.emit_print("%d:0", width - 1);
        err << cursor.emit_replacement(node_rdelim, "]");
      }
    } else if (auto node_identifier = node_exp->as<CNodeIdentifier>()) {
      // logic<CONSTANT> -> logic[CONSTANT-1:0]
      err << cursor.emit_replacement2(node_ldelim, "[");
      err << cursor.skip_over2(node_exp);
      err << emit_splice(node_identifier);
      err << cursor.emit_print("-1:0");
      err << cursor.emit_replacement2(node_rdelim, "]");
    } else {
      // logic<exp> -> logic[(exp)-1:0]
      err << cursor.emit_replacement2(node_ldelim, "[");
      err << cursor.emit_print("(");
      err << emit_dispatch2(node_exp);
      err << cursor.emit_print(")-1:0");
      err << cursor.emit_replacement2(node_rdelim, "]");
    }

    if (node_scope) {
      err << cursor.skip_gap();
      err << cursor.skip_over(node_scope);
    }
  } else {
    err << emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Emitter::emit_splice(CNode* n) {
  Err err;
  auto old_cursor = cursor.tok_cursor;
  cursor.tok_cursor = n->tok_begin();
  err << emit_dispatch(n);
  cursor.tok_cursor = old_cursor;
  return err;
}


Err Emitter::emit(const char* fmt, ...) {
  Err err;
  va_list args;
  va_start(args, fmt);

  const char* f = fmt;

  while(*f) {
    switch(*f) {
      case '@': {
        CNode* node = va_arg(args, CNode*);
        err << emit_splice(node);
        f++;
        break;
      }
      default: {
        err << cursor.emit_print("%c", *f);
        f++;
        break;
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeCall* node) {
  Err err = cursor.check_at(node);

  if (is_bit_extract(node)) return emit_bit_extract(node);

  auto src_class = node->ancestor<CNodeClass>();
  auto src_func = node->ancestor<CNodeFunction>();

  if (auto func_path = node->node_name->as<CNodeFieldExpression>()) {
    // We don't actually "call" into submodules, so we can just comment this
    // call out.

    auto field_name = func_path->node_path->get_textstr();
    auto func_name = func_path->node_name->get_textstr();

    auto src_field = src_class->get_field(field_name);
    auto dst_class = src_class->repo->get_class(src_field->node_decl->node_type->name);
    auto dst_func = dst_class->get_function(func_name);

    auto rtype = dst_func->child("return_type");

    if (rtype->name == "void") {
      err << cursor.comment_out(node);
    } else {
      err << emit("@_@_ret", func_path->node_path, func_path->node_name);
      err << cursor.skip_over(node);
    }
    return err << cursor.check_done(node);
  }

  if (auto func_id = node->node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_textstr();

    if (cursor.id_map.top().contains(func_name)) {
      auto replacement = cursor.id_map.top()[func_name];

      err << cursor.emit_replacement2(node->node_name, "%s", replacement.c_str());

      if (node->node_targs) {
        err << cursor.skip_over2(node->node_targs);
      }

      err << emit_dispatch(node->node_args);

      return err << cursor.check_done(node);
    }

    if (func_name == "cat") {
      err << cursor.skip_to(node->node_args);
      for (auto child : node->node_args) {
        if (child->tag_is("ldelim")) {
          err << cursor.emit_replacement2(child, "{");
        } else if (child->tag_is("rdelim")) {
          err << cursor.emit_replacement2(child, "}");
        } else {
          err << emit_dispatch2(child);
        }
      }
      return err << cursor.check_done(node);
    }

    //----------
    // Convert "dup<15>(x)" to "{15 {x}}"

    if (func_name == "dup") {
      auto node_val = node->node_targs->child("arg");
      auto node_exp = node->node_args->child("exp");
      err << emit("{@ {@}}", node_val, node_exp);
      err << cursor.skip_over(node);
      return err;
    }

    if (func_name == "sra") {
      auto args = node->node_args->as<CNodeList>();
      auto lhs = args->items[0];
      auto rhs = args->items[1];

      err << emit("($signed(@) >>> @)", lhs, rhs);
      err << cursor.skip_over(node);

      return err;
    }

    //----------
    // Not a special builtin call

    auto dst_func = src_class->get_function(func_id->get_text());
    auto dst_params = dst_func->node_params;

    // Replace call with return binding variable if the callee is a tock

    if (dst_func->method_type == MT_TOCK) {
      auto dst_name = dst_func->name;
      err << cursor.skip_over(node);
      err << cursor.emit_print("%s_ret", dst_name.c_str());
      return err;
    }

    auto src_mtype = src_func->method_type;
    auto dst_mtype = dst_func->method_type;

    if (dst_mtype == MT_INIT) {
      assert(src_mtype == MT_INIT);
      return emit_default(node);
    }

    if (dst_mtype == MT_FUNC) {
      return emit_default(node);
    }

    if (src_mtype == MT_TICK && dst_mtype == MT_TICK) {
      return emit_default(node);
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
        err << cursor.emit_print("%s_%s = %s", dst_func->name.c_str(),
                                 param->child("name")->name.c_str(),
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
    auto field_name = func_path->node_path->get_text();
    auto func_name = func_path->node_name->get_text();

    auto src_class = node->ancestor<CNodeClass>();
    auto field = src_class->get_field(field_name);
    auto dst_class = field->node_decl->_type_class;
    auto dst_func = dst_class->get_function(func_name);

    if (dst_func->node_type->get_text() == "void") {
      err << cursor.comment_out(node);
    } else {
      err << emit("@_@_ret", func_path->node_path, func_path->node_name);
      err << cursor.skip_over(node);
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
  err << emit_dispatch2(node_name);

  err << cursor.emit_print("(");
  cursor.indent_level++;
  err << emit_module_ports(node);
  cursor.indent_level--;
  err << cursor.start_line();
  err << cursor.emit_print(");");

  for (auto child : node_body) {
    if (child->get_text() == "{") {
      cursor.indent_level++;
      err << cursor.skip_over(child);
      err << emit_template_parameter_list(node);
      err << cursor.emit_gap(child);
    } else if (child->get_text() == "}") {
      cursor.indent_level--;
      err << cursor.emit_replacement2(child, "endmodule");
    } else {
      err << emit_dispatch2(child);
    }
  }

  err << cursor.skip_gap();
  err << cursor.skip_over(node_semi);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeClassType* node) {
  Err err;

  if (node->node_targs) {
    err << emit_dispatch2(node->child("name"));
    err << cursor.skip_over(node->node_targs);
  } else {
    err << emit_default(node);
  }
  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeCompound* node) {
  return emit_block(node, "begin", "end");
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
  return emit((CNodeFunction*)node);
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
        err << cursor.skip_over2(c);
      } else {
        err << emit_dispatch2(c);
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

    err << emit_dispatch2(child);
  }

  return err << cursor.check_done(node);
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

  err << emit(node->node_enum);
  err << cursor.emit_gap();

  err << cursor.skip_over2(node->node_class);

  err << cursor.skip_over2(node->node_name);

  if (node->node_colon) {
    err << cursor.skip_over2(node->node_colon);
    err << emit_dispatch2(node->node_type);
  }

  err << emit_dispatch2(node->node_body);

  err << emit_dispatch2(node->node_decl);

  if (node->node_name) {
    err << cursor.emit_print(" ");
    err << emit_splice(node->node_name);
  }

  err << emit_dispatch(node->node_semi);

  cursor.override_size.pop();

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodePrefixExp* node) {
  Err err = cursor.check_at(node);

  auto node_op = node->child("prefix");
  auto op = node_op->get_text();

  if (op != "++" && op != "--") {
    return emit_default(node);
  }

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func = node->ancestor<CNodeFunction>();
  auto node_rhs = node->child("rhs");
  auto node_field = resolve_field(node_class, node_rhs);

  err << cursor.skip_over(node);
  err << emit_splice(node_rhs);
  if (node_func->method_type == MT_TICK && node_field) {
    err << cursor.emit_print(" <= ");
  } else {
    err << cursor.emit_print(" = ");
  }
  err << emit_splice(node_rhs);
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
  auto node_field = resolve_field(node_class, node_lhs);

  auto op = node_op->get_text();

  if (op == "++" || op == "--") {
    err << cursor.skip_over(node);
    err << emit_splice(node_lhs);
    if (node_func->method_type == MT_TICK && node_field) {
      err << cursor.emit_print(" <= ");
    } else {
      err << cursor.emit_print(" = ");
    }
    err << emit_splice(node_lhs);
    if (op == "++") {
      err << cursor.emit_print(" + 1");
    } else {
      err << cursor.emit_print(" - 1");
    }
  } else {
    err << emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeExpStatement* node) {
  Err err = cursor.check_at(node);

  // Call expressions turn into either Verilog calls, or are replaced with
  // binding statements.
  if (auto call = node->node_exp->as<CNodeCall>()) {
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
    }
    else {
      err << emit_default(node);
    }
  }
  else if (auto keyword = node->node_exp->as<CNodeKeyword>()) {
    if (keyword->get_text() == "break") {
      err << cursor.comment_out(node);
    }
    else {
      err << emit_default(node);
    }
  }
  else if (auto decl = node->node_exp->as<CNodeDeclaration>()) {
    if (cursor.elide_type.top() && decl->node_value == nullptr) {
      err << cursor.skip_over(node);
    }
    else {
      err << emit_default(node);
    }
  }
  else {
    err << emit_default(node);
  }

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
    err << emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << emit_dispatch(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    err << emit_dispatch(node->node_decl->node_eq);
    err << cursor.emit_gap();
    err << emit_dispatch(node->node_decl->node_value);
    err << cursor.emit_gap();

    err << emit_dispatch(node->node_semi);

    return err << cursor.check_done(node);
  }

  //----------------------------------------

  if (node->node_decl->node_const) {
    // Localparam
    bool in_namespace = node->ancestor<CNodeNamespace>() != nullptr;
    err << cursor.emit_print(in_namespace ? "parameter " : "localparam ");

    err << cursor.comment_out2(node->node_decl->node_static);
    err << cursor.comment_out2(node->node_decl->node_const);

    err << emit_dispatch(node->node_decl->node_type);
    err << cursor.emit_gap();

    err << emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << emit_dispatch(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_eq) {
      err << emit_dispatch(node->node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_value) {
      err << emit_dispatch(node->node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << emit_dispatch(node->node_semi);
    return err << cursor.check_done(node);
  }

  //----------------------------------------

  auto node_builtin = node->node_decl->node_type->as<CNodeBuiltinType>();
  auto node_targs = node->node_decl->node_type->child("template_args");

  if (node_builtin && node_targs) {
    err << cursor.skip_to(node->node_decl->node_type);
    err << emit_dispatch(node->node_decl->node_type);
    err << cursor.emit_gap();

    err << emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << emit_dispatch(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_eq) {
      err << emit_dispatch(node->node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_value) {
      err << emit_dispatch(node->node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << emit_dispatch(node->node_semi);

    return err << cursor.check_done(node);
  }

  //----------------------------------------

  if (node_builtin) {
    err << cursor.skip_to(node->node_decl->node_type);
    err << emit_dispatch(node->node_decl->node_type);
    err << cursor.emit_gap();

    err << emit(node->node_decl->node_name);
    err << cursor.emit_gap();

    if (node->node_decl->node_array) {
      err << emit_dispatch(node->node_decl->node_array);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_eq) {
      err << emit_dispatch(node->node_decl->node_eq);
      err << cursor.emit_gap();
    }

    if (node->node_decl->node_value) {
      err << emit_dispatch(node->node_decl->node_value);
      err << cursor.emit_gap();
    }

    err << emit_dispatch(node->node_semi);
    return err << cursor.check_done(node);
  }

  //----------------------------------------

  if (node->is_struct()) {
    return cursor.emit_raw(node);
  }

  //----------------------------------------

  if (node->is_component()) {
    return emit_component(node);
  }

  //----------------------------------------

  assert(false);
  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz if the field refers to a submodule port,
// so that it instead refers to a glue expression.

Err Emitter::emit(CNodeFieldExpression* node) {
  Err err = cursor.check_at(node);

  auto node_func = node->ancestor<CNodeFunction>();
  auto node_class = node->ancestor<CNodeClass>();
  auto node_path = node->child("field_path");
  auto node_name = node->child("identifier");

  auto field = node_class->get_field(node_path->get_text());

  if (field && field->node_decl->_type_struct) {
    err << emit_default(node);
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
    err << emit_default(node);
  }

  return err << cursor.check_done(node);
}

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
    err << (node->as<CNodeConstructor>() ? emit_init(node)
                                         : emit_task(node));
  } else if (node->method_type == MT_TOCK) {
    err << emit_always_comb(node);
  } else if (node->method_type == MT_TICK) {
    err << (called_by_tick ? emit_task(node)
                           : emit_always_ff(node));
  } else if (node->method_type == MT_FUNC) {
    err << (node->internal_callers.size() ? emit_func(node)
                                          : emit_always_comb(node));
  } else {
    assert(false);
  }

  //----------

  if (node->needs_binding()) {
    err << emit_func_binding_vars(node);
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
    err << emit_default(node);
  }
  else {
    err << emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeIf* node) {
  Err err = cursor.check_at(node);

  err << emit(node->node_kw_if);
  err << cursor.emit_gap();

  err << emit_dispatch(node->node_cond);
  err << cursor.emit_gap();

  err << emit_dispatch(node->node_then);

  if (node->node_kw_else) {
    err << cursor.emit_gap();
    err << emit(node->node_kw_else);
  }

  if (node->node_else) {
    err << cursor.emit_gap();
    err << emit_dispatch(node->node_else);
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
      err << emit_dispatch(f);
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
    err << emit_default(node);
    auto name = node->child("name")->get_textstr();
    cursor.preproc_vars.insert(name);
  }

  else if (node->tag_is("preproc_include")) {
    err << cursor.emit_replacements(node, "#include", "`include", ".h", ".sv");
  }

  else {
    err << emit_default(node);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeQualifiedIdentifier* node) {
  Err err = cursor.check_at(node);

  bool elide_scope = false;

  if (node->node_scope->get_text() == "std") elide_scope = true;

  auto node_class = node->ancestor<CNodeClass>();

  // Verilog doesn't like the scope on the enums
  if (node_class && node_class->get_enum(node->node_scope->get_text())) {
    elide_scope = true;
  }

  if (elide_scope) {
    err << cursor.skip_to(node->node_name);
    err << emit_dispatch(node->node_name);
  }
  else {
    err << emit_dispatch(node->node_scope);
    err << cursor.emit_gap();
    err << emit_dispatch(node->node_colon);
    err << cursor.emit_gap();
    err << emit_dispatch(node->node_name);
  }

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeReturn* node) {
  Err err = cursor.check_at(node);

  auto func = node->ancestor<CNodeFunction>();
  auto fname = func->name;

  assert(node->node_val);

  err << cursor.skip_over(node->node_ret);
  err << cursor.skip_gap();

  if (func->should_emit_as_task() || func->should_emit_as_func()) {
    err << cursor.emit_print("%s = ", fname.c_str());
  }
  else {
    err << cursor.emit_print("%s_ret = ", fname.c_str());
  }

  err << emit_dispatch(node->node_val);
  err << cursor.emit_gap();

  err << emit_dispatch(node->node_semi);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeStruct* node) {
  Err err = cursor.check_at(node);

  err << cursor.emit_replacement(node->node_struct, "typedef struct packed");
  err << cursor.emit_gap();
  err << cursor.skip_over(node->node_name);
  err << cursor.skip_gap();
  err << emit_dispatch(node->node_body);
  err << cursor.emit_print(" ");
  err << emit_splice(node->node_name);
  err << cursor.emit_gap();
  err << emit_dispatch(node->node_semi);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeSwitch* node) {
  Err err = cursor.check_at(node);

  err << cursor.emit_replacement(node->node_switch, "case");
  err << cursor.emit_gap();
  err << emit_dispatch(node->node_cond);
  err << cursor.emit_gap();

  for (auto child : node->node_body) {
    if (child->tag_is("ldelim")) {
      err << cursor.skip_over(child);
    }
    else if (child->tag_is("rdelim")) {
      err << cursor.emit_replacement(child, "endcase");
    }
    else {
      err << emit_dispatch(child);
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
  err << emit_dispatch(node->node_cond);
  err << cursor.emit_gap();

  if (node->node_body) {
    err << emit_dispatch(node->node_colon);
    err << cursor.emit_gap();
    err << emit_dispatch(node->node_body);
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

  err << emit_dispatch(node_default);
  err << cursor.emit_gap();

  if (node_body) {
    err << emit_dispatch(node_colon);
    err << cursor.emit_gap();
    err << emit_dispatch(node_body);
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

  err << emit(node->node_class);

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeUsing* node) {
  Err err = cursor.check_at(node);

  err << cursor.skip_over(node);

  err << cursor.emit_print("import ");
  err << emit_splice(node->child("name"));
  err << cursor.emit_print("::*;");

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_template_parameter_list(CNodeClass* node) {
  Err err;

  auto node_template = node->ancestor<CNodeTemplate>();
  if (!node_template) return err;

  auto old_cursor = cursor.tok_cursor;

  for (auto param : node_template->params) {
    err << cursor.start_line();
    err << cursor.emit_print("parameter ");

    cursor.tok_cursor = param->node_name->tok_begin();
    err << emit(param->node_name);
    err << cursor.emit_gap();

    if (param->node_array) {
      err << emit_dispatch(param->node_array);
      err << cursor.emit_gap();
    }

    err << Emitter(cursor).emit_dispatch(param->node_eq);
    err << cursor.emit_gap();

    err << emit_dispatch(param->node_value);

    err << cursor.emit_print(";");
  }

  err << cursor.emit_print("\n");

  cursor.tok_cursor = old_cursor;

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_block(CNodeCompound* node, std::string ldelim, std::string rdelim) {
  Err err;

  cursor.elide_type.push(true);
  cursor.elide_value.push(false);

  if (auto child = node->child("ldelim")) {
    err << cursor.emit_replacement(child, "%s", ldelim.c_str());
    cursor.indent_level++;
    cursor.elide_type.push(false);
    cursor.elide_value.push(true);
    err << emit_hoisted_decls(node);
    cursor.elide_type.pop();
    cursor.elide_value.pop();
    err << cursor.emit_gap();
  }

  for (auto child : node->statements) {
    // We may need to insert input port bindings before any statement that
    // could include a call expression. We search the tree for calls and emit
    // those bindings here.
    if (!child->as<CNodeCompound>()) {
      err << emit_call_arg_bindings(node, child);
    }
    err << Emitter(cursor).emit_dispatch(child);
    err << cursor.emit_gap();
  }

  if (auto child = node->child("rdelim")) {
    cursor.indent_level--;
    err << cursor.emit_replacement(child, "%s", rdelim.c_str());
  }

  cursor.elide_type.pop();
  cursor.elide_value.pop();

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_bit_extract(CNodeCall* node) {
  Err err;

  auto func_name = node->node_name->get_textstr();

  auto& args = node->node_args->items;
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
    auto& targs = node->node_targs->items;
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

  err << cursor.skip_over(node);

  //----------------------------------------
  // Handle casting DONTCARE

  if (node_exp->get_text() == "DONTCARE") {
    //err << cursor.skip_over(node);

    if (node_width) {
      if (!bare_width) err << cursor.emit_print("(");
      err << emit_splice(node_width);
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
        err << emit_splice(node_exp);
        cursor.override_size.pop();
        return err;
      }
    }


    if (!bare_width) err << cursor.emit_print("(");
    if (node_width)  err << emit_splice(node_width);
    else             err << cursor.emit_print("%d", width);
    if (!bare_width) err << cursor.emit_print(")");

    err<< cursor.emit_print("'");
    err << cursor.emit_print("(");
    err << emit_splice(node_exp);
    err << cursor.emit_print(")");

    return err;
  }

  //----------------------------------------
  // Expression

  if (!bare_exp) err << cursor.emit_print("(");
  err << emit_splice(node_exp);
  if (!bare_exp) err << cursor.emit_print(")");

  //----------
  // Single bit extract

  if (width == 1) {
    err << cursor.emit_print("[");
    if (node_offset) {
      err << emit_splice(node_offset);
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
    err << emit_splice(node_width);
    if (!bare_width) err << cursor.emit_print(")");
    err << cursor.emit_print("+");
    if (!bare_offset) err << cursor.emit_print("(");
    err << emit_splice(node_offset);
    if (!bare_offset) err << cursor.emit_print(")");
    err << cursor.emit_print("-1");
    err << cursor.emit_print(":");
    if (!bare_offset) err << cursor.emit_print("(");
    err << emit_splice(node_offset);
    if (!bare_offset) err << cursor.emit_print(")");
    err << cursor.emit_print("]");
    return err;
  }

  //----------

  else if (node_width) {
    err << cursor.emit_print("[");
    if (!bare_width) err << cursor.emit_print("(");
    err << emit_splice(node_width);
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
    err << emit_splice(node_offset);
    if (!bare_offset) err << cursor.emit_print(")");
    err << cursor.emit_print(":");
    if (!bare_offset) err << cursor.emit_print("(");
    err << emit_splice(node_offset);
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

Err Emitter::emit_call_arg_bindings(CNodeCompound* node, CNode* child) {
  Err err;

  // Emit bindings for child nodes first, but do _not_ recurse into compound
  // blocks.

  for (auto gc : child) {
    if (!gc->as<CNodeCompound>()) {
      err << emit_call_arg_bindings(node, gc);
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

    auto src_class = node->ancestor<CNodeClass>();
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
      err << emit_splice(call->node_args->items[i]);
      err << cursor.emit_print(";");

      any_bindings = true;
    }
  }

  if (auto func_id = call->node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_text();

    auto src_class = node->ancestor<CNodeClass>();
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
          err << emit_splice(call->node_args->items[i]);
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

Err Emitter::emit_hoisted_decls(CNodeCompound* node) {
  Err err;

  auto old_cursor = cursor.tok_cursor;

  cursor.elide_type.push(false);
  cursor.elide_value.push(true);

  for (auto child : node) {
    if (auto exp = child->as<CNodeExpStatement>()) {
      if (auto decl = exp->child("exp")->as<CNodeDeclaration>()) {

        // Don't emit decls for localparams
        if (decl->child("const")) continue;

        auto name = decl->name;
        auto decl_type = decl->child("type");
        auto decl_name = decl->child("name");

        err << cursor.start_line();

        //err << cursor.emit_print("DECL %s", name.c_str());

        cursor.tok_cursor = decl_type->tok_begin();

        err << emit_dispatch(decl_type);
        err << cursor.emit_gap();
        err << emit_dispatch(decl_name);
        err << cursor.emit_print(";");
      }
    }

    if (auto node_for = child->as<CNodeFor>()) {
      if (auto node_decl = node_for->child("init")->as<CNodeDeclaration>()) {
        auto name = node_decl->name;
        auto decl_type = node_decl->child("type");
        auto decl_name = node_decl->child("name");

        err << cursor.start_line();

        cursor.tok_cursor = decl_type->tok_begin();

        err << emit_dispatch(decl_type);
        err << cursor.emit_gap();
        err << emit_dispatch(decl_name);
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

Err Emitter::emit_component(CNodeField* node) {
  Err err;

  err << cursor.skip_to(node->node_decl->node_type);
  err << emit_dispatch(node->node_decl->node_type);
  err << cursor.emit_gap();
  err << cursor.skip_over(node->node_decl->node_name);
  err << cursor.skip_gap();

  auto parent_class = node->ancestor<CNodeClass>();
  auto repo = parent_class->repo;
  auto component_class = repo->get_class(node->node_decl->node_type->name);
  auto component_template = component_class->node_parent->as<CNodeTemplate>();

  //----------------------------------------
  // Component modparams

  bool has_constructor_params = component_class->constructor && component_class->constructor->params.size();
  bool has_template_params = component_template != nullptr;

  if (has_template_params || has_constructor_params) {
    err << cursor.emit_print("#(");
    cursor.indent_level++;

    // Emit template arguments as module parameters
    if (has_template_params) {
      err << cursor.start_line();
      err << cursor.emit_print("// Template Parameters");

      auto args = node->node_decl->node_type->child("template_args")->as<CNodeList>();

      int param_count = component_template->params.size();
      int arg_count = args->items.size();
      assert(param_count == arg_count);

      for (int i = 0; i < param_count; i++) {
        auto param = component_template->params[i];
        auto arg = args->items[i];

        auto param_name = param->name;

        err << cursor.start_line();
        err << cursor.emit_print(".%.*s(", param_name.size(), param_name.data());
        err << emit_splice(arg);
        err << cursor.emit_print("),");
      }
    }

    // Emit constructor arguments as module parameters
    if (has_constructor_params) {
      err << cursor.start_line();
      err << cursor.emit_print("// Constructor Parameters");

      auto params = component_class->constructor->params;

      // Find the initializer node IN THE PARENT INIT LIST for the component
      // and extract arguments
      assert(parent_class->constructor->node_init);
      CNodeList* args = nullptr;
      for (auto init : parent_class->constructor->node_init->items) {
        if (init->child("name")->get_text() == node->node_decl->node_name->get_text()) {
          args = init->child("value")->as<CNodeList>();
        }
      }

      assert(params.size() == args->items.size());

      for (auto i = 0; i < params.size(); i++) {
        auto param = params[i];
        auto arg = args->items[i];
        auto param_name = param->name;

        err << cursor.start_line();
        err << cursor.emit_print(".%.*s(", param_name.size(), param_name.data());
        err << emit_splice(arg);
        err << cursor.emit_print("),");
      }
    }

    // Remove trailing comma from port list
    if (cursor.at_comma) {
      err << cursor.emit_backspace();
    }

    cursor.indent_level--;
    err << cursor.start_line();
    err << cursor.emit_print(") ");
  }

  //----------------------------------------
  // Component name

  //err << cursor.emit_print(" ");
  err << emit_splice(node->node_decl->node_name);

  //----------------------------------------
  // Port list

  err << cursor.emit_print("(");
  cursor.indent_level++;

  auto field_name = node->name;

  if (class_needs_tick(component_class)) {
    err << cursor.start_line();
    err << cursor.emit_print("// Global clock");
    err << cursor.start_line();
    err << cursor.emit_print(".clock(clock),");
  }

  if (component_class->input_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// Input signals");
    for (auto f : component_class->input_signals) {
      auto port_name = f->name;

      err << cursor.start_line();
      err << cursor.emit_print(".%s", port_name.c_str());
      err << cursor.emit_print("(");
      err << cursor.emit_print("%s_%s", field_name.c_str(), port_name.c_str());
      err << cursor.emit_print("),");
    }
  }

  if (component_class->output_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// Output signals");
    for (auto f : component_class->output_signals) {
      auto port_name = f->name;
      err << cursor.start_line();
      err << cursor.emit_print(".%s(%s_%s),", port_name.c_str(), field_name.c_str(), port_name.c_str());
    }
  }

  if (component_class->output_registers.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// Output registers");
    for (auto f : component_class->output_registers) {
      auto port_name = f->name;
      err << cursor.start_line();
      err << cursor.emit_print(".%s(%s_%s),", port_name.c_str(), field_name.c_str(), port_name.c_str());
    }
  }

  for (auto m : component_class->all_functions) {
    //if (m->is_constructor()) continue;
    if (!m->is_public) continue;
    if (m->method_type == MT_INIT) continue;
    if (m->internal_callers.size()) continue;
    if (m->params.empty() && !m->has_return()) continue;

    auto func_name = m->name;

    err << cursor.start_line();
    err << cursor.emit_print("// %s() ports", func_name.c_str());

    for (auto param : m->params) {
      auto param_name = param->name;

      err << cursor.start_line();
      err << cursor.emit_print(".%s_%s(%s_%s_%s),",
        func_name.c_str(), param_name.c_str(),
        field_name.c_str(), func_name.c_str(), param_name.c_str());
    }

    if (m->has_return()) {
      err << cursor.start_line();
      err << cursor.emit_print(".%s_ret(%s_%s_ret),",
        func_name.c_str(),
        field_name.c_str(), func_name.c_str());
    }
  }

  // Remove trailing comma from port list
  if (cursor.at_comma) {
    err << cursor.emit_backspace();
  }

  cursor.indent_level--;
  err << cursor.start_line();
  err << cursor.emit_print(")");
  err << emit_dispatch(node->node_semi);

  //----------------------------------------
  // Binding variables

  // Swap template arguments with the values from the template
  // instantiation.
  std::map<std::string, std::string> replacements;
  cursor.id_map.push(cursor.id_map.top());

  if (has_template_params) {
    auto args = node->node_decl->node_type->child("template_args")->as<CNodeList>();

    int param_count = component_template->params.size();
    int arg_count = args->items.size();
    assert(param_count == arg_count);

    for (int i = 0; i < param_count; i++) {
      auto param = component_template->params[i];
      auto arg = args->items[i];

      auto param_name = param->name;
      cursor.id_map.top()[param_name] = arg->get_textstr();
    }

  }

  for (auto m : component_class->all_functions) {
    //if (m->is_constructor()) continue;
    if (!m->is_public) continue;
    if (m->method_type == MT_INIT) continue;
    if (m->internal_callers.size()) continue;
    if (m->params.empty() && !m->has_return()) continue;

    auto func_name = m->name;

    for (auto param : m->params) {
      auto param_name = param->name;
      err << cursor.start_line();
      err << emit_splice(param->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s_%s;",
        field_name.c_str(), func_name.c_str(), param_name.c_str());
    }

    if (m->has_return()) {
      err << cursor.start_line();
      err << emit_splice(m->child("return_type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s_ret;",
        field_name.c_str(), func_name.c_str());
    }
  }

  if (component_class->input_signals.size()) {
    for (auto f : component_class->input_signals) {
      auto port_name = f->name;
      err << cursor.start_line();
      err << emit_splice(f->child("decl")->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s;", field_name.c_str(), port_name.c_str());
    }
  }

  if (component_class->output_signals.size()) {
    for (auto f : component_class->output_signals) {
      auto port_name = f->name;
      err << cursor.start_line();
      err << emit_splice(f->child("decl")->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s;", field_name.c_str(), port_name.c_str());
    }
  }

  if (component_class->output_registers.size()) {
    for (auto f : component_class->output_registers) {
      auto port_name = f->name;
      err << cursor.start_line();
      err << emit_splice(f->child("decl")->child("type"));
      err << cursor.emit_print(" ");
      err << cursor.emit_print("%s_%s;", field_name.c_str(), port_name.c_str());
    }
  }

  //----------------------------------------
  // Done

  cursor.id_map.pop();

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_module_ports(CNodeClass* node) {
  Err err;

  if (class_needs_tick(node)) {
    err << cursor.start_line();
    err << cursor.emit_print("// global clock");
    err << cursor.start_line();
    err << cursor.emit_print("input logic clock,");
  }

  if (node->input_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// input signals");
    for (auto f : node->input_signals) {
      err << emit_field_ports(f, false);
    }
  }

  if (node->output_signals.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// output signals");
    for (auto f : node->output_signals) {
      err << emit_field_ports(f, true);
    }
  }

  if (node->output_registers.size()) {
    err << cursor.start_line();
    err << cursor.emit_print("// output registers");
    for (auto f : node->output_registers) {
      err << emit_field_ports(f, true);
    }
  }

  for (auto f : node->all_functions) {
    err << emit_function_ports(f);
  }

  // Remove trailing comma from port list
  if (cursor.at_comma) {
    err << cursor.emit_backspace();
  }

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_function_ports(CNodeFunction* f) {
  Err err;

  auto fname = f->name;
  auto rtype = f->child("return_type");

  if (!f->is_public) return err;
  if (f->internal_callers.size()) return err;
  if (!f->params.size() && rtype->get_text() == "void") return err;

  err << cursor.start_line();
  err << cursor.emit_print("// %s() ports", fname.c_str());

  for (auto param : f->params) {
    auto pname = param->name;
    auto ptype = param->child("type");

    err << cursor.start_line();
    err << cursor.emit_print("input ");
    err << emit_splice(ptype);
    err << cursor.emit_print(" %s_%s", fname.c_str(), pname.c_str());
    err << cursor.emit_print(",");

  }

  if (rtype->get_text() != "void") {
    err << cursor.start_line();
    err << cursor.emit_print("output ");
    err << emit_splice(rtype);
    err << cursor.emit_print(" %s_ret,", fname.c_str());
  }

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_field_ports(CNodeField* f, bool is_output) {
  Err err;

  auto fname = f->name;

  err << cursor.start_line();
  if (is_output) {
    err << cursor.emit_print("output ");
  }
  else {
    err << cursor.emit_print("input ");
  }

  err << emit_splice(f->node_decl->child("type"));
  err << cursor.emit_print(" %s,", fname.c_str());

  return err;
}










































//------------------------------------------------------------------------------

Err Emitter::emit_init(CNodeFunction* node) {
  Err err;

  // FIXME not using node_init yet

  for (auto param : node->node_params) {
    auto decl = param->as<CNodeDeclaration>();
    if (!decl) continue;

    auto decl_const = decl->child("const");
    auto decl_type  = decl->child("type");
    auto decl_name  = decl->child("name");
    auto decl_eq    = decl->child("eq");
    auto decl_value = decl->child("value");

    assert(decl_type);
    assert(decl_name);
    assert(decl_eq);
    assert(decl_value);

    auto old_cursor = cursor.tok_cursor;
    if (decl_const) {
      cursor.tok_cursor = decl_const->tok_begin();
    }
    else {
      cursor.tok_cursor = decl_type->tok_begin();
    }

    err << cursor.start_line();
    err << cursor.emit_print("parameter ");

    if (decl_const) {
      err << emit_dispatch(decl_const);
      err << cursor.emit_gap();
    }

    err << cursor.skip_over(decl_type);
    err << cursor.skip_gap();

    err << emit_dispatch(decl_name);
    err << cursor.emit_gap();

    err << emit_dispatch(decl_eq);
    err << cursor.emit_gap();

    err << emit_dispatch(decl_value);
    err << cursor.emit_print(";");

    cursor.tok_cursor = old_cursor;
  }

  err << cursor.start_line();
  err << cursor.emit_print("initial ");
  err << cursor.skip_to(node->node_body);
  err << emit_block(node->node_body, "begin", "end");

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_always_comb(CNodeFunction* node) {
  Err err;

  auto func_name = node->name;

  cursor.id_map.push(cursor.id_map.top());
  for (auto node_param : node->node_params->items) {
    auto param = node_param->as<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->name;
    cursor.id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << cursor.emit_replacement(node->node_type, "always_comb begin :");
  err << cursor.emit_gap();
  err << emit_dispatch(node->node_name);
  err << cursor.emit_gap();

  err << cursor.skip_over(node->node_params);
  err << cursor.emit_gap();

  if (node->node_const) {
    err << cursor.skip_over(node->node_const);
    err << cursor.skip_gap();
  }

  err << emit_block(node->node_body, "", "end");

  cursor.id_map.pop();

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_always_ff(CNodeFunction* node) {
  Err err;

  auto func_name = node->name;

  cursor.id_map.push(cursor.id_map.top());
  for (auto node_param : node->node_params) {
    auto param = node_param->as<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->name;
    cursor.id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << cursor.emit_replacement(node->node_type, "always_ff @(posedge clock) begin :");
  err << cursor.emit_gap();
  err << emit_dispatch(node->node_name);
  err << cursor.emit_gap();

  err << cursor.skip_over(node->node_params);
  err << cursor.emit_gap();

  if (node->node_const) {
    err << cursor.skip_over(node->node_const);
    err << cursor.skip_gap();
  }

  err << emit_block(node->node_body, "", "end");

  cursor.id_map.pop();

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_func(CNodeFunction* node) {
  Err err;

  err << cursor.emit_print("function ");

  err << emit_dispatch(node->node_type);
  err << cursor.emit_gap();

  err << emit_dispatch(node->node_name);
  err << cursor.emit_gap();

  err << emit_dispatch(node->node_params);

  if (node->node_const) {
    err << cursor.emit_gap();
    err << cursor.comment_out(node->node_const);
  }

  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  err << emit_block(node->node_body, "", "endfunction");

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_task(CNodeFunction* node) {
  Err err;

  err << cursor.emit_print("task automatic ");

  err << cursor.skip_over(node->node_type);
  err << cursor.skip_gap();

  err << emit_dispatch(node->node_name);
  err << cursor.emit_gap();

  err << emit_dispatch(node->node_params);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  if (node->node_const) {
    err << cursor.comment_out(node->node_const);
    err << cursor.emit_gap();
  }

  err << emit_block(node->node_body, "", "endtask");

  return err << cursor.check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_func_binding_vars(CNodeFunction* node) {
  Err err;

  for (auto& param : node->params) {

    auto param_type = param->child("type")->as<CNodeType>();
    auto param_name = param->child("name")->as<CNodeIdentifier>();

    err << cursor.start_line();
    err << emit_splice(param_type);
    err << cursor.emit_print(" %s_", node->name.c_str());
    err << emit_splice(param_name);
    err << cursor.emit_print(";");
  }

  if (node->node_type->get_text() != "void") {
    err << cursor.start_line();
    err << emit_splice(node->node_type);
    err << cursor.emit_print(" %s_ret;", node->name.c_str());
  }

  return err;
}

//==============================================================================
