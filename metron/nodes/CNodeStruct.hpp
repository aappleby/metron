#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

struct CSourceFile;
struct CSourceRepo;
struct CNodeKeyword;
struct CNodeList;
struct CNodePunct;

// TODO - look into Verilog "interface" support

//==============================================================================

struct CNodeStruct : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  void dump() const override ;

  //----------------------------------------

  Err collect_fields_and_methods();

  //----------------------------------------

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  CNodeKeyword* node_struct = nullptr;
  CNode* node_name          = nullptr;
  CNodeList* node_body      = nullptr;
  CNodePunct* node_semi     = nullptr;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================

struct CNodeStructType : public CNodeType {
  std::string_view get_name() const override;
};

//==============================================================================
