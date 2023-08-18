#pragma once

#include "CContext.hpp"
#include "CNode.hpp"

//------------------------------------------------------------------------------

struct CNodeStruct : public CNode {
  virtual uint32_t debug_color() const { return 0xFFAAAA; }

  Err collect_fields_and_methods();
};

//------------------------------------------------------------------------------
