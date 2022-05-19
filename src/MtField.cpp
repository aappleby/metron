#include "MtField.h"

#include "Log.h"
#include "MtMethod.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "MtStruct.h"

//------------------------------------------------------------------------------

MtField::MtField(MtModule *_parent_mod, const MnNode &n, bool is_public) {
  assert(n.sym == sym_field_declaration);

  this->_node = n;
  this->_name = _node.name4();
  this->_type = _node.type5();
  this->_public = is_public;
  this->_is_enum = false;

  this->_parent_mod = _parent_mod;
  this->_type_mod = _parent_mod->lib->get_module(_type);

  this->_parent_struct = nullptr;
  this->_type_struct = nullptr;
}

//------------------------------------------------------------------------------

MtField::MtField(MtStruct *_parent_struct, const MnNode &n) {
  assert(n.sym == sym_field_declaration);

  this->_node = n;
  this->_name = _node.name4();
  this->_type = _node.type5();
  this->_is_enum = false;

  this->_parent_mod = nullptr;
  this->_type_mod = nullptr;

  this->_parent_struct = _parent_struct;
  this->_type_struct = _parent_struct->lib->get_struct(this->_type);
}

//------------------------------------------------------------------------------

bool MtField::is_component() const { return _type_mod != nullptr; }

bool MtField::is_struct() const { return _type_struct != nullptr; }

bool MtField::is_param() const { return _node.is_static() && _node.is_const(); }

bool MtField::is_public() const { return _public; }

const char *MtField::cname() const { return _name.c_str(); }

const std::string &MtField::name() const { return _name; }

const std::string &MtField::type_name() const { return _type; }

//------------------------------------------------------------------------------

void MtField::dump() const {
  LOG_INDENT_SCOPE();
  if (_is_enum) {
    LOG_C(0xFF80CC, "Enum '%s'", cname());
  } else if (_type_mod) {
    LOG_C(0xFF80CC, "Component '%s' : %s", cname(), _type.c_str());
  } else {
    switch (_state) {
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
      case CTX_PENDING:
        LOG_C(0x8080FF, "Pending field '%s' : %s", cname(), _type.c_str());
        break;
      case CTX_NIL:
        LOG_C(0x8080FF, "Nil field '%s' : %s", cname(), _type.c_str());
        break;
    }

    LOG(" = %s", to_string(_state));
  }

  LOG("\n");
}

//------------------------------------------------------------------------------
