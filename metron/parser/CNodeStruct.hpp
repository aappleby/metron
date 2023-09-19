#pragma once

#include "CContext.hpp"
#include "CNode.hpp"
#include "NodeTypes.hpp"

struct CSourceFile;
struct CSourceRepo;

//------------------------------------------------------------------------------

// TODO - look into Verilog "interface" support

struct CNodeStruct : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;

  Err collect_fields_and_methods();

  CHECK_RETURN Err emit(Cursor& cursor) override;

  virtual void wipe_field_types() {
  }

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

  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err emit(Cursor& cursor) override;

  std::string_view get_name() const override {
    auto n = child("name");
    return n ? n->get_text() : "<unnamed>";
  }

  void dump() {
    auto name = get_name();
    LOG_G("Enum %.*s\n", name.size(), name.data());
  }

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

//------------------------------------------------------------------------------

struct CNodeEnumerator : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//------------------------------------------------------------------------------
