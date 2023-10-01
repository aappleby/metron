#pragma once

#include <filesystem>
#include <map>
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

  CNodeClass*     get_class(std::string_view name);
  CNodeStruct*    get_struct(std::string_view name);
  CNodeNamespace* get_namespace(std::string_view name);
  CNodeEnum*      get_enum(std::string_view name);
  CInstClass*     get_instance(std::string name);

  std::string resolve_filename(const std::string& filename);
  Err load_source(std::string filename, CSourceFile** out_source = nullptr);

  std::vector<std::string> search_paths = {""};
  std::map<std::string, CSourceFile*> source_map;

  std::vector<CNodeClass*>     all_classes;
  std::vector<CNodeStruct*>    all_structs;
  std::vector<CNodeNamespace*> all_namespaces;
  std::vector<CNodeEnum*>      all_enums;
  std::vector<CInstClass*>     all_instances;
};

//------------------------------------------------------------------------------
