#include "MtContext.h"

#include "Log.h"
#include "MtModule.h"

//------------------------------------------------------------------------------

std::string MtContext::get_path() const {
  if (parent) {
    return parent->get_path() + "." + name;
  } else {
    return name;
  }
}

MtContext *MtContext::get_child(const std::string &name) const {
  for (auto c : children)
    if (c->name == name) return c;
  return nullptr;
}

//------------------------------------------------------------------------------

/*
MtContext *MtField::instantiate(MtContext *parent) {
  MtContext *result = new MtContext();
  result->parent = parent;
  result->name = _name;
  result->field = this;
  result->mod = _parent_mod->lib->get_module(_type);

  if (result->mod) {
    for (auto f : result->mod->all_fields) {
      result->children.push_back(f->instantiate(result));
    }
  }
  return result;
}
*/

//------------------------------------------------------------------------------

void MtContext::dump() const {
  LOG_G("%s", get_path().c_str());

  if (field) {
    if (field->_type_mod) {
      LOG_G(" : %s", field->_type_mod->cname());
    } else {
      LOG_G(" : %s", field->_type.c_str());
    }
  }

  LOG_G("\n");

  // LOG_INDENT_SCOPE();
  for (auto inst : children) {
    inst->dump();
  }
}

//------------------------------------------------------------------------------
