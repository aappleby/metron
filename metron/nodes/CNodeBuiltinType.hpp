#pragma once

#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeBuiltinType : public CNodeType {
  std::string_view get_name() const override {
    return get_text();
  }
};

//==============================================================================
