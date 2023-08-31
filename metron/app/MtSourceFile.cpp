#include "metron/app/MtSourceFile.h"

#include "metron/app/MtModLibrary.h"
#include "metron/nodes/MtModule.h"
#include "metron/nodes/MtStruct.h"

#include "metrolib/core/Log.h"

#include <memory.h>

//#pragma warning(disable : 4996)

extern "C" {
extern const TSLanguage* tree_sitter_cpp();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtSourceFile::init(MtModLibrary* _lib,
                                    const std::string& _filename,
                                    const std::string& _full_path,
                                    const std::string& _src_blob,
                                    bool _use_utf8_bom) {
  Err err;

  lib = _lib;
  filename = _filename;
  full_path = _full_path;
  src_blob = _src_blob;
  use_utf8_bom = _use_utf8_bom;

  auto blob_size = src_blob.size();
  source = (const char*)src_blob.data();
  source_end = source + blob_size;

  // Parse the source file.
  parser = ts_parser_new();
  lang = tree_sitter_cpp();
  ts_parser_set_language(parser, lang);
  tree = ts_parser_parse_string(parser, NULL, source, (uint32_t)blob_size);

  // Pull out all modules from the top level of the source.
  TSNode ts_root = ts_tree_root_node(tree);
  auto root_sym = ts_node_symbol(ts_root);
  root_node = MnNode(ts_root, root_sym, 0, this);
  err << collect_modules_and_structs(root_node);

  return err;
}

//------------------------------------------------------------------------------

MtSourceFile::~MtSourceFile() {
  ts_tree_delete(tree);
  ts_parser_delete(parser);

  lang = nullptr;
  parser = nullptr;
  tree = nullptr;
  source = nullptr;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtSourceFile::collect_modules_and_structs(MnNode toplevel) {
  Err err;

  for (const auto& c : toplevel) {
    switch (c.sym) {
      case sym_struct_specifier: {
        MtStruct* new_struct = new MtStruct(c, lib);
        src_structs.push_back(new_struct);
        lib->all_structs.push_back(new_struct);
        break;
      }
      case sym_class_specifier:
      case sym_template_declaration: {
        MtModule* mod = new MtModule();
        err << mod->init(this, c);
        src_modules.push_back(mod);
        lib->all_modules.push_back(mod);
        break;
      }
      case sym_preproc_ifdef: {
        err << collect_modules_and_structs(c);
        break;
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------
