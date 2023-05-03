#include "metron/nodes/MtModParam.h"

#include "metrolib/core/Log.h"

MtModParam::MtModParam(MtModule* mod, MnNode& n) {
  _mod = mod;
  _node = n;
  _name = n.name4();
  _type = n.type5();
}

const std::string& MtModParam::name() const { return _name; }
const char* MtModParam::cname() const { return _name.c_str(); }
const std::string& MtModParam::type_name() const { return _type; }

MnNode MtModParam::get_type_node() const { return _node.get_field(field_type); }
MnNode MtModParam::get_decl_node() const {
  return _node.get_field(field_declarator);
}

// KCOV_OFF
void MtModParam::dump() {
  LOG_INDENT_SCOPE();
  LOG_Y("Modparam %s : %s\n", _name.c_str(), _type.c_str());
}
// KCOV_ON
