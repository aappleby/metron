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
  CHECK_RETURN Err trace_call(MnNode n);
  CHECK_RETURN Err trace_branch(MnNode n);
  CHECK_RETURN Err trace_switch(MnNode n);


  CHECK_RETURN Err trace(MnNode node, bool is_write);
  CHECK_RETURN Err trace(const std::string& field_name, bool is_write);

  CHECK_RETURN Err merge_branch(StateMap & ma, StateMap & mb, StateMap & out);

  MtModule* mod_top();
  StateMap* state_top()  { return _state_stack.back(); }

  void push_state(StateMap* state) {
    _state_stack.push_back(state);
  }

  void pop_state() {
    _state_stack.pop_back();
  }

  //----------------------------------------

  MtModLibrary* lib;

  std::vector<MtField*>  _component_stack;
  std::vector<MtMethod*> _method_stack;
  std::vector<StateMap*> _state_stack;
};

//------------------------------------------------------------------------------
