#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeField.hpp"

struct CSourceRepo;
struct CSourceFile;

//==============================================================================

struct CNodeNamespace : public CNode {
  uint32_t debug_color() const override { return 0xFFFFFF; }

  std::string_view get_name() const override { return child("name")->get_text(); }

  Err collect_fields_and_methods() {
    for (auto c : child("fields")) {
      if (auto field = c->as<CNodeField>()) {
        all_fields.push_back(field);
      }
    }

    return Err();
  }

  CNodeField* get_field(std::string_view name) {
    for (auto f : all_fields) {
      if (f->get_name() == name) return f;
    }
    return nullptr;
  }

  void dump() const override {
    LOG_G("Fields:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_fields) n->dump();
  }

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================
