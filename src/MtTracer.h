#pragma once
#include <assert.h>

#include <string>

#include "Err.h"
#include "MtNode.h"
#include "MtUtils.h"
#include "Platform.h"

struct MtModLibrary;
struct MtContext;
struct MtField;

//------------------------------------------------------------------------------

struct TraceLog {
  MtContext* method_ctx;
  MtContext* dst_ctx;
  ContextAction action;
  SourceRange range;
};

//------------------------------------------------------------------------------

class MtTracer {
 public:
  MtTracer(MtModLibrary* lib) : lib(lib) {}

  CHECK_RETURN Err trace_dispatch(MtContext* inst, MnNode n,
                                  ContextAction action = CTX_READ);

  CHECK_RETURN Err trace_call(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_branch(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_switch(MtContext* inst, MnNode n);

  CHECK_RETURN Err trace_method_ctx(MtContext* method_ctx, MnNode node_call);
  CHECK_RETURN Err log_action(MtContext* method_ctx, MtContext* dst_ctx,
                              ContextAction action, SourceRange source);
  CHECK_RETURN Err merge_branch(MtContext* ma, MtContext* mb, MtContext* out);

  void dump_log(MtField* filter_field = nullptr);

  MtModLibrary* lib;

  std::vector<TraceLog> trace_log;
};

//------------------------------------------------------------------------------
