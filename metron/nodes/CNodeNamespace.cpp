#include "CNodeNamespace.hpp"

#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeField.hpp"

//==============================================================================

uint32_t CNodeNamespace::debug_color() const { return 0xFFFFFF; }

//------------------------------------------------------------------------------

std::string_view CNodeNamespace::get_name() const {
  return child("name")->get_text();
}

//------------------------------------------------------------------------------

Err CNodeNamespace::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto node_namespace = child("namespace");
  auto node_name      = child("name");
  auto node_fields    = child("fields");
  auto node_semi      = child("semi");

  err << cursor.emit_replacement(node_namespace, "package");
  err << cursor.emit_gap();

  err << cursor.emit_raw(node_name);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  for (auto f : node_fields) {
    if (f->tag_is("ldelim")) {
      err << cursor.skip_over(f);
      err << cursor.emit_gap();
      continue;
    }
    else if (f->tag_is("rdelim")) {
      err << cursor.emit_replacement(f, "endpackage");
      err << cursor.emit_gap();
      continue;
    }
    else {
      err << f->emit(cursor);
      err << cursor.emit_gap();
      continue;
    }
  }

  // Don't need semi after namespace in Verilog
  err << cursor.skip_over(node_semi);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeNamespace::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//------------------------------------------------------------------------------

Err CNodeNamespace::collect_fields_and_methods() {
  for (auto c : child("fields")) {
    if (auto field = c->as<CNodeField>()) {
      all_fields.push_back(field);
    }
  }

  return Err();
}

//------------------------------------------------------------------------------

CNodeField* CNodeNamespace::get_field(std::string_view name) {
  for (auto f : all_fields) {
    if (f->get_name() == name) return f;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

void CNodeNamespace::dump() const {
  LOG_G("Fields:\n");
  LOG_INDENT_SCOPE();
  for (auto n : all_fields) n->dump();
}

//==============================================================================
