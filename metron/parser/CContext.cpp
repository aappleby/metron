// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CContext.hpp"

#include "CParser.hpp"
#include <assert.h>

using namespace matcheroni;

TokenSpan match_translation_unit(CContext& ctx, TokenSpan body);

//------------------------------------------------------------------------------

CContext::CContext() {
  type_scope = new CScope();
  tokens.reserve(65536);
}

//------------------------------------------------------------------------------

void CContext::reset() {
  NodeContext::reset();

  tokens.clear();
  while (type_scope->parent) pop_scope();
  type_scope->clear();
}

//------------------------------------------------------------------------------

TokenSpan CContext::parse(matcheroni::TextSpan text, TokenSpan lexemes) {
  this->text_span = text;
  this->lexemes = lexemes;

  for (auto t = lexemes.begin; t < lexemes.end; t++) {
    if (!t->is_gap()) {
      tokens.push_back(*t);
    }
  }

  // Skip over BOF, stop before EOF
  auto tok_a = tokens.data() + 1;
  auto tok_b = tokens.data() + tokens.size() - 1;
  TokenSpan body(tok_a, tok_b);

  return match_translation_unit(*this, body);
}

/*
bool CContext::parse(std::vector<CToken>& lexemes) {

  for (auto& t : lexemes) {
    if (!t.is_gap()) {
      tokens.push_back(t);
    }
  }

  // Skip over BOF, stop before EOF
  auto tok_a = tokens.data() + 1;
  auto tok_b = tokens.data() + tokens.size() - 1;
  TokenSpan body(tok_a, tok_b);

  auto tail = NodeTranslationUnit::match(*this, body);
  return tail.is_valid();
}
*/

//------------------------------------------------------------------------------

TokenSpan CContext::match_class_name(TokenSpan body) {
  return type_scope->has_class(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_struct_name(TokenSpan body) {
  return type_scope->has_struct(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_union_name(TokenSpan body) {
  return type_scope->has_union(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_enum_name(TokenSpan body) {
  return type_scope->has_enum(*this, body) ? body.advance(1) : body.fail();
}

TokenSpan CContext::match_typedef_name(TokenSpan body) {
  return type_scope->has_typedef(*this, body) ? body.advance(1) : body.fail();
}

void CContext::add_class  (const CToken* a) { type_scope->add_class(*this, a); }
void CContext::add_struct (const CToken* a) { type_scope->add_struct(*this, a); }
void CContext::add_union  (const CToken* a) { type_scope->add_union(*this, a); }
void CContext::add_enum   (const CToken* a) { type_scope->add_enum(*this, a); }
void CContext::add_typedef(const CToken* a) { type_scope->add_typedef(*this, a); }

//----------------------------------------------------------------------------

void CContext::push_scope() {
  CScope* new_scope = new CScope();
  new_scope->parent = type_scope;
  type_scope = new_scope;
}

void CContext::pop_scope() {
  CScope* old_scope = type_scope->parent;
  if (old_scope) {
    delete type_scope;
    type_scope = old_scope;
  }
}

//----------------------------------------------------------------------------

TokenSpan CContext::match_builtin_type_base(TokenSpan body) {
  if (!body.is_valid() || body.is_empty()) return body.fail();
  if (SST<builtin_type_base>::match(body.begin->text.begin, body.begin->text.end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

TokenSpan CContext::match_builtin_type_prefix(TokenSpan body) {
  if (!body.is_valid() || body.is_empty()) return body.fail();
  if (SST<builtin_type_prefix>::match(body.begin->text.begin, body.begin->text.end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

TokenSpan CContext::match_builtin_type_suffix(TokenSpan body) {
  if (!body.is_valid() || body.is_empty()) return body.fail();
  if (SST<builtin_type_suffix>::match(body.begin->text.begin, body.begin->text.end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

//------------------------------------------------------------------------------
