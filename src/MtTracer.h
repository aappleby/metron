#pragma once
#include <assert.h>

#include <string>

#include "Err.h"
#include "MtNode.h"
#include "MtUtils.h"
#include "Platform.h"

struct MtModLibrary;
struct MtContext;
struct MtField;

//------------------------------------------------------------------------------

class MtTracer {
 public:
  MtTracer(MtModLibrary* lib, MtContext* root) : lib(lib), ctx_root(root) {}

  CHECK_RETURN Err log_action(MtContext* method_ctx, MtContext* dst_ctx,
                              ContextAction action, SourceRange source);

  CHECK_RETURN Err trace_method(MtContext* mod_ctx, MtMethod* method);

  // clang-format off
  CHECK_RETURN Err trace_identifier(MtContext* ctx, MnNode node, ContextAction action);
  CHECK_RETURN Err trace_expression(MtContext* ctx, MnNode node, ContextAction action);
  CHECK_RETURN Err trace_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_declarator(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_if_statement(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_call(MtContext* src_ctx, MtContext* dst_ctx, MnNode node_call);

  CHECK_RETURN Err trace_sym_argument_list(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_binary_expression(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_break_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_call_expression(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_sym_case_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_compound_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_condition_clause(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_conditional_expression(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_declaration(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_field_expression(MtContext* ctx, MnNode node, ContextAction action);
  CHECK_RETURN Err trace_sym_function_definition(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_init_declarator(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_return_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_switch_statement(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_sym_template_argument_list(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_template_type(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_type_identifier(MtContext* ctx, MnNode node);
  // clang-format on

  MtModLibrary* lib;
  MtContext* ctx_root;
};

//------------------------------------------------------------------------------
