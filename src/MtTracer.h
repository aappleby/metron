#pragma once
#include <map>
#include <string>
#include <assert.h>

#include "Err.h"
#include "MtNode.h"

struct MtModule;
struct MtMethod;
struct MtField;
struct MtModLibrary;

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

  MtTracer(MtModLibrary* lib) : lib(lib) {}

  CHECK_RETURN Err trace_dispatch(MnNode n);
  CHECK_RETURN Err trace_assign(MnNode n);
  CHECK_RETURN Err trace_call(MnNode n);
  CHECK_RETURN Err trace_branch(MnNode n);
  CHECK_RETURN Err trace_switch(MnNode n);

  CHECK_RETURN Err trace_read(MtField* field, MtField* component_field = nullptr);
  CHECK_RETURN Err trace_write(MtField* field, MtField* component_field = nullptr);

  CHECK_RETURN Err merge_branch(StateMap & ma, StateMap & mb, StateMap & out);

  MtModule* mod() { return _mod_stack.back(); }
  MtMethod* method() { return _method_stack.back(); }

  MtModLibrary* lib;

  StateMap* state_top;
  std::vector<std::string> _path_stack;
  std::vector<MtField*>    _field_stack;
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
