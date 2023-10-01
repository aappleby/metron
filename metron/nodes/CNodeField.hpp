#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"
#include "metron/MtUtils.h"

struct CNodeDeclaration;
struct CNodePunct;

//------------------------------------------------------------------------------

struct CNodeField : public CNode {

  void init(const char* match_tag, SpanType span, uint64_t flags);

  //----------------------------------------

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  void dump() const override;

  //----------------------------------------

  std::string_view get_type_name() const;

  bool is_component() const;
  bool is_struct() const;
  bool is_array() const;
  bool is_const_char() const;

  //----------------------------------------

  bool is_public = false;

  CNodeDeclaration* node_decl;
  CNodePunct*       node_semi;

  CNodeClass*  parent_class;
  CNodeStruct* parent_struct;
};

//------------------------------------------------------------------------------
