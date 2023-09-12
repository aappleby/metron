#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"
#include "metron/tools/MtUtils.h"

struct CNodeKeyword;
struct CNodeIdentifier;
struct CNodeList;
struct CNodePunct;
struct CNodeExpression;

//------------------------------------------------------------------------------

struct CNodeField : public CNode {

  //----------------------------------------
  // Methods to be implemented by subclasses.

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(CCall* call) override;

  //----------------------------------------

  void init(const char* match_tag, SpanType span, uint64_t flags);

  Err emit_component(Cursor& cursor);

  std::string_view get_type_name() const;

  bool is_component() const;
  bool is_struct() const;
  bool is_array() const;
  bool is_const_char() const;

  void dump();

  CNodeKeyword*    node_static = nullptr;
  CNodeKeyword*    node_const = nullptr;
  CNode*           node_type = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_array = nullptr;
  CNodePunct*      node_eq = nullptr;
  CNodeExpression* node_value = nullptr;

  FieldType field_type = FT_UNKNOWN;

  CNodeClass*  _parent_class;
  CNodeStruct* _parent_struct;

  CNodeClass*  _type_class;
  CNodeStruct* _type_struct;

  bool _public = false;
  bool _enum = false;
};

//------------------------------------------------------------------------------
