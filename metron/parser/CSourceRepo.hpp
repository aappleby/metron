#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "metrolib/core/Err.h"

class CSourceFile;
class CNodeClass;
class CNodeStruct;

//------------------------------------------------------------------------------

class CSourceRepo {
 public:

  CNodeClass*  get_module(std::string_view name);
  CNodeStruct* get_struct(std::string_view name);

  std::string resolve_path(const std::string& filename);
  Err load_source(std::string filename, CSourceFile** out_source = nullptr);
  Err collect_fields_and_methods();
  Err build_call_graphs();

  void categorize_methods();

  void dump();


  std::vector<std::string> search_paths = {""};
  std::map<std::string, CSourceFile*> source_map;

  std::vector<CNodeClass*>  all_modules;
  std::vector<CNodeStruct*> all_structs;
};

//------------------------------------------------------------------------------
