#pragma once
#include <functional>
#include <string>
#include <vector>

#include "Err.h"
#include "Platform.h"

struct MtMethod;
struct MtModule;
struct MtSourceFile;

typedef std::function<int(MtMethod*)> propagate_visitor;

//------------------------------------------------------------------------------

struct MtModLibrary {
  void add_search_path(const std::string& path);
  void add_source(MtSourceFile* source_file);

  CHECK_RETURN Err load_source(const char* name, MtSourceFile*& out_source,
                               bool verbose);
  CHECK_RETURN Err load_blob(const std::string& filename,
                             const std::string& full_path,
                             const std::string& src_blob, bool use_utf8_bom,
                             bool verbose);

  CHECK_RETURN Err process_sources();

  MtModule* get_module(const std::string& module_name);
  MtSourceFile* get_source(const std::string& filename);

  CHECK_RETURN Err propagate(propagate_visitor v);

  void dump();

  void dump_call_graph();

  void teardown();

  //----------

  std::vector<std::string> search_paths;
  std::vector<MtSourceFile*> source_files;
  std::vector<MtModule*> modules;
};

//------------------------------------------------------------------------------
