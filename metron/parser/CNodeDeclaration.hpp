#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

struct CNodeType;

//==============================================================================
//
//==============================================================================

struct CNodeDeclaration : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CCall* call) override;

  std::string_view get_type_name() const;
  bool is_array() const;

  CNodeClass*  _type_class = nullptr;
  CNodeStruct* _type_struct = nullptr;
};
