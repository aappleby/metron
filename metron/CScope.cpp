// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "metron/CScope.hpp"

#include "metrolib/core/Log.h"

#include "metron/CConstants.hpp"
#include "metron/CContext.hpp"
#include "metron/CToken.hpp"
#include "metrolib/core/Log.h"
#include <string_view>

using matcheroni::TextSpan;

using std::string_view;

void CScope::merge(CScope* other) {
  class_types.insert(class_types.begin(), other->class_types.begin(), other->class_types.end());
  struct_types.insert(struct_types.begin(), other->struct_types.begin(), other->struct_types.end());
  union_types.insert(union_types.begin(), other->union_types.begin(), other->union_types.end());
  enum_types.insert(enum_types.begin(), other->enum_types.begin(), other->enum_types.end());
  typedef_types.insert(typedef_types.begin(), other->typedef_types.begin(), other->typedef_types.end());
}

void CScope::clear() {
  class_types.clear();
  struct_types.clear();
  union_types.clear();
  enum_types.clear();
  typedef_types.clear();
}

bool CScope::has_type(CContext& ctx, TokenSpan body, token_list& types) {
  if(ctx.atom_cmp(*body.begin, LEX_IDENTIFIER)) {
    return false;
  }

  auto span = body.begin->as_text_span();
  for (const auto& c : types) {
    if (strcmp_span(span, c) == 0) return true;
  }

  return false;
}

void CScope::add_type(CContext& ctx, const CToken* a, token_list& types) {
  matcheroni_assert(ctx.atom_cmp(*a, LEX_IDENTIFIER) == 0);

  auto span = a->as_text_span();

  for (const auto& c : types) {
    if (strcmp_span(span, c) == 0) return;
  }

  types.push_back(span);
}

//----------------------------------------

void CScope::add_typedef(const char* t) {
  TextSpan span = matcheroni::utils::to_span(t);

  for (const auto& c : typedef_types) {
    if (strcmp_span(span, c) == 0) return;
  }

  typedef_types.push_back(span);
}

//----------------------------------------

bool CScope::has_class  (CContext& ctx, TokenSpan body) { if (has_type(ctx, body, class_types  )) return true; if (parent2) return parent2->has_class  (ctx, body); else return false; }
bool CScope::has_struct (CContext& ctx, TokenSpan body) { if (has_type(ctx, body, struct_types )) return true; if (parent2) return parent2->has_struct (ctx, body); else return false; }
bool CScope::has_union  (CContext& ctx, TokenSpan body) { if (has_type(ctx, body, union_types  )) return true; if (parent2) return parent2->has_union  (ctx, body); else return false; }
bool CScope::has_enum   (CContext& ctx, TokenSpan body) { if (has_type(ctx, body, enum_types   )) return true; if (parent2) return parent2->has_enum   (ctx, body); else return false; }
bool CScope::has_typedef(CContext& ctx, TokenSpan body) { if (has_type(ctx, body, typedef_types)) return true; if (parent2) return parent2->has_typedef(ctx, body); else return false; }

void CScope::add_class  (CContext& ctx, const CToken* a) { return add_type(ctx, a, class_types  ); }
void CScope::add_struct (CContext& ctx, const CToken* a) { return add_type(ctx, a, struct_types ); }
void CScope::add_union  (CContext& ctx, const CToken* a) { return add_type(ctx, a, union_types  ); }
void CScope::add_enum   (CContext& ctx, const CToken* a) { return add_type(ctx, a, enum_types   ); }
void CScope::add_typedef(CContext& ctx, const CToken* a) { return add_type(ctx, a, typedef_types); }
