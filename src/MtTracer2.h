#pragma once

#include "Err.h"
#include "Platform.h"
#include "MtInstance.h"
#include "MtUtils.h"

struct MtModLibrary;
struct MtField;
struct MtMethod;
struct MnNode;

class MtTracer2 {
public:
  MtTracer2(MtModLibrary* lib, MtModuleInstance* root_inst, bool verbose);

  CHECK_RETURN Err log_action(MnNode node, MtInstance* field_inst, TraceAction action);

  CHECK_RETURN Err trace_method(MtMethod* method);

  CHECK_RETURN Err trace_identifier(MtMethodInstance* inst, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_declarator(MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_statement (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_expression(MtMethodInstance* inst, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_call      (MtMethodInstance* src_inst, MtMethodInstance* dst_inst, MnNode node_call);
  CHECK_RETURN Err trace_default   (MtMethodInstance* inst, MnNode node);

  CHECK_RETURN Err trace_sym_argument_list         (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_assignment_expression (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_binary_expression     (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_call_expression       (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_case_statement        (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_compound_statement    (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_condition_clause      (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_conditional_expression(MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_declaration           (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_field_expression      (MtMethodInstance* ctx, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_sym_for_statement         (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_function_definition   (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_if_statement          (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_init_declarator       (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_initializer_list      (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_return_statement      (MtMethodInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_switch_statement      (MtMethodInstance* inst, MnNode node);

  std::vector<MtInstance*> path;

  MtModLibrary* lib;
  MtModuleInstance* root_inst;
  bool verbose;
};
