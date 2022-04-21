#pragma once
#include <map>
#include <string>
#include <assert.h>

#include "Err.h"
#include "MtNode.h"
#include "MtTracer.h"

struct MtModule;
struct MtMethod;
struct MtField;

//------------------------------------------------------------------------------

class MtChecker {
 public:

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

  bool ends_with_break(MnNode n);

  bool has_return(MnNode n);
  bool has_non_terminal_return(MnNode n);

  static void dump_trace(StateMap& m);
  void dump_stack();

  MtModule* mod() { return _mod_stack.back(); }
  MtMethod* method() { return _method_stack.back(); }

  bool in_tick() const;
  bool in_tock() const;

  MtMethod* root;
  StateMap* state_top;

  std::vector<std::string> _path_stack;
  std::vector<MtModule*>   _mod_stack;
  std::vector<MtMethod*>   _method_stack;

  void push_state(StateMap* state) {
    __state_stack.push_back(state);
    state_top = __state_stack.back();
  }

  void pop_state() {
    __state_stack.pop_back();
    state_top = __state_stack.back();
  }

  std::vector<StateMap*> __state_stack;
};

//------------------------------------------------------------------------------
