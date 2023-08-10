#pragma once
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"

#include <functional>
#include <string>
#include <vector>

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

  CHECK_RETURN Err load_includes(MtSourceFile* source_file);

  MtStruct* get_struct(const std::string& name) const;

  CHECK_RETURN Err collect_structs();
  CHECK_RETURN Err categorize_methods(bool verbose);

  MtModule* get_module(const std::string& module_name);
  MtSourceFile* get_source(const std::string& filename);

  CHECK_RETURN Err propagate(propagate_visitor v);


  std::string resolve_path(const std::string& path);

  void dump_lib();
  void teardown();

  //----------

  std::vector<std::string> search_paths;
  std::vector<MtSourceFile*> source_files;
  std::vector<MtModule*> all_modules;
  std::vector<MtStruct*> all_structs;
};

//------------------------------------------------------------------------------
