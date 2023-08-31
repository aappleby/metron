#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

struct CNodeType;

//==============================================================================
//
//==============================================================================

struct CNodeDeclaration : public CNode {
  virtual std::string_view get_name() const override;
  std::string_view get_type_name() const;
  bool is_array() const;
  virtual uint32_t debug_color() const override;
  virtual Err emit(Cursor& cursor) override;

  CNodeClass*  _type_class = nullptr;
  CNodeStruct* _type_struct = nullptr;
};
