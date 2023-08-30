#pragma once

#include "metron/tools/MtUtils.h"
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"

struct CNode;
struct CNodeExpression;
struct CNodeCompound;
struct CSourceRepo;
struct CInstField;
struct CInstFunction;

#if 0

class Tracer {
public:
  Tracer(CSourceRepo* repo, bool verbose);

  CHECK_RETURN Err trace();


  CHECK_RETURN Err log_action(CInstFunction* func, CNode* node, CInstField* field_inst, TraceAction action);

  CHECK_RETURN Err trace_top_call(CInstFunction* func);

  CHECK_RETURN Err trace_identifier(CInstFunction* func, CNode* node, TraceAction action);
  CHECK_RETURN Err trace_declarator(CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_statement (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_expression(CInstFunction* func, CNodeExpression* node, TraceAction action);
  CHECK_RETURN Err trace_call      (CInstFunction* func, CInstFunction* method, CNode* call_node);
  CHECK_RETURN Err trace_default   (CInstFunction* func, CNode* node);

  CHECK_RETURN Err trace_argument_list         (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_assignment_expression (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_binary_expression     (CInstFunction* func, CNodeExpression* node);
  CHECK_RETURN Err trace_call_expression       (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_case_statement        (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_compound_statement    (CInstFunction* func, CNodeCompound* node);
  CHECK_RETURN Err trace_condition_clause      (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_conditional_expression(CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_declaration           (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_field_expression      (CInstFunction* func, CNode* node, TraceAction action);
  CHECK_RETURN Err trace_for_statement         (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_function_definition   (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_if_statement          (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_init_declarator       (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_initializer_list      (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_return_statement      (CInstFunction* func, CNode* node);
  CHECK_RETURN Err trace_switch_statement      (CInstFunction* func, CNode* node);

  //std::vector<MtInstance*> path;
  //std::vector<MtMethodInstance*> call_stack;

  //MtModLibrary* lib;
  //MtModuleInstance* root_inst;

  CSourceRepo* repo;

  bool verbose;
};

#endif
