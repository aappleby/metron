#pragma once
#include <string>
#include <vector>

#include "Err.h"
#include "MtNode.h"
#include "Platform.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

struct MnNode;
struct MtModule;
struct MtStruct;
struct MtModLibrary;
typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtSourceFile {
  CHECK_RETURN Err init(MtModLibrary* _lib, const std::string& _filename,
                        const std::string& _full_path,
                        void* _src_blob, int _src_len);
  ~MtSourceFile();

  CHECK_RETURN Err collect_modules(MnNode toplevel);
  //MtModule* get_module(const std::string& name);

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
};

//------------------------------------------------------------------------------
