#pragma once

#include <vector>
#include "metron/CNode.hpp"

struct CNodeKeyword;
struct CNodeList;
struct CNodeClass;
struct CNodeDeclaration;

//------------------------------------------------------------------------------

struct CNodeTemplate : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override { return 0x00FFFF; }

  CNodeKeyword* node_template = nullptr;
  CNodeList*    node_params = nullptr;
  CNodeClass*   node_class = nullptr;

  std::vector<CNodeDeclaration*> params;
};

//------------------------------------------------------------------------------
