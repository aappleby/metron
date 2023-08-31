#include "metron/nodes/MtFuncParam.h"

#include "metron/nodes/TreeSymbols.h"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

MtFuncParam::MtFuncParam(const std::string& func_name, const MnNode& n)
    : node(n), func_name(func_name) {
  _name = node.name4();
  _type = node.type5();
}

const std::string& MtFuncParam::name() const { return _name; }

const char* MtFuncParam::cname() const { return _name.c_str(); }

const std::string& MtFuncParam::type_name() const { return _type; }

MnNode MtFuncParam::get_type_node() const { return node.get_field(field_type); }
MnNode MtFuncParam::get_decl_node() const {
  return node.get_field(field_declarator);
}

// KCOV_OFF
void MtFuncParam::dump() {
  LOG_INDENT_SCOPE();
  LOG_Y("Modparam %s : %s\n", _name.c_str(), _type.c_str());
}
// KCOV_ON

//------------------------------------------------------------------------------
