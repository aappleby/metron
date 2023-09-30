#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

struct CSourceRepo;
struct CSourceFile;
struct CNodeKeyword;
struct CNodeIdentifier;
struct CNodePunct;
struct CNodeList;
struct CNodeType;

//==============================================================================

struct CNodeEnum : public CNode {

  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err emit(Cursor& cursor) override;

  std::string_view get_name() const override;
  void dump() const override;

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;

  CNodeKeyword*    node_enum = nullptr;
  CNodeKeyword*    node_class = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodePunct*      node_colon = nullptr;
  CNodeType*       node_type = nullptr;
  CNodeList*       node_body = nullptr;
  CNode*           node_decl = nullptr;
  CNodePunct*      node_semi = nullptr;
};

//==============================================================================

struct CNodeEnumerator : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//==============================================================================

struct CNodeEnumType : public CNodeType {
  std::string_view get_name() const override {
    return child("name")->get_text();
  }
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//==============================================================================
