#pragma once

#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"
#include "CInstance.hpp"

struct CSourceRepo;

class Tracer {
public:
  Tracer(CSourceRepo* repo, CInstance* root_inst, bool verbose);

  CHECK_RETURN Err log_action(CInstCall* call, CNode* node, CInstField* field_inst, TraceAction action);

  CHECK_RETURN Err trace_top_call(CInstCall* call);

  //CHECK_RETURN Err trace_identifier(CCallInstance* call, CNode* node, TraceAction action);
  //CHECK_RETURN Err trace_declarator(CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_statement (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_expression(CCallInstance* call, CNode* node, TraceAction action);
  //CHECK_RETURN Err trace_call      (CCallInstance* call, MtMethodInstance* method, CNode* call_node);
  //CHECK_RETURN Err trace_default   (CCallInstance* call, CNode* node);


  //CHECK_RETURN Err trace_sym_argument_list         (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_assignment_expression (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_binary_expression     (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_call_expression       (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_case_statement        (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_compound_statement    (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_condition_clause      (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_conditional_expression(CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_declaration           (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_field_expression      (CCallInstance* call, CNode* node, TraceAction action);
  //CHECK_RETURN Err trace_sym_for_statement         (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_function_definition   (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_if_statement          (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_init_declarator       (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_initializer_list      (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_return_statement      (CCallInstance* call, CNode* node);
  //CHECK_RETURN Err trace_sym_switch_statement      (CCallInstance* call, CNode* node);

  //std::vector<MtInstance*> path;
  //std::vector<MtMethodInstance*> call_stack;

  //MtModLibrary* lib;
  //MtModuleInstance* root_inst;

  CSourceRepo* repo;
  CInstance* root_inst;

  bool verbose;
};
