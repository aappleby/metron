#include "CSourceFile.hpp"

#include "CSourceRepo.hpp"

#include "metrolib/core/Log.h"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

[[nodiscard]] Err CSourceFile::init(CSourceRepo* _repo,
                                    const std::string& _filename,
                                    const std::string& _filepath,
                                    const std::string& _source_code,
                                    bool _use_utf8_bom) {
  this->repo = _repo;
  this->filename = _filename;
  this->filepath = _filepath;
  this->source_code = _source_code;
  this->use_utf8_bom = _use_utf8_bom;

  context.repo = _repo;

  LOG("Lexing %s\n", filename.c_str());
  auto source_span = matcheroni::utils::to_span(source_code);
  lexer.lex(source_span);

  LOG("Parsing %s\n", filename.c_str());
  TokenSpan tok_span(lexer.tokens.data(),
                     lexer.tokens.data() + lexer.tokens.size());
  auto tail = context.parse(source_span, tok_span);

  //matcheroni::utils::print_trees(context, source_span, 50);

  if (!tail.is_valid() || !tail.is_empty()) {
    LOG_R("could not parse %s\n", filepath.c_str());
    return ERR("Could not parse file");
  }

  repo->source_map[filename] = this;

  return Err();
}

//------------------------------------------------------------------------------
