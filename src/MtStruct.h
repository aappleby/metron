#pragma once
#include <string>
#include <vector>

#include "MtNode.h"
#include "MtField.h"
#include "MtSourceFile.h"

struct MtStruct {
  MtStruct(MnNode _node, MtSourceFile* _source_file) {
    node = _node;
    name = node.name4();
    lib = _source_file->lib;
    source_file = _source_file;
  }

  MtField* get_field(MnNode node);

  std::string name;
  MnNode node;
  std::vector<MtField*> fields;
  MtSourceFile* source_file = nullptr;
  MtModLibrary* lib = nullptr;
};
