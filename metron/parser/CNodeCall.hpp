#pragma once

#include "metrolib/core/Log.h"

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  virtual std::string_view get_name() const;
  virtual uint32_t debug_color() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(IContext* context, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  virtual std::string_view get_name() const { return "arg"; }
};

//------------------------------------------------------------------------------
