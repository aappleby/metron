#pragma once

#include <string>

#include "CContext.hpp"
#include "CLexer.hpp"
#include "metrolib/core/Err.h"

struct CSourceRepo;

//------------------------------------------------------------------------------

class CSourceFile {
 public:
  [[nodiscard]] Err init(CSourceRepo* _repo, const std::string& _filename,
                         const std::string& _filepath,
                         const std::string& _source_code, bool _use_utf8_bom);

  CSourceRepo* repo;
  std::string filename;
  std::string filepath;
  std::string source_code;
  bool use_utf8_bom = false;

  CLexer lexer;
  CContext context;
};

//------------------------------------------------------------------------------
