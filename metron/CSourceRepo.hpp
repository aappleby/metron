#pragma once

#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "metrolib/core/Err.h"
#include "metron/CInstance.hpp"

class CSourceFile;
struct CNode;
struct CNodeNamespace;
struct CNodeEnum;

//------------------------------------------------------------------------------

class CSourceRepo /* : public IContext maybe? */ {
 public:

  CSourceFile*    get_file(std::string_view absolute_path);
  CNodeClass*     get_class(std::string_view name);
  CNodeStruct*    get_struct(std::string_view name);
  CNodeUnion*     get_union(std::string_view name);
  CNodeNamespace* get_namespace(std::string_view name);
  CNodeEnum*      get_enum(std::string_view name);
  CInstClass*     get_instance(std::string name);

  std::string resolve_filename(const std::string& filename);
  Err load_source(std::string filename, CSourceFile** out_source = nullptr);
  void dump();

  std::set<std::string>     search_paths;
  std::vector<CSourceFile*> source_files;
  //std::map<std::string, CSourceFile*> source_map;
};

//------------------------------------------------------------------------------
