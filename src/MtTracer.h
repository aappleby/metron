#pragma once

#include "Err.h"
#include "MtNode.h"

struct MtModLibrary;
struct MtContext;
struct MtField;
struct MtMethod;

//------------------------------------------------------------------------------

class MtTracer {
 public:
  MtTracer(MtModLibrary* lib, MtContext* root, bool verbose) : lib(lib), ctx_root(root), verbose(verbose) {}

  CHECK_RETURN Err log_action(MtContext* method_ctx, MtContext* dst_ctx,
                              TraceAction action, SourceRange source);

  CHECK_RETURN Err trace_method(MtContext* mod_ctx, MtMethod* method);

  CHECK_RETURN Err trace_default(MtContext* mod_ctx, MnNode node,
                                 TraceAction action = ACT_READ);

  CHECK_RETURN Err trace_identifier(MtContext* ctx, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_expression(MtContext* ctx, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_declarator(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_call(MtContext* src_ctx, MtContext* dst_ctx, MnNode node_call);

  CHECK_RETURN Err trace_sym_argument_list(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_assignment_expression(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_binary_expression(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_break_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_call_expression(MtContext* ctx, MnNode n);
  CHECK_RETURN Err trace_sym_case_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_compound_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_condition_clause(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_conditional_expression(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_declaration(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_field_expression(MtContext* ctx, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_sym_for_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_function_definition(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_if_statement(MtContext* ctx, MnNode n);
  CHECK_RETURN Err trace_sym_init_declarator(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_initializer_list(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_return_statement(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_switch_statement(MtContext* ctx, MnNode n);
  CHECK_RETURN Err trace_sym_template_argument_list(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_template_type(MtContext* ctx, MnNode node);
  CHECK_RETURN Err trace_sym_type_identifier(MtContext* ctx, MnNode node);

  MtModLibrary* lib;
  MtContext* ctx_root;
  bool verbose;
};

//------------------------------------------------------------------------------
