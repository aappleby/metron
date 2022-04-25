#pragma once
#include <assert.h>

#include <string>

#include "Err.h"
#include "MtNode.h"
#include "MtUtils.h"
#include "Platform.h"

struct MtModLibrary;
struct MtContext;

//------------------------------------------------------------------------------

struct TraceLog {
  std::string field_name;
  bool is_write;
  SourceRange source;
};

//------------------------------------------------------------------------------

class MtTracer {
 public:
  MtTracer(MtModLibrary* lib) : lib(lib) {}

  CHECK_RETURN Err trace_dispatch(MtContext* inst, MnNode n,
                                  ContextAction action = CTX_READ);

  CHECK_RETURN Err trace_method(MtContext ctx, MnNode n);
  CHECK_RETURN Err trace_call(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_branch(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_switch(MtContext* inst, MnNode n);

  CHECK_RETURN Err log_action(MtContext* inst, ContextAction action,
                              SourceRange source);
  CHECK_RETURN Err merge_branch(MtContext* ma, MtContext* mb, MtContext* out);

  MtModLibrary* lib;
};

//------------------------------------------------------------------------------
