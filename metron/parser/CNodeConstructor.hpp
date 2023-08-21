#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeConstructor : public CNode {
  virtual uint32_t debug_color() const { return 0x0000FF; }
};

//------------------------------------------------------------------------------
