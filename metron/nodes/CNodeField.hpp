#pragma once

#include <stdint.h>  // for uint32_t, uint64_t

#include <string_view>  // for string_view

#include "metron/CNode.hpp"  // for CNode, CNode::SpanType

struct CNodeClass;
struct CNodeDeclaration;
struct CNodePunct;
struct CNodeStruct;

//------------------------------------------------------------------------------

struct CNodeField : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  //----------------------------------------

  std::string_view get_name() const override;

  //----------------------------------------

  std::string_view get_type_name() const;

  bool is_component() const;
  bool is_struct() const;
  bool is_array() const;
  bool is_const_char() const;

  //----------------------------------------

  bool is_public = false;

  CNodeDeclaration* node_decl;
  CNodePunct* node_semi;

  CNodeClass* parent_class;
  CNodeStruct* parent_struct;
};

//------------------------------------------------------------------------------
