#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

struct CNodeClass;
struct CSourceRepo;

/*
[000.006]       ┣━━┳━ CNodeField field =
[000.006]       ┃  ┣━━┳━ CNodeType decl_type =
[000.006]       ┃  ┃  ┗━━━━ CNode type = "Snip"
[000.006]       ┃  ┗━━━━ CNodeIdentifier decl_name = "snip"
*/

//------------------------------------------------------------------------------

struct CNodeField : public CNode {
  virtual std::string_view get_name() const override;
  virtual uint32_t debug_color() const override;

  virtual Err emit(Cursor& cursor) override;

  std::string_view get_type_name() const;

  void dump();
};

//------------------------------------------------------------------------------
