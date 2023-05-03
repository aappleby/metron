#include "metron/nodes/MtEnum.h"

MtEnum::MtEnum(const MnNode& n) : node(n) {}

std::string MtEnum::name() {
  assert(node.sym == sym_field_declaration);
  auto enum_type = node.get_field(field_type);
  auto enum_name = enum_type.get_field(field_name);
  return enum_name.text();
}
