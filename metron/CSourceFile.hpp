#pragma once

#include <string>

#include "metron/CContext.hpp"
#include "metron/CLexer.hpp"
#include "metrolib/core/Err.h"

struct CSourceRepo;
struct CNodeClass;
struct CNodeStruct;
struct CNodeNamespace;
struct CNodeEnum;
struct CInstClass;
struct CNodeTranslationUnit;
struct CNodeUnion;

//------------------------------------------------------------------------------

class CSourceFile {
 public:
  [[nodiscard]] Err init(CSourceRepo* _repo, const std::string& _filename,
                         const std::string& _filepath,
                         const std::string& _source_code, bool _use_utf8_bom);

  void link();

  CSourceRepo* get_repo() {
    return repo;
  }

  CSourceRepo* repo;
  std::string filename;
  std::string filepath;
  bool use_utf8_bom = false;

  CLexer lexer;
  CContext context;
  CNodeTranslationUnit* translation_unit = nullptr;

  std::vector<CNodeClass*>     all_classes;
  std::vector<CNodeStruct*>    all_structs;
  std::vector<CNodeUnion*>     all_unions;
  std::vector<CNodeNamespace*> all_namespaces;
  std::vector<CNodeEnum*>      all_enums;
};

//------------------------------------------------------------------------------
