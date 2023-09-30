#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

//==============================================================================

struct CNodeType : public CNode {
  uint32_t debug_color() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

protected:
  CNodeType() {}
};

//==============================================================================
