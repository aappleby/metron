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

  CHECK_RETURN Err log_action(MtCallInstance* call, MnNode node, MtInstance* field_inst, TraceAction action);

  CHECK_RETURN Err trace_call(MtCallInstance* call);

  CHECK_RETURN Err trace_identifier(MtCallInstance* inst, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_declarator(MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_statement (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_expression(MtCallInstance* inst, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_call      (MtCallInstance* src_inst, MtMethodInstance* dst_inst, MnNode node_call);
  CHECK_RETURN Err trace_default   (MtCallInstance* inst, MnNode node);


  CHECK_RETURN Err trace_sym_argument_list         (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_assignment_expression (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_binary_expression     (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_call_expression       (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_case_statement        (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_compound_statement    (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_condition_clause      (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_conditional_expression(MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_declaration           (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_field_expression      (MtCallInstance* inst, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_sym_for_statement         (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_function_definition   (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_if_statement          (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_init_declarator       (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_initializer_list      (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_return_statement      (MtCallInstance* inst, MnNode node);
  CHECK_RETURN Err trace_sym_switch_statement      (MtCallInstance* inst, MnNode node);

  std::vector<MtInstance*> path;

  std::vector<MtMethodInstance*> call_stack;

  MtModLibrary* lib;
  MtModuleInstance* root_inst;
  bool verbose;
};
