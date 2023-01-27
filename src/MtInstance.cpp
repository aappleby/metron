#include "MtInstance.h"

#include "Log.h"
#include "MtModule.h"
#include "MtMethod.h"
#include "MtStruct.h"
#include "MtField.h"

//------------------------------------------------------------------------------

MtInstance::MtInstance(const std::string& name) : _name(name) {
}

MtInstance::~MtInstance() {
}

void MtInstance::dump() {
  LOG_R("MtInstance\n");
}

//------------------------------------------------------------------------------

MtPrimitiveInstance::MtPrimitiveInstance(MtField* _field) : MtInstance(_field->name()) {
  this->_field = _field;
}

MtPrimitiveInstance::~MtPrimitiveInstance() {
}

void MtPrimitiveInstance::dump() {
  LOG_B("MtPrimitiveInstance '%s' @ %p\n", _name.c_str(), this);
}

//------------------------------------------------------------------------------

MtArrayInstance::MtArrayInstance(MtField* _field) : MtInstance(_field->name()) {
  this->_field = _field;
}

MtArrayInstance::~MtArrayInstance() {
}

void MtArrayInstance::dump() {
  LOG_B("MtArrayInstance '%s' @ %p\n", _name.c_str(), this);
}

//------------------------------------------------------------------------------

MtMethodInstance::MtMethodInstance(MtMethod* m) : _method(m), MtInstance(m->name()) {
  for (auto c : _method->param_nodes) {
    /*
    _args.push_back(new MtArgumentInstance(c.text()));
    */
  }

  if (_method->has_return()) {
    //_retval = new MtRetvalInstance();
  }
}

MtMethodInstance::~MtMethodInstance() {
  for (auto a : _args) delete a;
}

void MtMethodInstance::dump() {
  LOG_B("MtMethodInstance '%s' @ %p\n", _name.c_str(), this);
  LOG_INDENT_SCOPE();
  for (auto arg : _args) arg->dump();
  if (_retval) _retval->dump();
}

//------------------------------------------------------------------------------

MtStructInstance::MtStructInstance(MtField* _field) : MtInstance(_field->name()) {
  this->_field = _field;
  assert(_field && _field->_type_struct);

  for (auto cf : _field->_type_struct->fields) {
    if (cf->is_struct()) {
      _fields.push_back(new MtStructInstance(cf));
    }
    else if (cf->is_component()) {
      _fields.push_back(new MtModuleInstance(cf));
    }
    else if (cf->is_array()) {
      _fields.push_back(new MtArrayInstance(cf));
    }
    else {
      _fields.push_back(new MtPrimitiveInstance(cf));
    }
  }
}

void MtStructInstance::dump() {
  LOG_B("MtStructInstance '%s' @ %p\n", _name.c_str(), this);
  LOG_INDENT_SCOPE();
  for (auto f : _fields) f->dump();
}

//------------------------------------------------------------------------------

MtModuleInstance::MtModuleInstance(MtModule* _mod) : MtInstance("<top>") {
  assert(_mod);

  this->_field = nullptr;
  this->_mod = _mod;

  for (auto cf : _mod->all_fields) {
    if (cf->is_struct()) {
      _fields.push_back(new MtStructInstance(cf));
    }
    else if (cf->is_component()) {
      _fields.push_back(new MtModuleInstance(cf));
    }
    else if (cf->is_array()) {
      _fields.push_back(new MtArrayInstance(cf));
    }
    else {
      _fields.push_back(new MtPrimitiveInstance(cf));
    }
  }

  for (auto m : _mod->all_methods) {
    _methods.push_back(new MtMethodInstance(m));
  }
}


MtModuleInstance::MtModuleInstance(MtField* _field) : MtInstance(_field->name()) {
  assert(_field && _field->_type_mod);

  this->_field = _field;
  this->_mod = _field->_type_mod;

  for (auto cf : _field->_type_mod->all_fields) {
    if (cf->is_struct()) {
      _fields.push_back(new MtStructInstance(cf));
    }
    else if (cf->is_component()) {
      _fields.push_back(new MtModuleInstance(cf));
    }
    else if (cf->is_array()) {
      _fields.push_back(new MtArrayInstance(cf));
    }
    else {
      _fields.push_back(new MtPrimitiveInstance(cf));
    }
  }

  for (auto m : _mod->all_methods) {
    _methods.push_back(new MtMethodInstance(m));
  }
}

MtModuleInstance::~MtModuleInstance() {
  for (auto f : _fields) delete f;
  for (auto m : _methods) delete m;
}

void MtModuleInstance::dump() {
  LOG_B("MtModuleInstance '%s' @ %p\n", _name.c_str(), this);
  LOG_INDENT_SCOPE();
  for (auto f : _fields)  f->dump();
  for (auto m : _methods) m->dump();
}

MtMethodInstance* MtModuleInstance::get_method(const std::string& name) {
  for (auto m : _methods) if (m->_name == name) return m;
  return nullptr;
}

MtInstance* MtModuleInstance::get_field(const std::string& name) {
  for (auto f : _fields) if (f->_name == name) return f;
  return nullptr;
}

//------------------------------------------------------------------------------
