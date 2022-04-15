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

enum FieldDelta {
  DELTA_RD = 0,
  DELTA_WR,
  DELTA_WS,
  DELTA_EF,
  DELTA_MAX,
};

enum FieldState {
  FIELD________ = 0,
  FIELD____WR__,
  FIELD____WR_L,
  FIELD_RD_____,
  FIELD_RD_WR__,
  FIELD_RD_WR_L,
  FIELD____WS__,
  FIELD____WS_L,
  FIELD_INVALID,
  FIELD_MAX,
};

enum FieldFlags {
  FIELD_READ  = 1,
  FIELD_WRITE = 2,
  FIELD_LOCK  = 4,
  FIELD_ERR   = 8,
};

FieldState merge_delta(FieldState a, FieldDelta b);
FieldState merge_parallel(FieldState a, FieldState b);
FieldState merge_series(FieldState a, FieldState b);

// KCOV_OFF
inline const char* to_string(FieldState s) {
  switch (s) {
  case FIELD________:  return "FIELD________";
  case FIELD____WR__:  return "FIELD____WR__";
  case FIELD____WR_L:  return "FIELD____WR_L";
  case FIELD_RD_____:  return "FIELD_RD_____";
  case FIELD_RD_WR__:  return "FIELD_RD_WR__";
  case FIELD_RD_WR_L:  return "FIELD_RD_WR_L";
  case FIELD____WS__:  return "FIELD____WS__";
  case FIELD____WS_L:  return "FIELD____WS_L";
  case FIELD_INVALID:  return "FIELD_INVALID";
  case FIELD_MAX:      return "FIELD_MAX";
  default:             return "?????";
  }
}
// KCOV_ON

struct MtStateMap {

  void clear() {
    state.clear();
  }

  void set_state(const std::string& key, FieldState val) {
    auto find1 = key.find("<top>");
    auto find2 = key.find("<top>", 5);
    assert(find1 == 0);
    assert(find2 == std::string::npos);

    state[key] = val;
  }

  FieldState get_state(const std::string& key) {
    auto find1 = key.find("<top>");
    auto find2 = key.find("<top>", 5);
    assert(find1 == 0);
    assert(find2 == std::string::npos);

    return state.contains(key) ? state[key] : FIELD________;
  }

  void push_action(const std::string& key, char action) {
    auto find1 = key.find("<top>");
    auto find2 = key.find("<top>", 5);
    assert(find1 == 0);
    assert(find2 == std::string::npos);

    auto& a = actions[key];

    if (a.empty() || (a.back() != action)) a.push_back(action);
  }

  void set_actions(const std::string& key, const std::string& new_actions) {
    auto find1 = key.find("<top>");
    auto find2 = key.find("<top>", 5);
    assert(find1 == 0);
    assert(find2 == std::string::npos);

    actions[key] = new_actions;
  }

  const std::string& get_actions(const std::string& key) {
    auto find1 = key.find("<top>");
    auto find2 = key.find("<top>", 5);
    assert(find1 == 0);
    assert(find2 == std::string::npos);

    return actions[key];
  }

  std::map<std::string, FieldState>& get_map() {
    return state;
  }

  const std::map<std::string, FieldState>& get_map() const {
    return state;
  }

  void swap(MtStateMap& b) {
    actions.swap(b.actions);
    state.swap(b.state);
  }

  Err lock_state() {
    Err err;

    for (auto& pair : actions) {
      pair.second.push_back('L');
    }
  
  
    for (auto& pair : state) {
      auto old_state = pair.second;
      auto new_state = merge_delta(old_state, DELTA_EF);

      if (new_state == FIELD_INVALID) {
        err << ERR("Field %s was %s, now %s!", pair.first.c_str(), to_string(old_state), to_string(new_state));
      }

      pair.second = new_state;
    }

    return err;
  }

private:

  std::map<std::string, std::string> actions;
  std::map<std::string, FieldState> state;
};


//------------------------------------------------------------------------------

class MtTracer {
 public:

  // Top level
  CHECK_RETURN Err trace_method(MtMethod* method);

  CHECK_RETURN Err trace_dispatch(MnNode n);
  CHECK_RETURN Err trace_children(MnNode n);

  CHECK_RETURN Err trace_assign(MnNode n);

  CHECK_RETURN Err trace_call(MnNode n);
  CHECK_RETURN Err trace_field(MnNode n);
  CHECK_RETURN Err trace_id(MnNode n);
  CHECK_RETURN Err trace_if(MnNode n);
  CHECK_RETURN Err trace_switch(MnNode n);
  CHECK_RETURN Err trace_ternary(MnNode n);

  CHECK_RETURN Err trace_submod_call(MnNode n);
  CHECK_RETURN Err trace_method_call(MnNode n);
  CHECK_RETURN Err trace_template_call(MnNode n);

  CHECK_RETURN Err trace_read(const std::string& field_name);
  CHECK_RETURN Err trace_write(const std::string& field_name);

  CHECK_RETURN Err trace_read(MnNode const& n);
  CHECK_RETURN Err trace_write(MnNode const& n);

  bool ends_with_break(MnNode n);

  static void dump_trace(MtStateMap& m);
  void dump_stack();

  MtModule* mod() { return _mod_stack.back(); }
  MtMethod* method() { return _method_stack.back(); }

  bool in_tick() const;
  bool in_tock() const;

  MtMethod* root;
  MtStateMap* root_state;
  MtStateMap* state_top;

  std::vector<std::string> _path_stack;
  std::vector<MtModule*>   _mod_stack;
  std::vector<MtMethod*>   _method_stack;

  void push_state(MtStateMap* state) {
    __state_stack.push_back(state);
    state_top = __state_stack.back();
  }

  void pop_state() {
    __state_stack.pop_back();
    state_top = __state_stack.back();
  }

  std::vector<MtStateMap*> __state_stack;
};

//------------------------------------------------------------------------------
