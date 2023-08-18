#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeField : public CNode {
  virtual std::string_view get_name() const override;
  virtual uint32_t debug_color() const override;

  virtual Err emit(Cursor& cursor) override;
};

//------------------------------------------------------------------------------
