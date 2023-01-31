#pragma once

#include <string>

//------------------------------------------------------------------------------

struct SourceRange {
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
  CTX_NIL,      // not an actual state, just a placeholder
};

struct LogEntry {
  TraceState state;
};

enum TraceAction {
  CTX_READ = 0,
  CTX_WRITE = 1,
};

// KCOV_OFF
inline const char* to_string(TraceAction f) {
  switch (f) {
    case CTX_READ:
      return "CTX_READ";
    case CTX_WRITE:
      return "CTX_WRITE";
    default:
      return "???";
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
    case CTX_NIL:
      return "CTX_NIL";
    default:
      return "CTX_INVALID";
  }
}
// KCOV_ON

TraceState merge_action(TraceState state, TraceAction action);
TraceState merge_branch(TraceState ma, TraceState mb);

std::string str_printf(const char* fmt, ...);

//------------------------------------------------------------------------------
