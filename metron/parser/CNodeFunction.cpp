#include "CNodeFunction.hpp"

std::string_view CNodeFunction::get_name() const {
  return child("name")->get_name();
}

Err CNodeFunction::emit(Cursor& cursor) {
  return cursor.emit_replacement(this, "{{CNodeFunction}}");
}
