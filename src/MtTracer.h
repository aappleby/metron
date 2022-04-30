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
  MtTracer(MtModLibrary* lib, MtContext* root) : lib(lib), ctx_root(root) {}

  CHECK_RETURN Err trace_dispatch(MtContext* inst, MnNode n,
                                  ContextAction action = CTX_READ);

  CHECK_RETURN Err trace_call(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_branch(MtContext* inst, MnNode n);
  CHECK_RETURN Err trace_switch(MtContext* inst, MnNode n);

  CHECK_RETURN Err trace_method_ctx(MtContext* method_ctx, MnNode node_call);
  CHECK_RETURN Err log_action(MtContext* method_ctx, MtContext* dst_ctx,
                              ContextAction action, SourceRange source);

  MtModLibrary* lib;
  MtContext* ctx_root;
};

//------------------------------------------------------------------------------
