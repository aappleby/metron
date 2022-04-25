#pragma once

//------------------------------------------------------------------------------

struct SourceRange {
  const char* start;
  const char* end;
};

//------------------------------------------------------------------------------

enum ContextType {
  CTX_FIELD,
  CTX_METHOD,
  CTX_PARAM,
  CTX_RETURN,
  CTX_MODULE,
};

enum ContextState {
  CTX_NONE = 0,
  CTX_INPUT = 1,
  CTX_OUTPUT = 2,
  CTX_SIGNAL = 3,
  CTX_REGISTER = 4,
  CTX_INVALID = 5,
  CTX_PENDING = 6,
};

enum ContextAction {
  CTX_READ = 0,
  CTX_WRITE = 1,
};

inline const char* to_string(ContextAction f) {
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
    case CTX_FIELD:
      return "CTX_FIELD";
    case CTX_METHOD:
      return "CTX_METHOD";
    case CTX_PARAM:
      return "CTX_PARAM";
    case CTX_RETURN:
      return "CTX_RETURN";
    case CTX_MODULE:
      return "CTX_MODULE";
    default:
      return "???";
  }
}

inline const char* to_string(ContextState f) {
  switch (f) {
    case CTX_NONE:
      return "CTX_NONE";
    case CTX_INPUT:
      return "CTX_INPUT";
    case CTX_OUTPUT:
      return "CTX_OUTPUT";
    case CTX_SIGNAL:
      return "CTX_SIGNAL";
    case CTX_REGISTER:
      return "CTX_REGISTER";
    default:
      return "CTX_INVALID";
  }
}

ContextState merge_action(ContextState state, ContextAction action);
ContextState merge_branch(ContextState ma, ContextState mb);

//------------------------------------------------------------------------------
