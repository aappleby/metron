#pragma once

#include <string>

#include "metron/CContext.hpp"
#include "metron/CLexer.hpp"
#include "metron/main/main.h"
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

  std::set<CSourceFile*> deps;
  std::set<CSourceFile*> transitive_deps;

  void update_transitive_deps() {
    if (!transitive_deps.empty()) return;

    transitive_deps.insert(deps.begin(), deps.end());
    for (auto file : deps) {
      file->update_transitive_deps();
      transitive_deps.insert(file->transitive_deps.begin(), file->transitive_deps.end());
    }
  }

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
