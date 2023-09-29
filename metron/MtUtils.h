#pragma once

#include <string>
#include <vector>
#include "metrolib/core/Platform.h"

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

std::vector<std::string> split_path(const std::string& input);
std::string join_path(std::vector<std::string>& path);
void mkdir_all(const std::vector<std::string>& full_path);

FieldType trace_state_to_field_type(TraceState s);

const char* to_string(FieldType f);
const char* to_string(MethodType f);
const char* to_string(TraceAction f);
const char* to_string(ContextType c);
const char* to_string(TraceState f);

TraceState merge_action(TraceState state, TraceAction action);
const char* merge_action_message(TraceState state, TraceAction action);
TraceState merge_branch(TraceState ma, TraceState mb);

std::string str_printf(const char* fmt, ...);

//------------------------------------------------------------------------------
