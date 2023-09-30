#pragma once

#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeBuiltinType : public CNodeType {
  std::string_view get_name() const override;
  Err trace(CInstance* inst, call_stack& stack) override;
  Err emit(Cursor& cursor) override;
};

//==============================================================================
