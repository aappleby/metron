#include "MtField.h"

#include "Log.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtSourceFile.h"

//------------------------------------------------------------------------------

MtField::MtField(MtModule *_parent_mod, const MnNode &n, bool is_public)
    : _parent_mod(_parent_mod), _public(is_public), node(n) {
  assert(node.sym == sym_field_declaration);
  _name = node.name4();
  _type = node.type5();
  _type_mod = _parent_mod->lib->get_module(_type);
}

//------------------------------------------------------------------------------

bool MtField::is_component() const {
  return (node.source->lib->get_module(_type));
}

bool MtField::is_param() const { return node.is_static() && node.is_const(); }

bool MtField::is_public() const { return _public; }

const char *MtField::cname() const { return _name.c_str(); }

const std::string &MtField::name() const { return _name; }

const std::string &MtField::type_name() const { return _type; }

// MnNode get_type_node() const { return node.get_field(field_type); }
// MnNode get_decl_node() const { return node.get_field(field_declarator); }

//------------------------------------------------------------------------------

void MtField::dump() {
  LOG_INDENT_SCOPE();
  if (is_component()) {
    LOG_C(0xFF80CC, "Component %s : %s\n", cname(), _type.c_str());
  } else {
    switch (state) {
      case FIELD_UNKNOWN:
        LOG_C(0x808080, "Unknown field %s : %s\n", cname(), _type.c_str());
        break;
      case FIELD_INPUT:
        LOG_C(0xFFFFFF, "-> Input %s : %s\n", cname(), _type.c_str());
        break;
      case FIELD_OUTPUT:
        LOG_C(0xAAAAFF, "<- Output %s : %s\n", cname(), _type.c_str());
        break;
      case FIELD_SIGNAL:
        LOG_C(0xAACCFF, "-- Signal %s : %s\n", cname(), _type.c_str());
        break;
      case FIELD_REGISTER:
        LOG_C(0xAAFFAA, ">| Register %s : %s\n", cname(), _type.c_str());
        break;
      case FIELD_INVALID:
        LOG_C(0x8080FF, "Invalid field %s : %s\n", cname(), _type.c_str());
        break;
    }
  }
}

//------------------------------------------------------------------------------
