#pragma once
#include <string>
#include <vector>

#include "MtNode.h"
#include "Platform.h"
#include "tree_sitter/api.h"

struct MnTranslationUnit;
struct MtModule;
struct MtModLibrary;
typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtSourceFile {
  MtSourceFile(const std::string& _filename, const std::string& _full_path, const std::string& _src_blob);
  ~MtSourceFile();

  void collect_modules(MnNode toplevel);

  MtModLibrary* lib = nullptr;

  const std::string filename;
  const std::string full_path;
  const std::string src_blob;
  bool use_utf8_bom = false;

  MnTranslationUnit root_node;

  const char* source = nullptr;
  const char* source_end = nullptr;
  const TSLanguage* lang = nullptr;
  TSParser* parser = nullptr;
  TSTree* tree = nullptr;

  std::vector<MtModule*> modules;
};

//------------------------------------------------------------------------------
