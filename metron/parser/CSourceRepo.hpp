#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "metrolib/core/Err.h"

class CSourceFile;

//------------------------------------------------------------------------------

class CSourceRepo {
 public:
  std::string resolve_path(const std::string& filename);
  [[nodiscard]] Err load_source(std::string filename, CSourceFile** out_source = nullptr);

  std::vector<std::string> search_paths = {""};
  std::map<std::string, CSourceFile*> source_map;
};

//------------------------------------------------------------------------------
