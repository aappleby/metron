#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeField.hpp"

struct CSourceRepo;
struct CSourceFile;

//==============================================================================

struct CNodeNamespace : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    color = 0x00FFFFFF;
    name = child("name")->name;
  }

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
      if (f->name == name) return f;
    }
    return nullptr;
  }

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================
