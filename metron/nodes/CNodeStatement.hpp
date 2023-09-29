#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeStatement : public CNode {
  uint32_t debug_color() const override;
protected:
  CNodeStatement() {}
};

//------------------------------------------------------------------------------

struct CNodeDoWhile : public CNodeStatement {
};

//------------------------------------------------------------------------------

struct CNodeWhile : public CNodeStatement {
};

//------------------------------------------------------------------------------
