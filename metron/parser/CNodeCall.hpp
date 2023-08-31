#pragma once

#include "metrolib/core/Log.h"

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  virtual std::string_view get_name() const override;
  virtual uint32_t debug_color() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  virtual std::string_view get_name() const override { return "arg"; }
};

//------------------------------------------------------------------------------
