#pragma once

#include "metron/CNode.hpp"

struct CSourceRepo;
struct CNodeKeyword;
struct CNodeList;
struct CNodePunct;
struct CNodeField;

// TODO - look into Verilog "interface" support

//==============================================================================

struct CNodeStruct : public CNode {
  void init();
  CNodeField* get_field(std::string_view name);

  CNode* resolve(std::vector<CNode*> path);

  //----------------------------------------

  CNodeKeyword* node_struct = nullptr;
  CNode* node_name = nullptr;
  CNodeList* node_body = nullptr;
  CNodePunct* node_semi = nullptr;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================
