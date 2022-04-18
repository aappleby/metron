#pragma once
#include <map>
#include <string>
#include <assert.h>

#include "Err.h"
#include "MtNode.h"

struct MtModule;
struct MtMethod;
struct MtField;

//------------------------------------------------------------------------------

#if 0

NONE     + read  -> INPUT
INPUT    + read  -> INPUT
OUTPUT   + read  -> SIGNAL
SIGNAL   + read  -> SIGNAL
REGISTER + read  -> X


NONE     + tock write -> OUTPUT
INPUT    + tock write -> X
OUTPUT   + tock write -> OUTPUT
SIGNAL   + tock write -> X
REGISTER + tock write -> X

NONE     + tick write -> REGISTER
INPUT    + tick write -> REGISTER
OUTPUT   + tick write -> X
SIGNAL   + tick write -> X
REGISTER + tick write -> REGISTER

// wait no, INPUT || OUTPUT is bad in tock, ok in tick
// but it can't happen in tick, because the write would've made a REGISTER
// and not an OUTPUT

in tick
  NONE     || INPUT    = INPUT
  NONE     || REGISTER = REGISTER
  INPUT    || REGISTER = REGISTER

  NONE     || OUTPUT   = X
  NONE     || SIGNAL   = X
  INPUT    || OUTPUT   = X
  INPUT    || SIGNAL   = X
  OUTPUT   || SIGNAL   = X
  OUTPUT   || REGISTER = X
  SIGNAL   || REGISTER = X

in tock

  NONE     || INPUT    = INPUT
  OUTPUT   || SIGNAL   = SIGNAL

  NONE     || OUTPUT   = X
  NONE     || SIGNAL   = X
  INPUT    || OUTPUT   = X
  INPUT    || SIGNAL   = X

  NONE     || REGISTER = X
  INPUT    || REGISTER = X
  OUTPUT   || REGISTER = X
  SIGNAL   || REGISTER = X

#endif

//------------------------------------------------------------------------------

enum FieldState {
  FIELD_NONE     = 0,
  FIELD_INPUT    = 1,
  FIELD_OUTPUT   = 2,
  FIELD_SIGNAL   = 3,
  FIELD_REGISTER = 4,
  FIELD_INVALID  = 5,
};

inline const char* to_string(FieldState f) {
  switch(f) {
  case FIELD_NONE     : return "FIELD_NONE";
  case FIELD_INPUT    : return "FIELD_INPUT";
  case FIELD_OUTPUT   : return "FIELD_OUTPUT";
  case FIELD_SIGNAL   : return "FIELD_SIGNAL";
  case FIELD_REGISTER : return "FIELD_REGISTER";
  default             : return "FIELD_INVALID";
  }
}

typedef std::map<std::string, FieldState> StateMap;

//------------------------------------------------------------------------------

class MtTracer {
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
  StateMap* root_state;
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
