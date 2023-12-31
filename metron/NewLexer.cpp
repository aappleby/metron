#include "NewLexer.hpp"

#include "main/main.h"
#include "metrolib/core/Log.h"
#include "matcheroni/Utilities.hpp"

int test_newlex(Options& opts) {
  LOG_R("Testing new lexer\n");

  std::string src_blob = matcheroni::utils::read(opts.src_name);

  printf("%s\n", src_blob.c_str());

  LOG("");
  return 0;
}
