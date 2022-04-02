
#include <map>
#include <string>

#include "MtNode.h"

struct MtModule;
struct MtMethod;

//------------------------------------------------------------------------------

enum FieldState {
  CLEAN = 0,
  MAYBE = 1,
  DIRTY = 2,
  ERROR = 3,
};

inline const char* to_string(FieldState s) {
  switch (s) {
    case CLEAN:
      return "CLEAN";
    case MAYBE:
      return "MAYBE";
    case DIRTY:
      return "DIRTY";
    case ERROR:
      return "ERROR";
    default:
      return "INVALID";
  }
}

typedef std::map<std::string, FieldState> field_state_map;

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

class MtTracer {
 public:
  void trace_dispatch(MnNode n);
  void trace_children(MnNode n);

  void trace_assign(MnNode n);
  void trace_call(MnNode n);
  void trace_id(MnNode n);
  void trace_if(MnNode n);
  void trace_switch(MnNode n);


#if 0
  void update_delta();
  void check_dirty_read_identifier(MnNode n, MtDelta& d);
  void check_dirty_read_submod(MnNode n, MtDelta& d);
  void check_dirty_write(MnNode n, MtDelta& d);
  void check_dirty_dispatch(MnNode n, MtDelta& d);
  void check_dirty_assign(MnNode n, MtDelta& d);
  void check_dirty_if(MnNode n, MtDelta& d);
  void check_dirty_call(MnNode n, MtDelta& d);
  void check_dirty_switch(MnNode n, MtDelta& d);
#endif

  MtModule* mod() { return mod_stack.back(); }
  MtMethod* method() { return method_stack.back(); }

  std::vector<MtModule*> mod_stack;
  std::vector<MtMethod*> method_stack;

  int depth;
  MtDelta delta;
};

//------------------------------------------------------------------------------
