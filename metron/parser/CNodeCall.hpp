#pragma once

#include "metrolib/core/Log.h"

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  std::string_view get_name() const override;
  uint32_t debug_color() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(CCall* call) override;
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  std::string_view get_name() const override { return "arg"; }
};

//------------------------------------------------------------------------------
