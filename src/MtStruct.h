#pragma once
#include <string>
#include <vector>

#include "MtNode.h"

struct MtField;
struct MtModLibrary;

struct MtStruct {
  MtStruct(MnNode _node, MtModLibrary* _lib) {
    node = _node;
    name = node.name4();
    lib = _lib;
  }

  CHECK_RETURN Err collect_fields();

  // FIXME put something here

  void dump_struct() {}

  MtField* get_field(MnNode node);

  std::string name;
  MnNode node;
  std::vector<MtField*> fields;
  MtModLibrary* lib = nullptr;
};
