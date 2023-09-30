#include "CNodeNamespace.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeField.hpp"

//==============================================================================

uint32_t CNodeNamespace::debug_color() const { return 0xFFFFFF; }

//------------------------------------------------------------------------------

std::string_view CNodeNamespace::get_name() const {
  return child("name")->get_text();
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
