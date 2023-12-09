#pragma once

#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeList.hpp"

//==============================================================================

struct CNodeSwitch : public CNodeStatement {
  void init() {
    node_kwswitch = child("switch")->req<CNodeKeyword>();
    node_cond     = child("condition")->req<CNodeList>();
    node_body     = child("body")->req<CNodeList>();
  }

  CNodeKeyword* node_kwswitch = nullptr;
  CNodeList*    node_cond     = nullptr;
  CNodeList*    node_body     = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeCase : public CNodeStatement {
  void init() {
    node_kwcase    = child("case")->opt<CNodeKeyword>();
    node_kwdefault = child("default")->opt<CNodeKeyword>();
    node_cond      = child("condition")->opt<CNode>();
    node_colon     = child("colon")->req<CNodePunct>();
    node_body      = child("body")->opt<CNodeList>();
  }

  CNodeKeyword* node_kwcase    = nullptr;
  CNodeKeyword* node_kwdefault = nullptr;
  CNode*        node_cond      = nullptr;
  CNodePunct*   node_colon     = nullptr;
  CNodeList*    node_body      = nullptr;
};

//==============================================================================
