#pragma once

#include "CContext.hpp"
#include "CNode.hpp"

struct CSourceRepo;
struct CNodeField;

//------------------------------------------------------------------------------

struct CNodeStruct : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;

  Err collect_fields_and_methods(CSourceRepo* repo);

  CHECK_RETURN Err emit(Cursor& cursor) override;

  void dump();

  std::vector<CNodeField*> all_fields;
};

//------------------------------------------------------------------------------

struct CNodeUnion : public CNode {
};

//------------------------------------------------------------------------------

struct CNodeEnum : public CNode {
};

//------------------------------------------------------------------------------
