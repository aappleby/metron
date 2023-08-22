#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeType : public CNode {
  virtual uint32_t debug_color() const;
  virtual Err emit(Cursor& cursor);
};

//------------------------------------------------------------------------------
