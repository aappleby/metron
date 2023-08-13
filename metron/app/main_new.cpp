#include "MetronApp.h"
#include "metrolib/core/Log.h"

#include "metron/parser/CSourceFile.hpp"
#include "metron/parser/CSourceRepo.hpp"

//------------------------------------------------------------------------------

int main_new(Options opts) {
  LOG_G("New parser!\n");

  Err err;

  CSourceRepo repo;
  CSourceFile* root_file = nullptr;
  err << repo.load_source(opts.src_name, &root_file);

  return 0;
}
