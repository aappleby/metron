#pragma once

#include "CContext.hpp"
#include "CNode.hpp"

struct CSourceFile;
struct CSourceRepo;
struct CNodeField;

//------------------------------------------------------------------------------

struct CNodeStruct : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;

  Err collect_fields_and_methods();

  CHECK_RETURN Err emit(Cursor& cursor) override;

  void dump();

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  std::vector<CNodeField*> all_fields;
};

//------------------------------------------------------------------------------

struct CNodeUnion : public CNode {
};

//------------------------------------------------------------------------------

struct CNodeEnum : public CNode {
  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  void dump() {
    dump_tree();
  }
};

//------------------------------------------------------------------------------
