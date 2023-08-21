#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metron/tools/MtUtils.h"

struct CNodeClass;
struct CSourceRepo;
struct CInstance;

/*
[000.006]       ┣━━┳━ CNodeField field =
[000.006]       ┃  ┣━━┳━ CNodeType decl_type =
[000.006]       ┃  ┃  ┗━━━━ CNode type = "Snip"
[000.006]       ┃  ┗━━━━ CNodeIdentifier decl_name = "snip"
*/

//------------------------------------------------------------------------------

struct CNodeField : public CNode {

  void init(const char* match_tag, SpanType span, uint64_t flags);

  virtual std::string_view get_name() const override;
  std::string_view get_type_name() const;
  virtual uint32_t debug_color() const override;
  virtual Err emit(Cursor& cursor) override;

  bool is_component() const;
  bool is_struct() const;
  bool is_array() const;
  bool is_enum() const;
  bool is_port() const;
  bool is_param() const;
  bool is_public() const;
  bool is_private() const;

  bool is_input() const;
  bool is_register() const;
  bool is_signal() const;
  bool is_dead() const;

  void dump();
  void error();

  TraceState _state = CTX_PENDING;

  CNodeClass*  _parent_class;
  CNodeStruct* _parent_struct;

  CNodeClass*  _type_class;
  CNodeStruct* _type_struct;

  //TraceState _state = CTX_PENDING;

  bool _static = false;
  bool _const = false;
  bool _public = false;
  bool _enum = false;
};

//------------------------------------------------------------------------------
