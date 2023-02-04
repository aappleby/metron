#pragma once

#include <string>

//------------------------------------------------------------------------------

struct SourceRange {
  const char* filename;
  const char* path;
  int row;
  int col;
  const char* start;
  const char* end;

  SourceRange trim() {
    SourceRange r = *this;
    while(r.start < r.end && isspace(*r.start)) r.start++;
    while(r.end > r.start && isspace(r.end[-1])) r.end--;
    return r;
  }
};

//------------------------------------------------------------------------------

enum FieldType {
  FT_UNKNOWN = 0,
  FT_REGISTER,
  FT_SIGNAL,
  FT_INPUT,
  FT_OUTPUT,
  FT_MODULE,
  FT_INVALID
};

enum MethodType {
  MT_UNKNOWN = 0,
  MT_TICK,
  MT_TOCK,
  MT_FUNC,
  MT_TASK,
  MT_INVALID
};

enum ContextType {
  CTX_MODULE,
  CTX_COMPONENT,
  CTX_FIELD,
  CTX_METHOD,
  CTX_PARAM,
  CTX_RETURN,
};

enum TraceState {
  CTX_NONE = 0,
  CTX_INPUT,
  CTX_OUTPUT,
  CTX_MAYBE,
  CTX_SIGNAL,
  CTX_REGISTER,
  CTX_INVALID,
  CTX_PENDING,  // hasn't been set yet
};

enum TraceAction {
  //CTX_READ = 0,
  //CTX_WRITE = 1,
  ACT_READ = 0,
  ACT_WRITE = 1,
  ACT_PUSH = 2,
  ACT_POP = 3,
  ACT_SWAP = 4,
  ACT_MERGE = 5
};

FieldType trace_state_to_field_type(TraceState s);

// KCOV_OFF
inline const char* to_string(FieldType f) {
  switch (f) {
    case FT_UNKNOWN:   return "FT_UNKNOWN";
    case FT_REGISTER:  return "FT_REGISTER";
    case FT_SIGNAL:    return "FT_SIGNAL";
    case FT_INPUT:     return "FT_INPUT";
    case FT_OUTPUT:    return "FT_OUTPUT";
    case FT_MODULE: return "FT_MODULE";
    case FT_INVALID:   return "FT_INVALID";
    default:           return "???";
  }
}

inline const char* to_string(MethodType f) {
  switch (f) {
    case MT_UNKNOWN: return "MT_UNKNOWN";
    case MT_TICK:    return "MT_TICK";
    case MT_TOCK:    return "MT_TOCK";
    case MT_FUNC:    return "MT_FUNC";
    case MT_TASK:    return "MT_TASK";
    case MT_INVALID: return "MT_INVALID";
    default:         return "???";
  }
}

inline const char* to_string(TraceAction f) {
  switch (f) {
    case ACT_READ:  return "READ";
    case ACT_WRITE: return "WRITE";
    case ACT_PUSH:  return "PUSH";
    case ACT_POP:   return "POP";
    case ACT_SWAP:  return "SWAP";
    case ACT_MERGE: return "MERGE";
    default:        return "???";
  }
}

inline const char* to_string(ContextType c) {
  switch (c) {
    case CTX_MODULE:
      return "CTX_MODULE";
    case CTX_COMPONENT:
      return "CTX_COMPONENT";
    case CTX_FIELD:
      return "CTX_FIELD";
    case CTX_METHOD:
      return "CTX_METHOD";
    case CTX_PARAM:
      return "CTX_PARAM";
    case CTX_RETURN:
      return "CTX_RETURN";
    default:
      return "???";
  }
}

inline const char* to_string(TraceState f) {
  switch (f) {
    case CTX_NONE:
      return "CTX_NONE";
    case CTX_INPUT:
      return "CTX_INPUT";
    case CTX_OUTPUT:
      return "CTX_OUTPUT";
    case CTX_MAYBE:
      return "CTX_MAYBE";
    case CTX_SIGNAL:
      return "CTX_SIGNAL";
    case CTX_REGISTER:
      return "CTX_REGISTER";
    case CTX_INVALID:
      return "CTX_INVALID";
    case CTX_PENDING:
      return "CTX_PENDING";
    default:
      return "CTX_?????";
  }
}
// KCOV_ON

TraceState merge_action(TraceState state, TraceAction action);
const char* merge_action_message(TraceState state, TraceAction action);
TraceState merge_branch(TraceState ma, TraceState mb);

std::string str_printf(const char* fmt, ...);

//------------------------------------------------------------------------------
