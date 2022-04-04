
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

class MtTracer {
 public:
  bool trace_dispatch(MnNode n);
  bool trace_children(MnNode n);


  //_Check_return_
  bool trace_assign(MnNode n);

  bool trace_call(MnNode n);
  bool trace_field(MnNode n);
  bool trace_id(MnNode n);
  bool trace_if(MnNode n);
  bool trace_switch(MnNode n);
  bool trace_ternary(MnNode n);

  bool trace_submod_call(MnNode n);
  bool trace_method_call(MnNode n);
  bool trace_template_call(MnNode n);

  bool trace_read(MnNode const& n);
  bool trace_write(MnNode const& n);
  bool trace_end_fn();

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
