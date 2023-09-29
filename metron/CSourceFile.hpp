#pragma once

#include <string>

#include "metron/CContext.hpp"
#include "metron/CLexer.hpp"
#include "metrolib/core/Err.h"

struct CSourceRepo;

//------------------------------------------------------------------------------

class CSourceFile {
 public:
  [[nodiscard]] Err init(CSourceRepo* _repo, const std::string& _filename,
                         const std::string& _filepath,
                         const std::string& _source_code, bool _use_utf8_bom);

  void dump();

  CSourceRepo* repo;
  std::string filename;
  std::string filepath;
  bool use_utf8_bom = false;

  CLexer lexer;
  CContext context;
};

//------------------------------------------------------------------------------
