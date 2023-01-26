#include "MtInstance.h"

#include "MtModule.h"
#include "MtMethod.h"
#include "MtStruct.h"

MtFieldInstance::MtFieldInstance(MtField* f) : _field(f) {
}

MtMethodInstance::MtMethodInstance(MtMethod* m) : _method(m) {
}

MtModuleInstance::MtModuleInstance(MtModule* m) : _mod(m) {

  for (auto f : _mod->all_fields) {
    _fields.push_back(new MtFieldInstance(f));
  }

  for (auto m : _mod->all_methods) {
    _methods.push_back(new MtMethodInstance(m));
  }
}

MtStructInstance::MtStructInstance(MtStruct* s) : _struct(s) {
  for (auto f : _struct->fields) {
    _fields.push_back(new MtFieldInstance(f));
  }
}
