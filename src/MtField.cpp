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
  assert(_parent_mod);

  this->_node = n;
  this->_name = _node.name4();
  this->_type = _node.type5();
  this->_public = is_public;
  this->_static = _node.is_static();
  this->_const = _node.is_const();
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
  this->_static = _node.is_static();
  this->_const = _node.is_const();
  this->_is_enum = false;

  this->_parent_mod = nullptr;
  this->_type_mod = nullptr;

  this->_parent_struct = _parent_struct;
  this->_type_struct = _parent_struct->lib->get_struct(this->_type);
}

//------------------------------------------------------------------------------

const char *MtField::cname() const {
  return _name.c_str();
}

const std::string &MtField::name() const {
  return _name;
}

const std::string &MtField::type_name() const {
  return _type;
}

//------------------------------------------------------------------------------

bool MtField::is_component() const {
  return _type_mod != nullptr;
}

bool MtField::is_struct() const {
  return _type_struct != nullptr;
}

bool MtField::is_enum() {
  return _node.sym == sym_enum_specifier;
}

bool MtField::is_param() const {
  return _node.is_static() && _node.is_const();
}

bool MtField::is_public() const {
  return _public;
}

bool MtField::is_private() const {
  return !_public;
}

bool MtField::is_port() const {
  return is_public();
}

bool MtField::is_input() const {
  return _state == CTX_INPUT;
}

bool MtField::is_register() const {
  return _state == CTX_REGISTER || _state == CTX_MAYBE;
}

bool MtField::is_signal() const {
  return _state == CTX_OUTPUT || _state == CTX_SIGNAL;
}

bool MtField::is_dead() const {
  return _state == CTX_NONE;
}

MnNode MtField::get_type_node() const {
  return _node.get_field(field_type);
}

MnNode MtField::get_decl_node() const {
  return _node.get_field(field_declarator);
}

//------------------------------------------------------------------------------

MtField* MtField::get_field(MnNode node) {
  if (_type_struct) {
    return _type_struct->get_field(node);
  }
  else if (_type_mod) {
    return _type_mod->get_field(node);
  }
  else {
    return nullptr;
  }
}

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
