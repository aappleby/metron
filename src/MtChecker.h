#pragma once
#include <assert.h>

#include <map>
#include <string>

#include "Err.h"
#include "MtNode.h"
#include "MtTracer.h"

struct MtModule;
struct MtMethod;
struct MtField;

//------------------------------------------------------------------------------

class MtChecker {
 public:
#if 0
  // Top level
  CHECK_RETURN Err trace_method(MtMethod* method);

  CHECK_RETURN Err trace_dispatch(MnNode n);
  CHECK_RETURN Err trace_children(MnNode n);

  CHECK_RETURN Err trace_assign(MnNode n);

  CHECK_RETURN Err trace_call(MnNode n);
  CHECK_RETURN Err trace_branch(MnNode n);
  CHECK_RETURN Err trace_switch(MnNode n);

  CHECK_RETURN Err trace_component_call(const std::string& component_name, MtModule* dst_module, MtMethod* dst_method);
  CHECK_RETURN Err trace_method_call(MtMethod* method);
  //CHECK_RETURN Err trace_template_call(MnNode n);

  CHECK_RETURN Err trace_read(MtField* field, MtField* component_field = nullptr);
  CHECK_RETURN Err trace_write(MtField* field, MtField* component_field = nullptr);

  CHECK_RETURN Err merge_branch(StateMap & ma, StateMap & mb, StateMap & out);
#endif

  static bool ends_with_break(MnNode n);
  static bool has_return(MnNode n);
  static bool has_non_terminal_return(MnNode n);
};

//------------------------------------------------------------------------------
