#include "CNodeFunction.hpp"

/*
TokenSpan CNodeFunction::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap2<"type",   CNodeType>,
    Cap2<"name",   CNodeIdentifier>,
    Cap2<"params", CNodeDeclList>,
    Opt<Cap3<"const",  Ref<match_keyword<"const">>>>,
    Cap2<"body",   CNodeCompound>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}
*/

std::string_view CNodeFunction::get_name() const {
  return child("name")->get_name();
}

Err CNodeFunction::emit(Cursor& cursor) {
  return cursor.emit_replacement(this, "{{CNodeFunction}}");
}
