#pragma once

#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeExpStatement.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeStatement.hpp"

struct CNodeKeyword;
struct CNodeList;

//==============================================================================

struct CNodeIf : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

    node_kw_if = child("if")->req<CNodeKeyword>();
    node_cond = child("condition")->req<CNodeList>();
    node_then = child("body_true")->req<CNodeStatement>();
    node_kw_else = child("else")->as<CNodeKeyword>();
    node_else = child("body_false")->as<CNodeStatement>();
  }

  CNodeKeyword* node_kw_if = nullptr;
  CNodeList* node_cond = nullptr;
  CNodeStatement* node_then = nullptr;
  CNodeKeyword* node_kw_else = nullptr;
  CNodeStatement* node_else = nullptr;
};

//==============================================================================
