#include "Emitter.hpp"

#include <stdio.h>
#include <stdarg.h>

#include "metron/Cursor.hpp"
#include "metron/MtUtils.h"
#include "metron/nodes/CNodeReturn.hpp"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeAssignment.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeCompound.hpp"
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

Err Emitter::emit_splice(CNode* n) {
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
  Err err = check_at(node);

  if (node->child_head) {
    err << emit_children(node);
  }
  else {
    err << cursor.emit_span(node->tok_begin(), node->tok_end());
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_children(CNode* n) {
  Err err;
  for (auto c = n->child_head; c; c = c->node_next) {
    err << emit_dispatch2(c);
  }
  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_everything() {
  Err err;

  // Emit header
  for (auto lex_cursor = cursor.source_file->context.lexemes.data(); lex_cursor < cursor.tok_cursor->lex; lex_cursor++) {
    err << cursor.emit_lexeme(lex_cursor);
  }

  // Emit body
  err << emit_dispatch(cursor.source_file->context.root_node);

  // Emit footer (everything in the gap after the translation unit)
  err << cursor.emit_gap();

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_dispatch(CNode* node) {
  Err err = check_at(node);

  if (auto n = node->as<CNodeAccess>()) return comment_out(node);

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
  err << emit_gap(node);
  return err;
}

//------------------------------------------------------------------------------
// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

Err Emitter::emit(CNodeAssignment* node) {
  Err err = check_at(node);

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func  = node->ancestor<CNodeFunction>();
  auto node_field = resolve_field(node_class, node->node_lhs);

  err << emit_dispatch2(node->node_lhs);

  // If we're in a tick, turn = into <=
  std::string assign = node_func->method_type == MT_TICK && node_field ? "<=" : "=";

  if (node->node_op->get_text() == "=") {
    err << emit_replacement2(node->node_op, assign);
  } else {
    auto lhs_text = node->node_lhs->get_textstr();
    auto s = str_printf("%s %s %c", assign.c_str(), lhs_text.c_str(), node->node_op->get_text()[0]);
    err << emit_replacement2(node->node_op, s);
  }

  err << emit_dispatch(node->node_rhs);

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeBuiltinType* node) {
  Err err = check_at(node);

  if (!node->node_targs) {
    return emit_default(node);
  }

  auto node_exp = node->node_targs->items[0];

  if (auto node_const_int = node_exp->as<CNodeConstInt>()) {
    auto width = atoi(node_const_int->text_begin());
    if (width == 1) {
      // logic<1> -> logic
      err << emit_raw2(node->node_name);
      err << skip_over(node->node_targs);
    } else {
      // logic<N> -> logic[N-1:0]
      err << emit_raw2(node->node_name);
      err << emit_replacement2(node->node_targs->node_ldelim, "[");
      err << emit_replacement2(node_const_int, "%d:0", width - 1);
      err << emit_replacement(node->node_targs->node_rdelim, "]");
    }
  } else if (auto node_identifier = node_exp->as<CNodeIdentifier>()) {
    // logic<CONSTANT> -> logic[CONSTANT-1:0]
    err << emit("@[@-1:0]", node->node_name, node_identifier);
    err << skip_over(node);

  } else {
    // logic<exp> -> logic[(exp)-1:0]
    err << emit("@[(@)-1:0]", node->node_name, node_exp);
    err << skip_over(node);
  }

  if (node->node_scope) {
    err << cursor.skip_gap();
    err << skip_over(node->node_scope);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

CNodeFunction* resolve_func(CNode* node) {
  if (auto node_call = node->as<CNodeCall>()) {
    auto src_class = node->ancestor<CNodeClass>();

    if (auto func_path = node_call->node_name->as<CNodeFieldExpression>()) {
      auto field_name = func_path->node_path->get_textstr();
      auto func_name = func_path->node_name->get_textstr();

      auto src_field = src_class->get_field(field_name);
      auto dst_class = src_class->repo->get_class(src_field->node_decl->node_type->name);
      auto dst_func = dst_class->get_function(func_name);
      return dst_func;
    }

    if (auto func_id = node_call->node_name->as<CNodeIdentifier>()) {
      auto dst_func = src_class->get_function(func_id->get_text());
      return dst_func;
    }
  }

  return nullptr;
}

//------------------------------------------------------------------------------

Err Emitter::emit_cat(CNodeCall* node) {
  Err err;
  err << skip_to(node->node_args);
  for (auto child : node->node_args) {
    if (child->tag_is("ldelim")) {
      err << emit_replacement2(child, "{");
    } else if (child->tag_is("rdelim")) {
      err << emit_replacement2(child, "}");
    } else {
      err << emit_dispatch2(child);
    }
  }
  return err << check_done(node);
}

//----------------------------------------

Err Emitter::emit_sra(CNodeCall* node) {
  Err err;
  auto args = node->node_args->as<CNodeList>();
  auto lhs = args->items[0];
  auto rhs = args->items[1];

  err << emit("($signed(@) >>> @)", lhs, rhs);
  err << skip_over(node);

  return err;
}

//----------------------------------------
// Convert "dup<15>(x)" to "{15 {x}}"

Err Emitter::emit_dup(CNodeCall* node) {
  Err err;
  auto node_val = node->node_targs->child("arg");
  auto node_exp = node->node_args->child("exp");
  err << emit("{@ {@}}", node_val, node_exp);
  err << skip_over(node);
  return err;
}

//----------------------------------------
// We don't actually "call" into submodules, so we can just comment this
// call out and patch in the return type binding if needed.

Err Emitter::emit_submod_call(CNodeCall* node) {
  Err err;

  auto func_path = node->node_name->as<CNodeFieldExpression>();
  auto dst_func = resolve_func(node);
  if (dst_func->child("return_type")->name == "void") {
    err << comment_out(node);
  } else {
    err << emit("@_@_ret", func_path->node_path, func_path->node_name);
    err << skip_over(node);
  }
  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeCall* node) {
  Err err = check_at(node);

  if (is_bit_extract(node)) return emit_bit_extract(node);

  auto src_class = node->ancestor<CNodeClass>();
  auto src_func = node->ancestor<CNodeFunction>();

  if (auto func_path = node->node_name->as<CNodeFieldExpression>()) {
    return emit_submod_call(node);
  }

  if (auto func_id = node->node_name->as<CNodeIdentifier>()) {

    // Some builtin functions just need to be renamed.
    if (id_map.top().contains(func_id->name)) {
      auto replacement = id_map.top()[func_id->name];
      err << emit_replacement2(node->node_name, replacement);
      err << skip_over2(node->node_targs);
      err << emit_dispatch(node->node_args);
      return err << check_done(node);
    }

    // Some builtin functions have special representations in Verilog.
    if (func_id->name == "cat") return emit_cat(node);
    if (func_id->name == "dup") return emit_dup(node);
    if (func_id->name == "sra") return emit_sra(node);

    //----------
    // Not a special builtin call

    auto dst_func = src_class->get_function(func_id->get_text());
    auto dst_params = dst_func->node_params;
    auto src_mtype = src_func->method_type;
    auto dst_mtype = dst_func->method_type;

    // Replace call with return binding variable if the callee is a tock

    if (dst_mtype == MT_INIT) return emit_default(node);
    if (dst_mtype == MT_FUNC) return emit_default(node);
    if (src_mtype == MT_TICK && dst_mtype == MT_TICK) return emit_default(node);

    // FIXME: Exiting after doing this can't be right...
    // check tock_task.h
    if (dst_mtype == MT_TOCK) {
      auto dst_name = dst_func->name;
      err << emit_replacement2(node, "%s_ret", dst_name.c_str());
      return err;
    }

    if (src_mtype == MT_TOCK) {

      // Emit writes to binding variables in place of the function arguments
      err << comment_out(node);

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
      return err << check_done(node);
    }
  }

  assert(false);
  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeClass* node) {
  Err err = check_at(node);

  auto node_class = node->child<CNodeKeyword>();
  auto node_name = node->child("name");
  auto node_body = node->child("body");
  auto node_semi = node->child("semi");

  err << emit_replacement2(node_class, "module");
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
      err << emit_template_parameter_list(node);
      err << skip_over(child);
      err << emit_gap(child);
    } else if (child->get_text() == "}") {
      cursor.indent_level--;
      err << emit_replacement2(child, "endmodule");
    } else {
      err << emit_dispatch2(child);
    }
  }

  err << cursor.skip_gap();
  err << skip_over(node_semi);

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeClassType* node) {
  Err err;

  if (node->node_targs) {
    err << emit_dispatch2(node->child("name"));
    err << skip_over(node->node_targs);
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
  Err err = check_at(node);

  std::string body = node->get_textstr();

  // FIXME what was this for?
  // This was for forcing enum constants to the size of the enum type
  int size_cast = override_size.top();
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
    err << emit_replacement(node, "%d'h%s", size_cast, body.c_str() + prefix_count);
  } else if (body.starts_with("0b")) {
    prefix_count = 2;
    if (!size_cast) size_cast = (int)body.size() - 2 - spacer_count;
    err << emit_replacement(node, "%d'b%s", size_cast, body.c_str() + prefix_count);
  } else {
    if (size_cast) {
      err << emit_replacement(node, "%d'd%s", size_cast, body.c_str() + prefix_count);
    }
    else {
      err << emit_replacement(node, "%s", body.c_str() + prefix_count);
    }
  }



  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeDeclaration* node) {
  Err err = check_at(node);

  // Check for const char*
  if (node->node_const) {
    if (node->node_type->child("name")->get_text() == "char") {
      if (node->node_type->child("star")) {
        return emit_replacement(node, "{{const char*}}");
      }
    }

    err << cursor.emit_print("parameter ");

    for (auto c = node->child_head; c; c = c->node_next) {
      if (c->as<CNodeType>()) {
        err << skip_over2(c);
      } else {
        err << emit_dispatch2(c);
      }
    }

    return err << check_done(node);
  }

  for (auto child : node) {
    if (elide_type.top()) {
      if (child->as<CNodeType>()) {
        err << skip_over(child);
        if (child->node_next) err << cursor.skip_gap();
        continue;
      }
    }

    err << emit_dispatch2(child);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeEnum* node) {
  Err err = check_at(node);

  // Extract enum bit width, if present.
  override_size.push(32);
  if (node->node_type) {
    if (auto targs = node->node_type->node_targs->as<CNodeList>()) {
      override_size.top() = atoi(targs->items[0]->text_begin());
    }
  }

  if (!node->node_decl) {
    err << cursor.emit_print("typedef ");
  }

  err << emit_dispatch2(node->node_enum);
  err << skip_over2(node->node_class);
  err << skip_over2(node->node_name);
  err << skip_over2(node->node_colon);
  err << emit_dispatch2(node->node_type);
  err << emit_dispatch2(node->node_body);
  err << emit_dispatch2(node->node_decl);

  if (node->node_name) {
    err << cursor.emit_print(" ");
    err << emit_splice(node->node_name);
  }

  err << emit_dispatch(node->node_semi);

  override_size.pop();

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodePrefixExp* node) {
  Err err = check_at(node);

  auto node_op = node->child("prefix");
  auto op = node_op->get_text();

  if (op != "++" && op != "--") {
    return emit_default(node);
  }

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func = node->ancestor<CNodeFunction>();
  auto node_rhs = node->child("rhs");
  auto node_field = resolve_field(node_class, node_rhs);

  std::string assign = node_func->method_type == MT_TICK && node_field ? " <= " : " = ";
  std::string suffix = op == "++" ? " + 1" : " - 1";

  err << skip_over(node);
  err << emit_splice(node_rhs);
  err << cursor.emit_print(assign.c_str());
  err << emit_splice(node_rhs);
  err << cursor.emit_print(suffix.c_str());

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeSuffixExp* node) {
  Err err = check_at(node);

  auto node_class = node->ancestor<CNodeClass>();
  auto node_func = node->ancestor<CNodeFunction>();
  auto node_op = node->child("suffix");
  auto node_lhs = node->child("lhs");
  auto node_field = resolve_field(node_class, node_lhs);

  auto op = node_op->get_text();

  if (op == "++" || op == "--") {
    err << skip_over(node);
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

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeExpStatement* node) {
  Err err = check_at(node);

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
      err << comment_out(node);
    }
    else {
      err << emit_default(node);
    }
  }
  else if (auto keyword = node->node_exp->as<CNodeKeyword>()) {
    if (keyword->get_text() == "break") {
      err << comment_out(node);
    }
    else {
      err << emit_default(node);
    }
  }
  else if (auto decl = node->node_exp->as<CNodeDeclaration>()) {
    if (elide_type.top() && decl->node_value == nullptr) {
      err << skip_over(node);
    }
    else {
      err << emit_default(node);
    }
  }
  else {
    err << emit_default(node);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeField* node) {
  Err err = check_at(node);

  //----------------------------------------
  // Ports don't go in the class body.
  // FIXME should we disallow public components?

  if (node->is_public && !node->is_component() &&
      !node->node_decl->is_localparam()) {
    return err << skip_over(node);
  }

  //----------------------------------------

  if (node->is_const_char()) {
    err << skip_to(node->node_decl->node_name);

    err << cursor.emit_print("localparam string ");
    err << emit_dispatch2(node->node_decl->node_name);
    err << emit_dispatch2(node->node_decl->node_array);
    err << emit_dispatch2(node->node_decl->node_eq);
    err << emit_dispatch2(node->node_decl->node_value);
    err << emit_dispatch2(node->node_semi);

    return err << check_done(node);
  }

  //----------------------------------------

  if (node->node_decl->node_const) {
    // Localparam
    bool in_namespace = node->ancestor<CNodeNamespace>() != nullptr;
    err << cursor.emit_print(in_namespace ? "parameter " : "localparam ");

    err << comment_out2(node->node_decl->node_static);
    err << comment_out2(node->node_decl->node_const);

    err << emit_dispatch2(node->node_decl->node_type);
    err << emit_dispatch2(node->node_decl->node_name);
    err << emit_dispatch2(node->node_decl->node_array);
    err << emit_dispatch2(node->node_decl->node_eq);
    err << emit_dispatch2(node->node_decl->node_value);
    err << emit_dispatch2(node->node_semi);

    return err << check_done(node);
  }

  //----------------------------------------

  auto node_builtin = node->node_decl->node_type->as<CNodeBuiltinType>();
  if (node_builtin) {
    err << skip_to(node->node_decl->node_type);

    err << emit_dispatch2(node->node_decl->node_type);
    err << emit_dispatch2(node->node_decl->node_name);
    err << emit_dispatch2(node->node_decl->node_array);
    err << emit_dispatch2(node->node_decl->node_eq);
    err << emit_dispatch2(node->node_decl->node_value);
    err << emit_dispatch2(node->node_semi);

    return err << check_done(node);
  }

  //----------------------------------------

  if (node->is_struct()) {
    return emit_raw(node);
  }

  //----------------------------------------

  if (node->is_component()) {
    return emit_component(node);
  }

  //----------------------------------------

  assert(false);
  return err << check_done(node);
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz if the field refers to a submodule port,
// so that it instead refers to a glue expression.

Err Emitter::emit(CNodeFieldExpression* node) {
  Err err = check_at(node);

  auto node_func = node->ancestor<CNodeFunction>();
  auto node_class = node->ancestor<CNodeClass>();
  auto node_path = node->child("field_path");
  auto node_name = node->child("identifier");

  auto field = node_class->get_field(node_path->get_text());

  if (field && field->node_decl->_type_struct) {
    err << emit_default(node);
    return err;
  }

  if (field) {
    auto field = node->get_textstr();
    for (auto& c : field) {
      if (c == '.') c = '_';
    }
    err << emit_replacement(node, field);
  } else {
    err << emit_default(node);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeFunction* node) {
  Err err = check_at(node);

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
  Err err = check_at(node);
  auto text = node->get_textstr();

  auto found = id_map.top().find(text);

  if (found != id_map.top().end()) {
    return emit_replacement(node, (*found).second);
  }
  else if (preproc_vars.contains(text)) {
    err << cursor.emit_print("`");
    return emit_default(node);
  }
  else {
    return emit_default(node);
  }
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeIf* node) {
  Err err = check_at(node);

  err << emit_dispatch2(node->node_kw_if);
  err << emit_dispatch2(node->node_cond);
  err << emit_dispatch2(node->node_then);

  if (node->node_kw_else) {
    err << emit_dispatch2(node->node_kw_else);
    err << emit_dispatch2(node->node_else);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeKeyword* node) {
  auto text = node->get_text();

  if (text == "static" || text == "const" || text == "break") {
    return comment_out(node);
  }

  if (text == "nullptr") {
    return emit_replacement(node, "\"\"");
  }

  if (text == "if" || text == "else" || text == "default" || text == "for" || "enum") {
    return emit_raw(node);
  }

  assert(false);
  return ERR("Don't know how to handle this keyword - %s\n", node->get_textstr().c_str());
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeNamespace* node) {
  Err err = check_at(node);
  auto node_namespace = node->node_namespace;
  auto node_name      = node->node_name;
  auto node_fields    = node->node_fields;
  auto node_semi      = node->node_semi;

  err << emit_replacement2(node_namespace, "package");

  err << emit_raw(node_name);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  for (auto f : node_fields) {
    if (f->tag_is("ldelim")) {
      err << skip_over(f);
      err << cursor.emit_gap();
    }
    else if (f->tag_is("rdelim")) {
      err << emit_replacement2(f, "endpackage");
    }
    else {
      err << emit_dispatch2(f);
    }
  }

  // Don't need semi after namespace in Verilog
  err << skip_over(node_semi);

  return err << check_done(node);
}

//------------------------------------------------------------------------------

void replace(std::string& text, const std::string& a, const std::string& b) {
  size_t pos;
  while ((pos = text.find(a)) != std::string::npos) {
    text.replace(pos, a.size(), b);
  }
}

Err Emitter::emit(CNodePreproc* node) {
  Err err = check_at(node);

  if (node->tag_is("preproc_define")) {
    err << emit_default(node);
    auto name = node->child("name")->get_textstr();
    preproc_vars.insert(name);
  }

  else if (node->tag_is("preproc_include")) {
    auto text = node->get_textstr();
    replace(text, "#include", "`include");
    replace(text, ".h", ".sv");
    err << emit_replacement(node, text);
  }

  else {
    err << emit_default(node);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeQualifiedIdentifier* node) {
  Err err = check_at(node);

  bool elide_scope = false;

  if (node->node_scope->get_text() == "std") elide_scope = true;

  auto node_class = node->ancestor<CNodeClass>();

  // Verilog doesn't like the scope on the enums
  if (node_class && node_class->get_enum(node->node_scope->get_text())) {
    elide_scope = true;
  }

  if (elide_scope) {
    err << skip_to(node->node_name);
    err << emit_dispatch2(node->node_name);
  }
  else {
    err << emit_dispatch2(node->node_scope);
    err << emit_dispatch2(node->node_colon);
    err << emit_dispatch2(node->node_name);
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeReturn* node) {
  Err err = check_at(node);

  auto func = node->ancestor<CNodeFunction>();
  auto fname = func->name;

  assert(node->node_val);

  err << skip_over2(node->node_ret);

  if (func->should_emit_as_task() || func->should_emit_as_func()) {
    err << cursor.emit_print("%s = ", fname.c_str());
  }
  else {
    err << cursor.emit_print("%s_ret = ", fname.c_str());
  }

  err << emit_dispatch2(node->node_val);
  err << emit_dispatch2(node->node_semi);

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeStruct* node) {
  Err err = check_at(node);

  err << emit_replacement2(node->node_struct, "typedef struct packed");
  err << skip_over2(node->node_name);
  err << emit_dispatch2(node->node_body);
  err << cursor.emit_print(" ");
  err << emit_splice(node->node_name);
  err << cursor.emit_gap();
  err << emit_dispatch2(node->node_semi);

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeSwitch* node) {
  Err err = check_at(node);

  err << emit_replacement2(node->node_switch, "case");
  err << emit_dispatch2(node->node_cond);

  for (auto child : node->node_body) {
    if (child->tag_is("ldelim")) {
      err << skip_over(child);
    }
    else if (child->tag_is("rdelim")) {
      err << emit_replacement(child, "endcase");
    }
    else {
      err << emit_dispatch(child);
    }

    if (child->node_next) err << cursor.emit_gap();
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeCase* node) {
  Err err = check_at(node);

  err << skip_over2(node->node_case);
  err << emit_dispatch2(node->node_cond);

  if (node->node_body) {
    err << emit_dispatch2(node->node_colon);
    err << emit_dispatch2(node->node_body);
  }
  else {
    err << emit_replacement2(node->node_colon, ",");
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeDefault* node) {

  Err err = check_at(node);

  auto node_default = node->child("default");
  auto node_colon   = node->child("colon");
  auto node_body    = node->child("body");

  err << emit_dispatch2(node_default);

  if (node_body) {
    err << emit_dispatch2(node_colon);
    err << emit_dispatch(node_body);
  }
  else {
    err << emit_replacement(node_colon, ",");
  }

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeTemplate* node) {
  Err err = check_at(node);

  err << skip_over(node->node_template);
  err << cursor.skip_gap();

  err << skip_over(node->node_params);
  err << cursor.skip_gap();

  err << emit(node->node_class);

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit(CNodeUsing* node) {
  return emit_replacement2(node, "import %s::*;", node->child("name")->get_textstr().c_str());
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
    err << emit_dispatch2(param->node_name);
    err << emit_dispatch2(param->node_array);
    err << emit_dispatch2(param->node_eq);
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

  elide_type.push(true);
  elide_value.push(false);

  if (auto child = node->child("ldelim")) {
    err << emit_replacement(child, ldelim);
    cursor.indent_level++;
    elide_type.push(false);
    elide_value.push(true);
    err << emit_hoisted_decls(node);
    elide_type.pop();
    elide_value.pop();
    err << cursor.emit_gap();
  }

  for (auto child : node->statements) {
    // We may need to insert input port bindings before any statement that
    // could include a call expression. We search the tree for calls and emit
    // those bindings here.
    if (!child->as<CNodeCompound>()) {
      err << emit_call_arg_bindings(node, child);
    }
    err << emit_dispatch2(child);
  }

  if (auto child = node->child("rdelim")) {
    cursor.indent_level--;
    err << emit_replacement(child, rdelim);
  }

  elide_type.pop();
  elide_value.pop();

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

  err << skip_over(node);

  //----------------------------------------
  // Handle casting DONTCARE

  if (node_exp->get_text() == "DONTCARE") {
    if (node_width) {
      if (bare_width) {
        err << emit("@'('x)", node_width);
      }
      else {
        err << emit("(@)'('x)", node_width);
      }
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
        override_size.push(width);
        err << emit_splice(node_exp);
        override_size.pop();
        return err;
      }
    }


    if (!bare_width) err << cursor.emit_print("(");
    if (node_width)  err << emit_splice(node_width);
    else             err << cursor.emit_print("%d", width);
    if (!bare_width) err << cursor.emit_print(")");

    err << cursor.emit_print("'");
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

  elide_type.push(false);
  elide_value.push(true);

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

        err << emit_dispatch2(decl_type);
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

        err << emit_dispatch2(decl_type);
        err << emit_dispatch(decl_name);
        err << cursor.emit_print(";");

      }
    }

  }

  elide_type.pop();
  elide_value.pop();

  cursor.tok_cursor = old_cursor;

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::emit_component(CNodeField* node) {
  Err err;

  err << skip_to(node->node_decl->node_type);
  err << emit_dispatch2(node->node_decl->node_type);
  err << skip_over2(node->node_decl->node_name);

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

      auto args = node->node_decl->node_type->node_targs->as<CNodeList>();

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
  id_map.push(id_map.top());

  if (has_template_params) {
    auto args = node->node_decl->node_type->node_targs->as<CNodeList>();

    int param_count = component_template->params.size();
    int arg_count = args->items.size();
    assert(param_count == arg_count);

    for (int i = 0; i < param_count; i++) {
      auto param = component_template->params[i];
      auto arg = args->items[i];

      auto param_name = param->name;
      id_map.top()[param_name] = arg->get_textstr();
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

  id_map.pop();

  return err << check_done(node);
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
    err << emit_dispatch2(decl_const);
    err << skip_over2(decl_type);
    err << emit_dispatch2(decl_name);
    err << emit_dispatch2(decl_eq);
    err << emit_dispatch2(decl_value);
    err << cursor.emit_print(";");

    cursor.tok_cursor = old_cursor;
  }

  err << cursor.start_line();
  err << cursor.emit_print("initial ");
  err << skip_to(node->node_body);
  err << emit_block(node->node_body, "begin", "end");

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_always_comb(CNodeFunction* node) {
  Err err;

  auto func_name = node->name;

  id_map.push(id_map.top());
  for (auto node_param : node->node_params->items) {
    auto param = node_param->as<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->name;
    id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << emit_replacement2(node->node_type, "always_comb begin :");
  err << emit_dispatch2(node->node_name);

  err << skip_over(node->node_params);
  err << cursor.emit_gap();

  if (node->node_const) {
    err << skip_over(node->node_const);
    err << cursor.skip_gap();
  }

  err << emit_block(node->node_body, "", "end");

  id_map.pop();

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_always_ff(CNodeFunction* node) {
  Err err;

  auto func_name = node->name;

  id_map.push(id_map.top());
  for (auto node_param : node->node_params) {
    auto param = node_param->as<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->name;
    id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << emit_replacement2(node->node_type, "always_ff @(posedge clock) begin :");
  err << emit_dispatch2(node->node_name);

  err << skip_over(node->node_params);
  err << cursor.emit_gap();

  if (node->node_const) {
    err << skip_over2(node->node_const);
  }

  err << emit_block(node->node_body, "", "end");

  id_map.pop();

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_func(CNodeFunction* node) {
  Err err;

  err << cursor.emit_print("function ");
  err << emit_dispatch2(node->node_type);
  err << emit_dispatch2(node->node_name);

  if (node->node_const) {
    err << emit_dispatch2(node->node_params);
    err << comment_out(node->node_const);
    err << cursor.emit_print(";");
    err << cursor.emit_gap();
  }
  else {
    err << emit_dispatch(node->node_params);
    err << cursor.emit_print(";");
    err << cursor.emit_gap();
  }


  err << emit_block(node->node_body, "", "endfunction");

  return err << check_done(node);
}

//------------------------------------------------------------------------------

Err Emitter::emit_task(CNodeFunction* node) {
  Err err;

  err << cursor.emit_print("task automatic ");

  err << skip_over2(node->node_type);

  err << emit_dispatch2(node->node_name);

  err << emit_dispatch(node->node_params);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  err << comment_out2(node->node_const);

  err << emit_block(node->node_body, "", "endtask");

  return err << check_done(node);
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







Err Emitter::emit_gap(CNode* n) {
  if (n && n->node_next) {
    return cursor.emit_gap();
  }
  else {
    return Err();
  }
}

Err Emitter::skip_gap(CNode* n) {
  if (n && n->node_next) {
    return cursor.skip_gap();
  }
  else {
    return Err();
  }
}

//------------------------------------------------------------------------------

Err Emitter::skip_over(CNode* n) {
  if (n == nullptr) return Err();
  Err err = check_at(n);

  {
    /*
    auto begin = n->tok_begin()->text_begin();
    auto end   = (n->tok_end()-1)->text_end();

    Err err;
    for (auto c = begin; c < end; c++) {
      err << cursor.skip_char(*c);
    }
    */
    err << cursor.skip_span(n->tok_begin(), n->tok_end());
    cursor.tok_cursor = n->tok_end();
    cursor.line_elided = true;

  }

  return err << check_done(n);
}

Err Emitter::skip_over2(CNode* n) {
  if (n == nullptr) return Err();

  Err err;
  err << skip_over(n);
  err << cursor.skip_gap();

  return err;
}

Err Emitter::skip_to(CNode* n) {
  if (n == nullptr) return Err();

  Err err;
  err << cursor.skip_span(cursor.tok_cursor, n->tok_begin());
  /*
  while (cursor.tok_cursor < n->tok_begin()) {
    for (auto c = cursor.tok_cursor->text_begin(); c < cursor.tok_cursor->text_end(); c++) {
      err << cursor.skip_char(*c);
    }
    cursor.tok_cursor++;
  }
  */

  return err;
}

//------------------------------------------------------------------------------

Err Emitter::check_at(CNode* n) {

  if (cursor.tok_cursor != n->tok_begin()) {
    LOG_R("check_at - bad tok_cursor\n");
    LOG_R("  want @%.10s@\n", n->text_begin());
    LOG_R("  got  @%.10s@\n", cursor.tok_cursor->text_begin());
    assert(false);
  }

  return Err();
}

//------------------------------------------------------------------------------

Err Emitter::check_done(CNode* n) {

  auto tok_end = n->tok_end();

  if (cursor.tok_cursor < tok_end) {
    LOG_R("Token cursor was left inside the current node\n");
    assert(false);
  }

  if (cursor.tok_cursor > tok_end) {
    LOG_R("Token cursor was left past the end of the current node\n");
    assert(false);
  }

  return Err();
}

//----------------------------------------

Err Emitter::emit_replacement(CNode* n, const std::string& s) {
  return cursor.emit_replacement(n->tok_begin(), n->tok_end(), s);
}

//----------------------------------------

Err Emitter::emit_replacement2(CNode* n, const std::string& s) {
  Err err;
  err << cursor.emit_replacement(n->tok_begin(), n->tok_end(), s);
  err << emit_gap(n);
  return err;
}

Err Emitter::emit_replacement(CNode* n, const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  std::string result;
  result.resize(size + 1);
  va_start(args, fmt);
  vsnprintf(result.data(), size_t(size + 1), fmt, args);
  va_end(args);
  assert(result.back() == 0);
  result.pop_back();

  return emit_replacement(n, result);
}

Err Emitter::emit_replacement2(CNode* n, const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  std::string result;
  result.resize(size + 1);
  va_start(args, fmt);
  vsnprintf(result.data(), size_t(size + 1), fmt, args);
  va_end(args);
  assert(result.back() == 0);
  result.pop_back();

  return emit_replacement2(n, result);
}


//----------------------------------------

Err Emitter::emit_raw(CNode* n) {
  Err err;
  err << cursor.emit_span(n->tok_begin(), n->tok_end());
  return err;
}

Err Emitter::emit_raw2(CNode* n) {
  Err err;
  if (n == nullptr) return err;
  err << emit_raw(n);
  err << emit_gap(n);
  return err;
}













//==============================================================================
