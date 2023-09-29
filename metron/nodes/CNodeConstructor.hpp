#pragma once

#include "metron/nodes/CNodeFunction.hpp"

//------------------------------------------------------------------------------

struct CNodeConstructor : public CNodeFunction {
  void init(const char* match_tag, SpanType span, uint64_t flags);
};

//------------------------------------------------------------------------------
