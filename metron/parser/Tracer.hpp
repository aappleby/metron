#pragma once

#include "metron/tools/MtUtils.h"
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"

struct CNode;
struct CNodeExpression;
struct CNodeCompound;
struct CSourceRepo;
struct CInstField;
struct CInstCall;

#if 0

class Tracer {
public:
  Tracer(CSourceRepo* repo, bool verbose);

  CHECK_RETURN Err trace();


  CHECK_RETURN Err log_action(CInstCall* call, CNode* node, CInstField* field_inst, TraceAction action);

  CHECK_RETURN Err trace_top_call(CInstCall* call);

  CHECK_RETURN Err trace_identifier(CInstCall* call, CNode* node, TraceAction action);
  CHECK_RETURN Err trace_declarator(CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_statement (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_expression(CInstCall* call, CNodeExpression* node, TraceAction action);
  CHECK_RETURN Err trace_call      (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_default   (CInstCall* call, CNode* node);

  CHECK_RETURN Err trace_argument_list         (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_assignment_expression (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_binary_expression     (CInstCall* call, CNodeExpression* node);
  CHECK_RETURN Err trace_call_expression       (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_case_statement        (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_compound_statement    (CInstCall* call, CNodeCompound* node);
  CHECK_RETURN Err trace_condition_clause      (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_conditional_expression(CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_declaration           (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_field_expression      (CInstCall* call, CNode* node, TraceAction action);
  CHECK_RETURN Err trace_for_statement         (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_function_definition   (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_if_statement          (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_init_declarator       (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_initializer_list      (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_return_statement      (CInstCall* call, CNode* node);
  CHECK_RETURN Err trace_switch_statement      (CInstCall* call, CNode* node);

  //std::vector<MtInstance*> path;
  //std::vector<MtMethodInstance*> call_stack;

  //MtModLibrary* lib;
  //MtModuleInstance* root_inst;

  CSourceRepo* repo;

  bool verbose;
};

#endif
