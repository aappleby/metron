#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeTypedef : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//==============================================================================

struct CNodeTypedefType : public CNodeType {
  std::string_view get_name() const override {
    return child("name")->get_text();
  }
};

//==============================================================================
