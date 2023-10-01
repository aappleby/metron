#pragma once

#include "metron/CScope.hpp"
#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeCompound : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

    for (auto child : this) {
      if (!child->tag_is("ldelim") && !child->tag_is("rdelim")) {
        statements.push_back(child);
      }
    }
  }

  std::vector<CNode*> statements;
  CScope scope;
};

//==============================================================================
