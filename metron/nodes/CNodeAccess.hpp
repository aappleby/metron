#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeAccess : public CNode {
  uint32_t debug_color() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  void dump() const override;
};

//==============================================================================
