#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

struct CSourceFile;
struct CSourceRepo;

// TODO - look into Verilog "interface" support

//==============================================================================

struct CNodeStruct : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  void dump() const override ;

  //----------------------------------------

  Err collect_fields_and_methods();

  //----------------------------------------

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================

struct CNodeStructType : public CNodeType {
  std::string_view get_name() const override;
};

//==============================================================================
