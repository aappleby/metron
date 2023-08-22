#include "Tracer.hpp"


Tracer::Tracer(CSourceRepo* repo, bool verbose)
: repo(repo), verbose(verbose) {
}


CHECK_RETURN Err Tracer::log_action(CInstCall* call, CNode* node, CInstField* field_inst, TraceAction action) {
  Err err;
  return err;
}

CHECK_RETURN Err Tracer::trace() {
  Err err;
  return err;
}

CHECK_RETURN Err Tracer::trace_top_call(CInstCall* call) {
  Err err;
  return err;
}
