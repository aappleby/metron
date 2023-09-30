#include "CNodeEnum.hpp"

#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeType.hpp"

//==============================================================================

void CNodeEnum::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_enum  = child("enum")->as<CNodeKeyword>();
  node_class = child("class")->as<CNodeKeyword>();
  node_name  = child("name")->as<CNodeIdentifier>();
  node_colon = child("colon")->as<CNodePunct>();
  node_type  = child("base_type")->as<CNodeType>();
  node_body  = child("body")->as<CNodeList>();
  node_decl  = child("decl");
  node_semi  = child("semi")->as<CNodePunct>();
}

CHECK_RETURN Err CNodeEnum::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  // Extract enum bit width, if present.
  cursor.override_size.push(32);
  if (node_type) {
    if (auto targs = node_type->child("template_args")->as<CNodeList>()) {
      cursor.override_size.top() = atoi(targs->items[0]->text_begin());
    }
  }

  if (!node_decl) {
    err << cursor.emit_print("typedef ");
  }

  err << cursor.emit(node_enum);
  err << cursor.emit_gap();

  if (node_class) {
    err << cursor.skip_over(node_class);
    err << cursor.skip_gap();
  }

  if (node_name) {
    err << cursor.skip_over(node_name);
    err << cursor.skip_gap();
  }

  if (node_colon) {
    err << cursor.skip_over(node_colon);
    err << cursor.skip_gap();
    err << cursor.emit(node_type);
    err << cursor.emit_gap();
  }

  err << cursor.emit(node_body);
  err << cursor.emit_gap();

  if (node_decl) {
    err << cursor.emit(node_decl);
    err << cursor.emit_gap();
  }

  if (node_name) {
    err << cursor.emit_print(" ");
    err << cursor.emit_splice(node_name);
  }

  err << cursor.emit(node_semi);

  cursor.override_size.pop();

  return err << cursor.check_done(this);
}

std::string_view CNodeEnum::get_name() const {
  auto n = child("name");
  return n ? n->get_text() : "<unnamed>";
}

void CNodeEnum::dump() const {
  auto name = get_name();
  LOG_G("Enum %.*s\n", name.size(), name.data());
}

//==============================================================================

void CNodeEnumerator::init(const char* match_tag, SpanType span,
                           uint64_t flags) {
  CNode::init(match_tag, span, flags);
}

CHECK_RETURN Err CNodeEnumerator::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  err << cursor.emit_default(this);

  return err << cursor.check_done(this);
}

//==============================================================================

CHECK_RETURN Err CNodeEnumType::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

//==============================================================================
