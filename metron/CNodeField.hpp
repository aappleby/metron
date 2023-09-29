#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"
#include "metron/MtUtils.h"
#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

struct CNodeField : public CNode {

  void init(const char* match_tag, SpanType span, uint64_t flags);

  //----------------------------------------

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  void dump() const override;

  //----------------------------------------

  Err emit_component(Cursor& cursor);

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
