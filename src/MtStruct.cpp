#include "MtStruct.h"

#include "MtField.h"

Err MtStruct::collect_fields() {
  Err err;
  for (auto f : node.get_field(field_body)) {
    if (f.sym == sym_field_declaration) {
      MtField* new_field = new MtField(this, f);
      fields.push_back(new_field);
    }
  }
  return err;
}

MtField *MtStruct::get_field(MnNode node) {
  if (node.sym == sym_identifier || node.sym == alias_sym_field_identifier) {
    for (auto f : fields) {
      if (f->name() == node.text()) return f;
    }
    return nullptr;
  }
  else if (node.sym == sym_field_expression) {
    auto lhs = node.get_field(field_argument);
    auto rhs = node.get_field(field_field);

    auto lhs_field = get_field(lhs);
    return lhs_field->get_subfield(rhs);
  }
  else {
    return nullptr;
  }
}
