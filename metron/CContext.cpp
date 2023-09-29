// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "metron/CContext.hpp"

#include "matcheroni/Matcheroni.hpp"
#include "metrolib/core/Log.h"
#include "metron/CParser.hpp"
#include "metron/CSourceFile.hpp"
#include "metron/CSourceRepo.hpp"

#include <assert.h>

using namespace matcheroni;
using namespace matcheroni::utils;

TokenSpan match_translation_unit(CContext& ctx, TokenSpan body);

//------------------------------------------------------------------------------

CContext::CContext() {
  tokens.reserve(65536);
}

//------------------------------------------------------------------------------

void CContext::reset() {
  NodeContext::reset();

  source.clear();
  lexemes.clear();
  tokens.clear();
  global_scope.clear();
  root_node = nullptr;
}

//------------------------------------------------------------------------------

TokenSpan CContext::parse() {

  // Skip over BOF, stop before EOF
  //printf("tokens.size() %d\n", int(tokens.size()));

  TokenSpan body(tokens.data() + 1, tokens.data() + tokens.size() - 1);

  auto tail = match_translation_unit(*this, body);

  if (tail.is_valid() && tail.is_empty()) {
    if (top_head == top_tail) {
      root_node = top_head;
    }
  }

  return tail;
}

//------------------------------------------------------------------------------

TokenSpan CContext::match_class_name(TokenSpan body) {
  return top_scope->has_class(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_struct_name(TokenSpan body) {
  return top_scope->has_struct(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_union_name(TokenSpan body) {
  return top_scope->has_union(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_enum_name(TokenSpan body) {
  return top_scope->has_enum(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_typedef_name(TokenSpan body) {
  return top_scope->has_typedef(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_type(TokenSpan body) {
  return Oneof<
    Ref<&CContext::match_class_name>,
    Ref<&CContext::match_struct_name>,
    Ref<&CContext::match_union_name>,
    Ref<&CContext::match_enum_name>,
    Ref<&CContext::match_typedef_name>,
    Ref<&CContext::match_builtin_type_base>
  >::match(*this, body);
}


void CContext::add_class  (const CToken* a) { top_scope->add_class(*this, a); }
void CContext::add_struct (const CToken* a) { top_scope->add_struct(*this, a); }
void CContext::add_union  (const CToken* a) { top_scope->add_union(*this, a); }
void CContext::add_enum   (const CToken* a) { top_scope->add_enum(*this, a); }
void CContext::add_typedef(const CToken* a) { top_scope->add_typedef(*this, a); }

//----------------------------------------------------------------------------

TokenSpan CContext::match_builtin_type_base(TokenSpan body) {
  if (!body.is_valid() || body.is_empty()) return body.fail();

  if (SST<builtin_type_prefix>::match(body.begin->lex->text_begin, body.begin->lex->text_end)) {
    body = body.advance(1);
  }

  if (SST<builtin_type_base>::match(body.begin->lex->text_begin, body.begin->lex->text_end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

TokenSpan CContext::match_builtin_type_prefix(TokenSpan body) {
  if (!body.is_valid() || body.is_empty()) return body.fail();

  if (SST<builtin_type_prefix>::match(body.begin->lex->text_begin, body.begin->lex->text_end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

TokenSpan CContext::match_builtin_type_suffix(TokenSpan body) {
  if (!body.is_valid() || body.is_empty()) return body.fail();

  auto lex = body.begin->lex;
  if (SST<builtin_type_suffix>::match(lex->text_begin, lex->text_end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

//------------------------------------------------------------------------------

TokenSpan CContext::handle_preproc(TokenSpan body) {
  auto node_preproc = top_tail->as<CNodePreproc>();

  if (node_preproc->tag_is("preproc_include")) {
    std::string path_raw = node_preproc->child("path")->get_textstr();
    std::string path(path_raw.begin() + 1, path_raw.end() - 1);

    if (path.find("metron_tools.h") != std::string::npos) {
    }
    else if (path.find("stdio") != std::string::npos) {
      for (auto t : stdio_typedefs) {
        global_scope.add_typedef(t);
      }
    }
    else if (path.find("stdint") != std::string::npos) {
      for (auto t : stdint_typedefs) {
        global_scope.add_typedef(t);
      }
    }
    else if (path.find("stddef") != std::string::npos) {
      for (auto t : stddef_typedefs) {
        global_scope.add_typedef(t);
      }
    }
    else {
      CSourceFile* include_file = nullptr;
      Err err = repo->load_source(path, &include_file);

      if (!err) {
        global_scope.merge(&(include_file->context.global_scope));
      }
    }
  }
  else if (node_preproc->tag_is("preproc_define")) {
    auto name = node_preproc->child("name")->get_textstr();
    define_names.insert(name);
  }

  return body.consume();
}

//------------------------------------------------------------------------------
