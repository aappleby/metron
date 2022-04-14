#pragma once
#include <string>
#include <vector>

#include "Err.h"
#include "Platform.h"

struct MtModule;
struct MtSourceFile;

//------------------------------------------------------------------------------

struct MtModLibrary {
  void add_search_path(const std::string& path);
  void add_source(MtSourceFile* source_file);
  
  CHECK_RETURN Err load_source(const char* name, MtSourceFile*& out_source);
  CHECK_RETURN Err load_blob(const std::string& filename, const std::string& full_path,
                 const std::string& src_blob, bool use_utf8_bom = false);

  CHECK_RETURN Err process_sources();

  MtModule* get_module(const std::string& module_name);
  MtSourceFile* get_source(const std::string& filename);

  void teardown();

  //----------

  std::vector<std::string> search_paths;
  std::vector<MtSourceFile*> source_files;
  std::vector<MtModule*> modules;

  bool sources_loaded = false;
  bool sources_processed = false;
};

//------------------------------------------------------------------------------
