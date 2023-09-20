#pragma once

#include <string>
#include <vector>
#include "metrolib/core/Platform.h"

//------------------------------------------------------------------------------

static std::vector<std::string> split_path(const std::string& input) {
  std::vector<std::string> result;
  std::string temp;

  const char* c = input.c_str();

  do {
    if (*c == '/' || *c == '\\' || *c == 0) {
      if (temp.size()) result.push_back(temp);
      temp.clear();
    } else {
      temp.push_back(*c);
    }
  } while (*c++ != 0);

  return result;
}

//------------------------------------------------------------------------------

static std::string join_path(std::vector<std::string>& path) {
  std::string result;
  for (auto& s : path) {
    if (result.size()) result += "/";
    result += s;
  }
  return result;
}

//------------------------------------------------------------------------------

static void mkdir_all(const std::vector<std::string>& full_path) {
  std::string temp;
  for (size_t i = 0; i < full_path.size(); i++) {
    if (temp.size()) temp += "/";
    temp += full_path[i];
    //printf("making dir %s\n", temp.c_str());
    int result = plat_mkdir(temp.c_str());
    //printf("mkdir result %d\n", result);
  }
}

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
  FT_INPUT,
  FT_OUTPUT,
  FT_REGISTER,
  FT_SIGNAL,
  FT_MODULE,
  FT_INVALID
};

enum MethodType {
  MT_UNKNOWN = 0,
  MT_INIT,
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
  TS_NONE = 0,
  TS_INPUT,
  TS_OUTPUT,
  TS_MAYBE,
  TS_SIGNAL,
  TS_REGISTER,
  TS_INVALID,
  TS_PENDING,  // hasn't been set yet
};

enum TraceAction {
  ACT_READ = 0,
  ACT_WRITE = 1,
  ACT_PUSH = 2,
  ACT_POP = 3,
  ACT_SWAP = 4,
  ACT_MERGE = 5,
  ACT_INVALID = -1
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
    case MT_INIT:    return "MT_INIT";
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
    case ACT_READ:    return "READ";
    case ACT_WRITE:   return "WRITE";
    case ACT_PUSH:    return "PUSH";
    case ACT_POP:     return "POP";
    case ACT_SWAP:    return "SWAP";
    case ACT_MERGE:   return "MERGE";
    case ACT_INVALID: return "INVALID";
    default:          return "???";
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
    case TS_NONE:
      return "TS_NONE";
    case TS_INPUT:
      return "TS_INPUT";
    case TS_OUTPUT:
      return "TS_OUTPUT";
    case TS_MAYBE:
      return "TS_MAYBE";
    case TS_SIGNAL:
      return "TS_SIGNAL";
    case TS_REGISTER:
      return "TS_REGISTER";
    case TS_INVALID:
      return "TS_INVALID";
    case TS_PENDING:
      return "TS_PENDING";
    default:
      return "TS_?????";
  }
}
// KCOV_ON

TraceState merge_action(TraceState state, TraceAction action);
const char* merge_action_message(TraceState state, TraceAction action);
TraceState merge_branch(TraceState ma, TraceState mb);

std::string str_printf(const char* fmt, ...);

//------------------------------------------------------------------------------
