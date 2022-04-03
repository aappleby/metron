
#include <map>
#include <string>

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

typedef std::map<std::string, FieldState> state_map;

//------------------------------------------------------------------------------

#if 0
struct MtDelta {
  bool valid = false;
  bool error = false;
  field_state_map state_old;
  field_state_map state_new;

  void add_prefix(const std::string& p) {
    field_state_map temp_old;
    field_state_map temp_new;

    temp_old.swap(state_old);
    temp_new.swap(state_new);

    for (auto& n : temp_old) state_old[p + "." + n.first] = n.second;
    for (auto& n : temp_new) state_new[p + "." + n.first] = n.second;
  }

  void wipe() {
    valid = false;
    error = false;
    state_old.clear();
    state_new.clear();
  }

  /*
  void sanity_check() const {
  for (auto& s1 : state_old) {
  auto& s2 = *state_new.find(s1.first);
  assert(s1.second != ERROR);
  assert(s2.second != ERROR);
  if (s1.second == MAYBE) assert(s2.second != CLEAN);
  if (s1.second == DIRTY) assert(s2.second == DIRTY);
  }
  }
  */
};
#endif

//------------------------------------------------------------------------------

class MtTracer {
 public:
  void trace_dispatch(MnNode n);
  void trace_children(MnNode n);

  void trace_assign(MnNode n);
  void trace_call(MnNode n);
  void trace_field(MnNode n);
  void trace_id(MnNode n);
  void trace_if(MnNode n);
  void trace_switch(MnNode n);
  void trace_ternary(MnNode n);

  void trace_submod_call(MnNode n);
  void trace_method_call(MnNode n);
  void trace_template_call(MnNode n);

  void trace_read(MnNode const& n);
  void trace_write(MnNode const& n);
  void trace_end_fn();

  void dump_trace();

  MtModule* mod() { return _mod_stack.back(); }
  MtMethod* method() { return _method_stack.back(); }
  state_map& state_top() { return *_state_stack.back(); }

  int  depth()   const { return (int)_method_stack.size(); }
  bool in_tick() const;
  bool in_tock() const;

  std::vector<MtField*>   _field_stack;
  std::vector<MtModule*>  _mod_stack;
  std::vector<MtMethod*>  _method_stack;
  std::vector<state_map*> _state_stack;
};

//------------------------------------------------------------------------------
