#pragma once

#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeList.hpp"

//==============================================================================

struct CNodeSwitch : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    node_switch = child("switch")->req<CNodeKeyword>();
    node_cond   = child("condition")->req<CNodeList>();
    node_body   = child("body")->req<CNodeList>();
  }

  CNodeKeyword* node_switch = nullptr;
  CNodeList*    node_cond   = nullptr;
  CNodeList*    node_body   = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeCase : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    node_case  = child("case")->req<CNodeKeyword>();
    node_cond  = child("condition")->req<CNode>();
    node_colon = child("colon")->req<CNodePunct>();
    node_body  = child("body")->opt<CNodeList>();
  }

  CNodeKeyword* node_case  = nullptr;
  CNode*        node_cond  = nullptr;
  CNodePunct*   node_colon = nullptr;
  CNodeList*    node_body  = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeDefault : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

    node_default = child("default")->req<CNodeKeyword>();
    node_colon   = child("colon")->req<CNodePunct>();
    node_body    = child("body")->opt<CNodeList>();
  }

  CNodeKeyword* node_default = nullptr;
  CNodePunct*   node_colon   = nullptr;
  CNodeList*    node_body    = nullptr;
};

//==============================================================================
