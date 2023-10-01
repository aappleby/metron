#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeType.hpp"

struct CSourceRepo;
struct CSourceFile;

//==============================================================================

struct CNodeEnum : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

    node_enum = child("enum")->as<CNodeKeyword>();
    node_class = child("class")->as<CNodeKeyword>();
    node_name = child("name")->as<CNodeIdentifier>();
    node_colon = child("colon")->as<CNodePunct>();
    node_type = child("base_type")->as<CNodeType>();
    node_body = child("body")->as<CNodeList>();
    node_decl = child("decl");
    node_semi = child("semi")->as<CNodePunct>();
  }

  std::string_view get_name() const override {
    auto n = child("name");
    return n ? n->get_text() : "<unnamed>";
  }

  void dump() const override {
    auto name = get_name();
    LOG_G("Enum %.*s\n", name.size(), name.data());
  }

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;

  CNodeKeyword* node_enum = nullptr;
  CNodeKeyword* node_class = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodePunct* node_colon = nullptr;
  CNodeType* node_type = nullptr;
  CNodeList* node_body = nullptr;
  CNode* node_decl = nullptr;
  CNodePunct* node_semi = nullptr;
};

//==============================================================================

struct CNodeEnumerator : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
  }
};

//==============================================================================

struct CNodeEnumType : public CNodeType {
  std::string_view get_name() const override {
    return child("name")->get_text();
  }
};

//==============================================================================
