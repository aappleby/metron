#pragma once

#include "metron/CNode.hpp"
#include "metron/CSourceRepo.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeType.hpp"

// TODO - look into Verilog "interface" support

//==============================================================================

struct CNodeStruct : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    node_struct = child("struct")->req<CNodeKeyword>();
    node_name   = child("name");
    node_body   = child("body")->req<CNodeList>();
    node_semi   = child("semi")->req<CNodePunct>();
  }

  //----------------------------------------

  uint32_t debug_color() const override {
    return 0xFFAAAA;
  }

  //----------------------------------------

  std::string_view get_name() const override {
    return child("name")->get_name();
  }

  //----------------------------------------

  Err collect_fields_and_methods(CSourceRepo* repo) {
    Err err;

    auto body = child("body");
    for (auto c : body) {
      if (auto n = c->as<CNodeField>()) {
        n->parent_struct = n->ancestor<CNodeStruct>();
        n->node_decl->_type_struct  = repo->get_struct(n->get_type_name());
        all_fields.push_back(n);
      }
    }

    return err;
  }

  //----------------------------------------

  CNodeKeyword* node_struct = nullptr;
  CNode* node_name = nullptr;
  CNodeList* node_body = nullptr;
  CNodePunct* node_semi = nullptr;

  std::vector<CNodeField*> all_fields;
};

//==============================================================================

struct CNodeStructType : public CNodeType {
  std::string_view get_name() const override {
    return child("name")->get_text();
  }
};

//==============================================================================
