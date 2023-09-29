#include "CSourceFile.hpp"

#include "CSourceRepo.hpp"
#include "metrolib/core/Log.h"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

Err CSourceFile::init(CSourceRepo* _repo, const std::string& _filename,
                      const std::string& _filepath,
                      const std::string& _source_code, bool _use_utf8_bom) {
  this->repo = _repo;
  this->filename = _filename;
  this->filepath = _filepath;
  this->use_utf8_bom = _use_utf8_bom;

  context.reset();
  context.repo = _repo;
  context.source = _source_code;

  LOG("Lexing %s\n", filename.c_str());
  lexer.lex(context.source, context.lexemes);

  LOG("Tokenizing %s\n", filename.c_str());
  for (auto& t : context.lexemes) {
    if (!t.is_gap()) {
      context.tokens.push_back(CToken(&t));
    }
  }

  LOG("Parsing %s\n", filepath.c_str());
  LOG_INDENT();
  auto tail = context.parse();
  LOG_DEDENT();

  if (tail.is_valid() && tail.is_empty() && context.root_node) {
    LOG("Parse OK\n");
  } else {
    LOG_R("could not parse %s\n", filepath.c_str());
    return ERR("Could not parse file");
  }

  repo->source_map[filename] = this;

  return Err();
}

//------------------------------------------------------------------------------

void CSourceFile::dump() {
  auto source_span = matcheroni::utils::to_span(context.source);
  matcheroni::utils::print_trees(context, source_span, 50, 2);
}

//------------------------------------------------------------------------------
