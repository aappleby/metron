#pragma once

#include "metron/CNode.hpp"

struct CNodeField;
struct CSourceRepo;
struct CSourceFile;

//==============================================================================

struct CNodeNamespace : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  Err collect_fields_and_methods();

  CNodeField* get_field(std::string_view name);

  void dump() const override;

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================
