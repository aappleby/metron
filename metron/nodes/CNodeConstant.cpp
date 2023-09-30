#include "CNodeConstant.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

Err CNodeConstant::trace(CInstance* inst, call_stack& stack) { return Err(); }

//------------------------------------------------------------------------------

Err CNodeConstant::emit(Cursor& cursor) { return Emitter(cursor).emit(this); }

//==============================================================================
