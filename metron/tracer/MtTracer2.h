#pragma once
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"
#include "metron/tracer/MtInstance.h"
#include "metron/tools/MtUtils.h"

struct MtModLibrary;
struct MtField;
struct MtMethod;
struct MnNode;

class MtTracer2 {
public:
  MtTracer2(MtModLibrary* lib, MtModuleInstance* root_inst, bool verbose);

  CHECK_RETURN Err log_action(MtCallInstance* call, MnNode node, MtInstance* field_inst, TraceAction action);

  CHECK_RETURN Err trace_top_call(MtCallInstance* call);

  CHECK_RETURN Err trace_identifier(MtCallInstance* call, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_declarator(MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_statement (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_expression(MtCallInstance* call, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_call      (MtCallInstance* call, MtMethodInstance* method, MnNode call_node);
  CHECK_RETURN Err trace_default   (MtCallInstance* call, MnNode node);


  CHECK_RETURN Err trace_sym_argument_list         (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_assignment_expression (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_binary_expression     (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_call_expression       (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_case_statement        (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_compound_statement    (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_condition_clause      (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_conditional_expression(MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_declaration           (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_field_expression      (MtCallInstance* call, MnNode node, TraceAction action);
  CHECK_RETURN Err trace_sym_for_statement         (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_function_definition   (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_if_statement          (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_init_declarator       (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_initializer_list      (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_return_statement      (MtCallInstance* call, MnNode node);
  CHECK_RETURN Err trace_sym_switch_statement      (MtCallInstance* call, MnNode node);

  std::vector<MtInstance*> path;

  std::vector<MtMethodInstance*> call_stack;

  MtModLibrary* lib;
  MtModuleInstance* root_inst;
  bool verbose;
};
