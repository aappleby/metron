#pragma once

#include "metrolib/core/Log.h"

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  virtual std::string_view get_name() const { return child("func_name")->get_text(); }
  virtual uint32_t debug_color() const { return COL_SKY; }
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  virtual std::string_view get_name() const { return "arg"; }
};

//------------------------------------------------------------------------------
