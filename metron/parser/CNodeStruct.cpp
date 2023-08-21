#include "CNodeStruct.hpp"

#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

uint32_t CNodeStruct::debug_color() const {
  return 0xFFAAAA;
}

//------------------------------------------------------------------------------

std::string_view CNodeStruct::get_name() const {
  return child("name")->get_name();
}

//------------------------------------------------------------------------------

Err CNodeStruct::collect_fields_and_methods(CSourceRepo* repo) {
  Err err;

  auto body = child("body");
  for (auto c : body) {
    if (auto n = c->as_a<CNodeField>()) {
      n->_parent_struct = n->ancestor<CNodeStruct>();
      n->_type_struct  = repo->get_struct(n->get_type_name());
      all_fields.push_back(n);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

void CNodeStruct::dump() {
  auto name = get_name();
  LOG_B("Struct %.*s @ %p\n", name.size(), name.data(), this);
  LOG_INDENT();

  if (all_fields.size()) {
    for (auto f : all_fields) f->dump();
  }

  LOG_DEDENT();
}

//------------------------------------------------------------------------------
