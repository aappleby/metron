#pragma once

#include "metron/Cursor.hpp"
#include "metron/CNode.hpp"

struct CNodeKeyword;
struct CNodeType;
struct CNodeIdentifier;
struct CNodeList;
struct CNodePunct;

//==============================================================================

struct CNodeDeclaration : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  void dump() const override;

  std::string_view get_type_name() const;
  bool is_array() const;

  bool is_localparam() const {
    return child("static") != nullptr && child("const") != nullptr;
  }

  CNodeKeyword*    node_static = nullptr;
  CNodeKeyword*    node_const = nullptr;
  CNodeType*       node_type = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_array = nullptr;
  CNodePunct*      node_eq = nullptr;
  CNode*           node_value = nullptr;

  CNodeClass*  _type_class = nullptr;
  CNodeStruct* _type_struct = nullptr;
};

//==============================================================================
