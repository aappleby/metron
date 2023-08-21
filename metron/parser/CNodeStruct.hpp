#pragma once

#include "CContext.hpp"
#include "CNode.hpp"

struct CSourceRepo;
struct CNodeField;

//------------------------------------------------------------------------------

struct CNodeStruct : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;

  Err collect_fields_and_methods(CSourceRepo* repo);

  void dump();

  std::vector<CNodeField*> all_fields;
};

//------------------------------------------------------------------------------
