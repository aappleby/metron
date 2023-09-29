#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeReturn : public CNodeStatement {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//==============================================================================
