#pragma once
#include "metron/nodes/MtNode.h"

#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"

#include "metron/parser/CContext.hpp"
#include "metron/parser/CParser.hpp"
#include "metron/parser/CLexer.hpp"

#include "tree-sitter/lib/include/tree_sitter/api.h"

#include <string>
#include <vector>

struct MnNode;
struct MtModule;
struct MtStruct;
struct MtModLibrary;
typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtSourceFile {
  CHECK_RETURN Err init(MtModLibrary* _lib,
                        const std::string& _filename,
                        const std::string& _full_path,
                        const std::string& _src_blob);
  ~MtSourceFile();

  CHECK_RETURN Err collect_modules_and_structs(MnNode toplevel);

  MtModLibrary* lib = nullptr;

  std::string filename;
  std::string full_path;
  std::string src_blob;
  bool use_utf8_bom = false;

  MnNode root_node;

  const char* source = nullptr;
  const char* source_end = nullptr;
  const TSLanguage* lang = nullptr;
  TSParser* parser = nullptr;
  TSTree* tree = nullptr;

  std::vector<MtModule*>     src_modules;
  std::vector<MtStruct*>     src_structs;
  std::vector<MtSourceFile*> src_includes;

  CLexer   lexer;
  CContext context;
};

//------------------------------------------------------------------------------
