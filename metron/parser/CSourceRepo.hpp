#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "metrolib/core/Err.h"
#include "CInstance.hpp"

class CSourceFile;
class CNodeClass;
class CNodeStruct;
class CNode;

//------------------------------------------------------------------------------

class CSourceRepo /* : public IContext maybe? */ {
 public:

  CNodeClass*  get_class(std::string_view name);
  CNodeStruct* get_struct(std::string_view name);

  std::string resolve_filename(const std::string& filename);
  Err load_source(std::string filename, CSourceFile** out_source = nullptr);
  Err collect_fields_and_methods();
  Err build_call_graphs();

  void categorize_methods();
  void dump();

  CNode* resolve(CNodeClass* parent, CNode* path);


  std::vector<std::string> search_paths = {""};
  std::map<std::string, CSourceFile*> source_map;

  std::vector<CNodeClass*>  all_classes;
  std::vector<CNodeStruct*> all_structs;

  CNodeClass* top = nullptr;
};

//------------------------------------------------------------------------------
