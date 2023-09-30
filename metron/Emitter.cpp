#include "Emitter.hpp"

#include "metron/nodes/CNodeAccess.hpp"

//==============================================================================

Err Emitter::emit(CNodeAccess* node) {
  Err err = cursor.check_at(node);
  err << cursor.comment_out(node);
  return err << cursor.check_done(node);
}

//==============================================================================
