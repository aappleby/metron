#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeUsing : public CNodeStatement {
  CNodeUsing() {
    color = 0x00DFFF;
  }

  std::string_view get_name() const override {
    return child("name")->get_text();
  }
};

//==============================================================================
