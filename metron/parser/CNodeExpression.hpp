#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

struct CNodeUnit : public CNode {
  virtual uint32_t debug_color() const;
  virtual Err emit(Cursor& cursor);
  virtual Err trace(CInstance* instance);
};

//------------------------------------------------------------------------------

struct CNodeExpression : public CNode {
  virtual uint32_t debug_color() const;
  virtual Err emit(Cursor& cursor);
  virtual Err trace(CInstance* instance);

  bool is_integer_constant();
};

//------------------------------------------------------------------------------
