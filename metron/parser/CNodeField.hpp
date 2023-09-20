#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"
#include "metron/tools/MtUtils.h"
#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

struct CNodeField : public CNode {

  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  //----------------------------------------

  Err emit_component(Cursor& cursor);
  Err emit_submod_binding_fields(Cursor& cursor);

  std::string_view get_type_name() const;

  bool is_component() const;
  bool is_struct() const;
  bool is_array() const;
  bool is_const_char() const;

  void dump();

  CNodeDeclaration* node_decl;
  CNodePunct*       node_semi;

  //FieldType field_type = FT_UNKNOWN;

  CNodeClass*  parent_class;
  CNodeStruct* parent_struct;

  bool is_public = false;
  //bool _enum = false;
};

//------------------------------------------------------------------------------
