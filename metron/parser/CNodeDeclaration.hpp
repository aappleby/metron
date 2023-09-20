#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "NodeTypes.hpp"

//==============================================================================
//
//==============================================================================

struct CNodeDeclaration : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  void dump();

  std::string_view get_type_name() const;
  bool is_array() const;

  bool is_localparam() const {
    return child("static") != nullptr && child("const") != nullptr;
  }

  /*
  TokenSpan match_declaration(CContext& ctx, TokenSpan body) {
    using pattern =
    Seq<
      Any<
        Tag<"static", cap_keyword<"static">>,
        Tag<"const",  cap_keyword<"const">>
      >,
      Tag<"type",      Ref<cap_type>>,
      Tag<"name",      cap_identifier>,
      Any<Tag<"array", cap_index_list>>,
      Opt<Seq<
        Tag<"eq", cap_punct<"=">>,
        Tag<"value",   cap_expression>
      >>
    >;
    return pattern::match(ctx, body);
  }
  */

  CNodeKeyword*    node_static = nullptr;
  CNodeKeyword*    node_const = nullptr;
  CNodeType*       node_type = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_array = nullptr;
  CNodePunct*      node_eq = nullptr;
  CNode*           node_value = nullptr;

  CNodeClass*  _type_class = nullptr;
  CNodeStruct* _type_struct = nullptr;
};
