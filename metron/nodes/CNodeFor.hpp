#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeFor : public CNodeStatement {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//==============================================================================
