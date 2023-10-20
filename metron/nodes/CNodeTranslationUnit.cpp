#include "CNodeTranslationUnit.hpp"

#include "metron/CSourceRepo.hpp"
#include "metron/CSourceFile.hpp"

//==============================================================================

void CNodeTranslationUnit::init() {
  name = "<CNodeTranslationUnit>";
}

CSourceRepo* CNodeTranslationUnit::get_repo() {
  return file->get_repo();
}

//==============================================================================
