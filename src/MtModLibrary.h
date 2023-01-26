#pragma once
#include <functional>
#include <string>
#include <vector>

#include "Err.h"
#include "Platform.h"

struct MtMethod;
struct MtModule;
struct MtSourceFile;
struct MtStruct;

typedef std::function<int(MtMethod*)> propagate_visitor;

//------------------------------------------------------------------------------

struct MtModLibrary {
  void add_search_path(const std::string& path);
  void add_source(MtSourceFile* source_file);

  CHECK_RETURN Err load_source(const char* name, MtSourceFile*& out_source);
  CHECK_RETURN Err load_blob(const std::string& filename,
                             const std::string& full_path,
                             void* src_blob, int src_len,
                             MtSourceFile*& out_source,
                             bool use_utf8_bom);

  MtStruct* get_struct(const std::string& name) const;

  CHECK_RETURN Err collect_structs();
  CHECK_RETURN Err categorize_methods(bool verbose);

  MtModule* get_module(const std::string& module_name);
  MtSourceFile* get_source(const std::string& filename);

  CHECK_RETURN Err propagate(propagate_visitor v);


  void dump_lib();
  void teardown();

  //----------

  std::vector<std::string> search_paths;
  std::vector<MtSourceFile*> source_files;
  std::vector<MtModule*> all_modules;
  std::vector<MtStruct*> all_structs;
};

//------------------------------------------------------------------------------
