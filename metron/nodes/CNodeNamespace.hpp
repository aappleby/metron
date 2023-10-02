#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeField.hpp"

struct CSourceRepo;
struct CSourceFile;

//==============================================================================

struct CNodeNamespace : public CNode {
  void init() {
    node_namespace = child("namespace");
    node_name      = child("name");
    node_fields    = child("fields");
    node_semi      = child("semi");
    name = node_name->name;
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

  CNode* node_namespace = nullptr;
  CNode* node_name      = nullptr;
  CNode* node_fields    = nullptr;
  CNode* node_semi      = nullptr;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================
