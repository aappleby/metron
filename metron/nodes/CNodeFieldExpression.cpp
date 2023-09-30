#include "CNodeFieldExpression.hpp"

#include "metron/CInstance.hpp"
#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"

//==============================================================================

void CNodeFieldExpression::init(const char* match_tag, SpanType span,
                                uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_path = child("field_path")->as<CNodeIdentifier>();
  node_name = child("identifier")->as<CNodeIdentifier>();
}

//------------------------------------------------------------------------------

uint32_t CNodeFieldExpression::debug_color() const { return 0x80FF80; }

//------------------------------------------------------------------------------

std::string_view CNodeFieldExpression::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//------------------------------------------------------------------------------

Err CNodeFieldExpression::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto inst_dst = inst->resolve(this);

  err << inst_dst->log_action(this, ACT_READ, stack);

  return err;
}

//------------------------------------------------------------------------------
// Replace foo.bar.baz with foo_bar_baz if the field refers to a submodule port,
// so that it instead refers to a glue expression.

Err CNodeFieldExpression::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_func = ancestor<CNodeFunction>();
  auto node_class = ancestor<CNodeClass>();
  auto node_path = child("field_path");
  auto node_name = child("identifier");

  auto field = node_class->get_field(node_path->get_text());

  if (field && field->node_decl->_type_struct) {
    err << cursor.emit_default(this);
    return err;
  }

  // FIXME this needs the submod_ prefix for submod ports

  if (field) {
    auto field = get_textstr();
    for (auto& c : field) {
      if (c == '.') c = '_';
    }
    err << cursor.emit_replacement(this, field.c_str());
  } else {
    err << cursor.emit_default(this);
  }

  return err << cursor.check_done(this);
}

//==============================================================================
