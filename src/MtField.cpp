#include "MtField.h"

#include "Log.h"
#include "MtMethod.h"
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

bool MtField::is_component() const { return _type_mod != nullptr; }

bool MtField::is_param() const { return node.is_static() && node.is_const(); }

bool MtField::is_public() const { return _public; }

const char *MtField::cname() const { return _name.c_str(); }

const std::string &MtField::name() const { return _name; }

const std::string &MtField::type_name() const { return _type; }

// MnNode get_type_node() const { return node.get_field(field_type); }
// MnNode get_decl_node() const { return node.get_field(field_declarator); }

//------------------------------------------------------------------------------

void MtField::dump() const {
  LOG_INDENT_SCOPE();
  if (_type_mod) {
    LOG_C(0xFF80CC, "Component '%s' : %s", cname(), _type.c_str());
  } else {
    switch (state) {
      case CTX_NONE:
        LOG_C(0x808080, "Unknown field '%s' : %s", cname(), _type.c_str());
        break;
      case CTX_INPUT:
        LOG_C(0xFFFFFF, "-> Input '%s' : %s", cname(), _type.c_str());
        break;
      case CTX_OUTPUT:
        LOG_C(0xAAAAFF, "<- Output '%s' : %s", cname(), _type.c_str());
        break;
      case CTX_SIGNAL:
        LOG_C(0xAACCFF, "-- Signal '%s' : %s", cname(), _type.c_str());
        break;
      case CTX_REGISTER:
      case CTX_MAYBE:
        LOG_C(0xAAFFAA, ">| Register '%s' : %s", cname(), _type.c_str());
        break;
      case CTX_INVALID:
        LOG_C(0x8080FF, "Invalid field '%s' : %s", cname(), _type.c_str());
        break;
    }

    LOG(" = %s", to_string(state));

    if (written_by) {
      LOG(" (Written by %s)", written_by->cname());
    }
  }

  LOG("\n");
}

//------------------------------------------------------------------------------
