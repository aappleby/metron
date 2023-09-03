#include "NodeTypes.hpp"

#include "CInstance.hpp"
#include "CNodeClass.hpp"
#include "CNodeType.hpp"

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

uint32_t CNodeTranslationUnit::debug_color() const { return 0xFFFF00; }

std::string_view CNodeTranslationUnit::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTranslationUnit::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

Err CNodeTranslationUnit::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeNamespace::debug_color() const { return 0xFFFFFF; }

std::string_view CNodeNamespace::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeNamespace::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeNamespace::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodePreproc::debug_color() const { return 0x00BBBB; }

std::string_view CNodePreproc::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodePreproc::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto v = get_text();

  if (v.starts_with("#include")) {
    err << cursor.emit_replacements(this, "#include", "`include", ".h", ".sv");
  } else if (v.starts_with("#ifndef")) {
    err << cursor.emit_replacements(this, "#ifndef", "`ifndef");
  } else if (v.starts_with("#define")) {
    err << cursor.emit_replacements(this, "#define", "`define");
  } else if (v.starts_with("#endif")) {
    err << cursor.emit_replacements(this, "#endif", "`endif");
  } else {
    return ERR("Don't know how to handle this preproc");
  }

  return err << cursor.check_done(this);
}

Err CNodePreproc::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeIdentifier::debug_color() const { return 0x80FF80; }

std::string_view CNodeIdentifier::get_name() const {
  return get_text();
}

Err CNodeIdentifier::emit(Cursor& cursor) { return cursor.emit_default(this); }

Err CNodeIdentifier::trace(CCall* call) {
  if (auto inst_field = call->inst_class->resolve(this)) {
    return inst_field->log_action(this, ACT_READ);
  }
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodePunct::debug_color() const { return 0x88FF88; }

std::string_view CNodePunct::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodePunct::emit(Cursor& c) {
  return c.emit_default(this);
}

Err CNodePunct::trace(CCall* call) {
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeFieldExpression::debug_color() const { return 0x80FF80; }

std::string_view CNodeFieldExpression::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeFieldExpression::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeFieldExpression::trace(CCall* call) {
  Err err;

  auto inst = call->inst_class->resolve(this);
  err << inst->log_action(this, ACT_READ);

  return err;
}

//------------------------------------------------------------------------------

uint32_t CNodeQualifiedIdentifier::debug_color() const { return 0x80FF80; }

std::string_view CNodeQualifiedIdentifier::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeQualifiedIdentifier::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeQualifiedIdentifier::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeText::debug_color() const { return 0x888888; }

std::string_view CNodeText::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeText::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeText::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeKeyword::debug_color() const { return 0xFFFF88; }

std::string_view CNodeKeyword::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeKeyword::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeKeyword::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeTypedef::debug_color() const { return 0xFFFF88; }

std::string_view CNodeTypedef::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTypedef::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeTypedef::trace(CCall* call) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeList::debug_color() const { return 0xCCCCCC; }

std::string_view CNodeList::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeList::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
}

Err CNodeList::trace(CCall* call) {
  Err err;
  for (auto child : this) {
    err << child->trace(call);
  }
  return err;
}

//------------------------------------------------------------------------------
