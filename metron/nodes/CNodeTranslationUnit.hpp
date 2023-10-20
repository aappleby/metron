#pragma once

#include "metron/CNode.hpp"

struct CSourceRepo;
struct CSourceFile;

//==============================================================================

struct CNodeTranslationUnit : public CNode {
  void init();
  CSourceRepo* get_repo() override;

  CSourceFile* file = nullptr;
};

//==============================================================================
