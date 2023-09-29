// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "metron/CParser.hpp"

#include "metron/CConstants.hpp"
#include "metron/CContext.hpp"

#include "metron/nodes/NodeTypes.hpp"

#include "metron/CSourceFile.hpp"
#include "metron/CSourceRepo.hpp"
#include "metron/CToken.hpp"

#include "matcheroni/Cookbook.hpp"
#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Parseroni.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "metron/nodes/NodeTypes.hpp"
#include "metron/SST.hpp"
#include <assert.h>

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

TokenSpan match_statement   (CContext& ctx, TokenSpan body);
TokenSpan match_targ_list   (CContext& ctx, TokenSpan body);
TokenSpan match_tdecl_list  (CContext& ctx, TokenSpan body);
TokenSpan match_decl_list   (CContext& ctx, TokenSpan body);
TokenSpan match_exp_list    (CContext& ctx, TokenSpan body);
TokenSpan match_index_list  (CContext& ctx, TokenSpan body);
TokenSpan cap_expression    (CContext& ctx, TokenSpan body);
TokenSpan match_declaration_exp (CContext& ctx, TokenSpan body);
TokenSpan match_declaration_stmt (CContext& ctx, TokenSpan body);
TokenSpan match_constructor (CContext& ctx, TokenSpan body);
TokenSpan match_function    (CContext& ctx, TokenSpan body);
TokenSpan match_struct      (CContext& ctx, TokenSpan body);
TokenSpan match_class       (CContext& ctx, TokenSpan body);
TokenSpan match_union       (CContext& ctx, TokenSpan body);
TokenSpan match_template    (CContext& ctx, TokenSpan body);
TokenSpan match_enum        (CContext& ctx, TokenSpan body);
TokenSpan match_access      (CContext& ctx, TokenSpan body);
TokenSpan match_type        (CContext& ctx, TokenSpan body);
TokenSpan match_call        (CContext& ctx, TokenSpan body);

TokenSpan cap_compound(CContext& ctx, TokenSpan body);
TokenSpan cap_type    (CContext& ctx, TokenSpan body);

using cap_enum =  CaptureAnon<Ref<match_enum>, CNodeEnum>;


using cap_targ_list    = CaptureAnon<Ref<match_targ_list>,   CNodeList>;
using cap_statement    = Ref<match_statement>;
using cap_exp_list     = CaptureAnon<Ref<match_exp_list>,    CNodeList>;

using cap_access       = CaptureAnon<Ref<match_access>,      CNodeAccess>;
using cap_call         = CaptureAnon<Ref<match_call>,        CNodeCall>;
using cap_decl_list    = CaptureAnon<Ref<match_decl_list>,   CNodeList>;
using cap_index_list   = CaptureAnon<Ref<match_index_list>,  CNodeList>;
using cap_function     = CaptureAnon<Ref<match_function>,    CNodeFunction>;
//using cap_typedef      = CaptureAnon<Ref<match_typedef>,     CNodeTypedef;
using cap_union        = CaptureAnon<Ref<match_union>,       CNodeUnion>;
using cap_constructor  = CaptureAnon<Ref<match_constructor>, CNodeConstructor>;
//using cap_declaration  = CaptureAnon<Ref<match_declaration_exp>, CNodeDeclaration>;

//------------------------------------------------------------------------------

template <StringParam match_tag, typename P>
struct TraceToken {

  inline static void print_match2(TokenSpan body, TokenSpan tail, int width) {

    if (tail.is_valid()) {
      const char* a = body.begin->text_begin();
      const char* b = tail.begin->text_begin();
      const char* c = (body.end - 1)->text_end();
      utils::print_match(a, b, c, 0x80FF80, 0xCCCCCC, width);
    } else {
      const char* a = body.begin->text_begin();
      const char* b = tail.end->text_begin();
      const char* c = (body.end - 1)->text_end();
      utils::print_match(a, b, c, 0xCCCCCC, 0x8080FF, width);
    }
  }

  static TokenSpan match(CContext& ctx, TokenSpan body) {
    matcheroni_assert(body.is_valid());
    if (body.is_empty()) return body.fail();

    auto tag = match_tag.str_val;

    int depth = ctx.trace_depth++;

    print_match2(body, body, 40);
    utils::print_trellis(depth, tag, "?", 0xCCCCCC);

    auto tail = P::match(ctx, body);
    depth = --ctx.trace_depth;

    print_match2(body, tail, 40);

    if (tail.is_valid()) {
      utils::print_trellis(depth, tag, "!", 0x80FF80);
    }
    else {
      utils::print_trellis(depth, tag, "X", 0x8080FF);
    }

    return tail;
  }
};

//------------------------------------------------------------------------------
// Matches string literals as if they were atoms. Does ___NOT___ match the
// trailing null.
// You'll need to define atom_cmp(CContext& ctx, atom& a, StringParam<N>& b) to use
// this.

template <StringParam lit>
TokenSpan match_keyword(CContext& ctx, TokenSpan body) {
  static_assert(SST<c_keywords>::contains(lit.str_val));
  if (!body.is_valid()) return body.fail();
  if (ctx.atom_cmp(*body.begin, LEX_KEYWORD) != 0) return body.fail();
  if (ctx.atom_cmp(*body.begin, lit.span()) != 0) return body.fail();
  return body.advance(1);
}

template<StringParam lit>
using cap_keyword = CaptureAnon<Ref<match_keyword<lit>>, CNodeKeyword>;

template <StringParam lit>
TokenSpan match_literal(CContext& ctx, TokenSpan body) {
  if (!body.is_valid()) return body.fail();
  if (ctx.atom_cmp(*body.begin, lit.span()) != 0) return body.fail();
  return body.advance(1);
}

//------------------------------------------------------------------------------
// This is slightly weird - as emitted from the lexer, each punctuator is a
// single token. However, to match multi-character punctuators we need N
// consecutive punctuator tokens, _and_ the punctuators can't have any
// whitespace between them.

template <StringParam lit>
inline TokenSpan match_punct(CContext& ctx, TokenSpan body) {
  if (!body.is_valid()) return body.fail();

  size_t lit_len = lit.str_len;
  const char* lit_val = lit.str_val;

  if (body.len() < lit.str_len) return body.fail();

  for (auto i = 0; i < lit.str_len; i++) {
    if (ctx.atom_cmp(body.begin[i], LEX_PUNCT) != 0) return body.fail();
  }

  auto text_begin = body.begin[0].text_begin();

  for (auto i = 0; i < lit.str_len; i++) {
    if (ctx.atom_cmp(text_begin[i], lit.str_val[i]) != 0) return body.fail();
  }

  body = body.advance(lit.str_len);

  return body;
}

//------------------------------------------------------------------------------

template<StringParam p>
using cap_punct = CaptureAnon<Ref<match_punct<p>>, CNodePunct>;

template <typename P>
using comma_separated =
Seq<
  P,
  Any<
    Seq<
      Tag<"comma", cap_punct<",">>,
      P
    >
  >,
  Opt<
    Tag<"comma", cap_punct<",">>
  >
>;

template <typename P>
using opt_comma_separated = Opt<comma_separated<P>>;

//------------------------------------------------------------------------------
// Excluding builtins and typedefs from being identifiers changes the total
// number of parse nodes, but why?

// - Because "uint8_t *x = 5" gets misparsed as an expression if uint8_t matches
// as an identifier

TokenSpan match_identifier(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Not<Ref<&CContext::match_builtin_type_base>>,
    Not<Ref<&CContext::match_typedef_name>>,
    Atom<LEX_IDENTIFIER>
  >;
  return pattern::match(ctx, body);
}

using cap_identifier = CaptureAnon<Ref<match_identifier>, CNodeIdentifier>;

TokenSpan match_field_expression(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Some<
      Seq<
        Tag<"field_path", cap_identifier>,
        cap_punct<".">
      >
    >,
    Tag<"identifier", cap_identifier>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_qualified_identifier(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Some<
      Seq<
        Tag<"scope_path", cap_identifier>,
        Tag<"colon", cap_punct<"::">>
      >
    >,
    Tag<"identifier", cap_identifier>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template<StringParam lit>
using cap_literal = CaptureAnon<Ref<match_literal<lit>>, CNodeIdentifier>;
using cap_string = CaptureAnon<Atom<LEX_STRING>, CNodeConstString>;

TokenSpan cap_preproc(CContext& ctx, TokenSpan body) {

  using pragma_pattern =
  Tag<
    "preproc_pragma",
    CaptureAnon<
      Seq<
        Tag<"pragma", cap_literal<"#pragma">>,
        Tag<"exp",    Ref<cap_expression>>
      >,
      CNodePreproc
    >
  >;

  using include_pattern =
  Tag<
    "preproc_include",
    CaptureAnon<
      Seq<
        Tag<"include", cap_literal<"#include">>,
        Tag<"path",    cap_string>
      >,
      CNodePreproc
    >
  >;

  using define_pattern =
  Tag<
    "preproc_define",
    CaptureAnon<
      Seq<
        Tag<"define",  cap_literal<"#define">>,
        Tag<"name",    cap_identifier>,
        Tag<"exp",     Opt<Ref<cap_expression>>>
      >,
      CNodePreproc
    >
  >;

  using ifndef_pattern =
  Tag<
    "preproc_ifndef",
    CaptureAnon<
      Seq<
        Tag<"ifndef",  cap_literal<"#ifndef">>,
        Tag<"name",    cap_identifier>
      >,
      CNodePreproc
    >
  >;

  using endif_pattern =
  Tag<
    "preproc_endif",
    CaptureAnon<
      Tag<"endif",  cap_literal<"#endif">>,
      CNodePreproc
    >
  >;

  using pattern =
  Dispatch<
    Oneof<
      pragma_pattern,
      include_pattern,
      define_pattern,
      ifndef_pattern,
      endif_pattern
    >,
    Ref<&CContext::handle_preproc>
  >;

  auto tail = pattern::match(ctx, body);

  return tail;
}

//------------------------------------------------------------------------------

using cap_constant =
Oneof<
  CaptureAnon<Atom<LEX_FLOAT>,                CNodeConstFloat>,
  CaptureAnon<Atom<LEX_INT>,                  CNodeConstInt>,
  CaptureAnon<Atom<LEX_CHAR>,                 CNodeConstChar>,
  CaptureAnon<Some<Atom<LEX_STRING>>,         CNodeConstString>,
  CaptureAnon<Ref<match_keyword<"nullptr">>,  CNodeKeyword>,
  CaptureAnon<Ref<match_keyword<"true">>,     CNodeKeyword>,
  CaptureAnon<Ref<match_keyword<"false">>,    CNodeKeyword>
>;

//------------------------------------------------------------------------------

template <StringParam lit>
TokenSpan match_prefix_op(CContext& ctx, TokenSpan body) {
  using pattern = Ref<match_punct<lit>>;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template <StringParam lit>
TokenSpan match_binary_op(CContext& ctx, TokenSpan body) {
  using pattern = Ref<match_punct<lit>>;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template <StringParam lit>
TokenSpan match_suffix_op(CContext& ctx, TokenSpan body) {
  using pattern = Ref<match_punct<lit>>;
  return pattern::match(ctx, body);
}

template <StringParam lit>
using suffix_op = Ref<match_suffix_op<lit>>;

//------------------------------------------------------------------------------

TokenSpan match_qualifier(CContext& ctx, TokenSpan body) {
  TextSpan span = body.begin->as_text_span();
  if (SST<qualifiers>::match(span.begin, span.end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

//------------------------------------------------------------------------------

TokenSpan match_access(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Oneof<
      Ref<match_keyword<"private">>,
      Ref<match_keyword<"protected">>,
      Ref<match_keyword<"public">>
    >,
    Atom<':'>
  >;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

using cap_cast =
CaptureAnon<
  Seq<
    Tag<"ldelim", cap_punct<"(">>,
    Tag<"type",   Ref<cap_type>>,
    Tag<"rdelim", cap_punct<")">>
  >,
  CNodeCast
>;

//------------------------------------------------------------------------------

using cap_any_identifier =
Oneof<
  CaptureAnon<Ref<match_qualified_identifier>, CNodeQualifiedIdentifier>,
  CaptureAnon<Ref<match_field_expression>,     CNodeFieldExpression>,
  cap_identifier
>;

TokenSpan match_call(CContext& ctx, TokenSpan body) {
  using pattern = Seq<
    Tag<"func_name",    cap_any_identifier>,
    Tag<"func_targs",   Opt<cap_targ_list>>,
    Tag<"func_args",    cap_exp_list>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template<StringParam p>
using cap_prefix_op = CaptureAnon<Ref<match_punct<p>>, CNodePrefixOp>;

// clang-format off
using cap_exp_prefix =
Oneof<
  cap_cast,
  cap_prefix_op<"++">,
  cap_prefix_op<"--">,
  cap_prefix_op<"+">,
  cap_prefix_op<"-">,
  cap_prefix_op<"!">,
  cap_prefix_op<"~">,
  cap_prefix_op<"*">,
  cap_prefix_op<"&">
>;
// clang-format on


//----------------------------------------

// clang-format off
using cap_exp_core =
Oneof<
  cap_call,
  cap_exp_list,
  cap_any_identifier,
  cap_constant
>;
// clang-format on

//----------------------------------------

template<StringParam p>
using cap_suffix_op = CaptureAnon<Ref<match_punct<p>>, CNodeSuffixOp>;

// clang-format off
using cap_exp_suffix =
Oneof<
  cap_index_list,
  cap_suffix_op<"++">,
  cap_suffix_op<"--">
>;
// clang-format on


//----------------------------------------

TokenSpan cap_exp_unit(CContext& ctx, TokenSpan body) {
  auto old_tail = ctx.top_tail;

  TokenSpan rest;

  // Put all the prefixes on the node list.
  rest = Any<cap_exp_prefix>::match(ctx, body);
  if (!rest) return rest;
  body = rest;

  // Match the core
  rest = cap_exp_core::match(ctx, body);
  if (!rest) return rest;
  body = rest;

  // Match suffixes and enclose them as we see them.
  while(1) {
    rest = cap_exp_suffix::match(ctx, body);
    if (!rest) break;
    body = rest;

    ctx.enclose_tail<CNodeSuffixExp>(2);
    ctx.top_tail->child_head->match_tag = "lhs";
    ctx.top_tail->child_tail->match_tag = "suffix";
  }

  // Enclose all prefixes
  while(ctx.top_tail->node_prev != old_tail) {
    ctx.enclose_tail<CNodePrefixExp>(2);
    ctx.top_tail->child_head->match_tag = "prefix";
    ctx.top_tail->child_tail->match_tag = "rhs";
  }

  return body;
}

//----------------------------------------
// FIXME need to cap the then

TokenSpan match_ternary_op(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    CaptureAnon<Ref<match_binary_op<"?">>, CNodeBinaryOp>,
    Opt<comma_separated<Ref<cap_expression>>>,
    CaptureAnon<Ref<match_binary_op<":">>, CNodeBinaryOp>
  >;
  return pattern::match(ctx, body);
}

//----------------------------------------

TokenSpan cap_binary_ops(CContext& ctx, TokenSpan body) {
  matcheroni_assert(body.is_valid());

  if (ctx.atom_cmp(*body.begin, LEX_PUNCT)) {
    return body.fail();
  }

  // clang-format off
  using pattern = Oneof<
    Ref<match_binary_op<"+">>,
    Ref<match_binary_op<"->*">>,
    Ref<match_binary_op<"->">>,
    Ref<match_binary_op<"-">>,
    Ref<match_binary_op<"*">>,
    Ref<match_binary_op<"/">>,
    Ref<match_binary_op<"==">>,
    Ref<match_binary_op<"<=>">>,
    Ref<match_binary_op<"<=">>,
    Ref<match_binary_op<"<<">>,
    Ref<match_binary_op<"<">>,
    Ref<match_binary_op<">=">>,
    Ref<match_binary_op<">>">>,
    Ref<match_binary_op<">">>,
    Ref<match_binary_op<"!=">>,
    Ref<match_binary_op<"&&">>,
    Ref<match_binary_op<"&">>,
    Ref<match_binary_op<"||">>,
    Ref<match_binary_op<"|">>,
    Ref<match_binary_op<"^">>,
    Ref<match_binary_op<"%">>,
    Ref<match_binary_op<".*">>,
    Ref<match_binary_op<".">>,
    Ref<match_ternary_op>
  >;
  // clang-format on

  return Tag<"op", CaptureAnon<pattern, CNodeBinaryOp>>::match(ctx, body);
}

TokenSpan match_assignment_ops(CContext& ctx, TokenSpan body) {
  matcheroni_assert(body.is_valid());

  if (ctx.atom_cmp(*body.begin, LEX_PUNCT)) {
    return body.fail();
  }

  // clang-format off
  using pattern = Oneof<
    Ref<match_binary_op<"+=">>,
    Ref<match_binary_op<"-=">>,
    Ref<match_binary_op<"*=">>,
    Ref<match_binary_op<"/=">>,
    Ref<match_binary_op<"=">>,
    Ref<match_binary_op<"<<=">>,
    Ref<match_binary_op<">>=">>,
    Ref<match_binary_op<"&=">>,
    Ref<match_binary_op<"|=">>,
    Ref<match_binary_op<"^=">>,
    Ref<match_binary_op<"%=">>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}


using cap_assignment_op = CaptureAnon<Ref<match_assignment_ops>, CNodeAssignOp>;

//----------------------------------------

/*
Operators that have the same precedence are bound to their arguments in the
direction of their associativity. For example, the expression a = b = c is
parsed as a = (b = c), and not as (a = b) = c because of right-to-left
associativity of assignment, but a + b - c is parsed (a + b) - c and not
a + (b - c) because of left-to-right associativity of addition and
subtraction.
*/

TokenSpan cap_expression(CContext& ctx, TokenSpan body) {
  auto old_tail = ctx.top_tail;
  TokenSpan rest;

  rest = cap_exp_unit(ctx, body);
  if (!rest) return rest;
  CNode* unit_a = ctx.top_tail;
  body = rest;

  rest = cap_binary_ops(ctx, body);
  if (!rest) return body;
  CNodeBinaryOp* op_x = ctx.top_tail->as<CNodeBinaryOp>();
  body = rest;

  rest = cap_exp_unit(ctx, body);
  if (!rest) return rest;
  CNode* unit_b = ctx.top_tail;
  body = rest;

  while(1) {
    // We have matched an expression, an operator, and an expression. We can't
    // collapse them into a single BinaryExpression though, as we don't know if
    // there's a higher-precedence operator on the right.

    // If there are no more binary operators, we're done with this phase of
    // parsing.
    rest = cap_binary_ops(ctx, body);
    if (!rest) break;
    CNodeBinaryOp* op_y = ctx.top_tail->as<CNodeBinaryOp>();
    body = rest;

    // If there's no expression after the operator, parsing fails.
    rest = cap_exp_unit(ctx, body);
    if (!rest) return rest;
    CNode* unit_c = ctx.top_tail;
    body = rest;

    // We now have three expressions separated by two operators - "a # b # c"
    // We either merge the left pair together to make (a # b) # c, or we shift
    // everything left to make "b # c _ _" and repeat the process. The
    // expression and op shifted out aren't lost, they will be merged at the
    // end.

    bool fold_left = false;
    // Fold the left side when the right operator is "lower" precedence.
    // "a * b + c" -> "(a * b) + c"
    fold_left |= op_x->precedence < op_y->precedence;

    // Fold the left side when the ops match and associate left-to-right.
    // "a + b - c" -> "(a + b) - c"
    fold_left |= op_x->precedence == op_y->precedence && op_x->assoc == 1;

    if (fold_left) {

      auto new_node = ctx.create_node<CNodeBinaryExp>();
      ctx.splice(new_node, unit_a, unit_b);

      unit_a->match_tag = "lhs";
      op_x->match_tag   = "op";
      unit_b->match_tag = "rhs";

      unit_a = new_node;
      op_x   = op_y;
      unit_b = unit_c;
      op_y   = nullptr;
      unit_c = nullptr;
    }
    else {
      // Precedence is increasing, shift nodes over and continue.
      unit_a = unit_b;
      op_x   = op_y;
      unit_b = unit_c;
    }
  }

  // Any binary operators left are in increasing-precedence order, but since
  // there are no more operators we can just fold them all up right-to-left
  while(ctx.top_tail->node_prev != old_tail) {
    auto rhs = ctx.top_tail;
    auto op  = rhs->node_prev;
    auto lhs = op->node_prev;

    lhs->match_tag = "lhs";
    op->match_tag  = "op";
    rhs->match_tag = "rhs";

    ctx.enclose_tail<CNodeBinaryExp>(3);
  }


  return body;
}

//------------------------------------------------------------------------------

TokenSpan match_decl_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"(">>,
    Tag<"decl",   CaptureAnon<Ref<match_declaration_exp>, CNodeDeclaration>>,
    cap_punct<",">,
    Tag<"rdelim", cap_punct<")">>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_exp_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"(">>,
    Tag<"exp",    Ref<cap_expression>>,
    cap_punct<",">,
    Tag<"rdelim", cap_punct<")">>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_index_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"[">>,
    Tag<"exp",    Ref<cap_expression>>,
    cap_punct<",">,
    Tag<"rdelim", cap_punct<"]">>
  >;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

const CToken* find_matching_delimiter(const CToken* begin, const CToken* end) {
  for (auto cursor = begin + 1; cursor < end; cursor++) {
    if (cursor->text_begin()[0] == '>') return cursor;
    if (cursor->text_begin()[0] == '<') cursor = find_matching_delimiter(cursor, end);
    if (!cursor) return cursor;
  }
  return nullptr;
}

TokenSpan get_template_span(TokenSpan body) {
  if (body.begin->text_begin()[0] != '<') return body.fail();
  auto ldelim = body.begin;
  auto rdelim = find_matching_delimiter(body.begin, body.end);
  if (rdelim == nullptr) return body.fail();
  return TokenSpan(ldelim + 1, rdelim);
}

TokenSpan match_targ_list(CContext& ctx, TokenSpan body) {
  auto tight_span = get_template_span(body);
  if (!tight_span.is_valid()) return body.fail();

  TokenSpan tail;

  tail = Tag<"ldelim", cap_punct<"<">>::match(ctx, body);
  if (!tail.is_valid()) return tail;

  tail = comma_separated<Tag<"arg", Ref<cap_expression>>>::match(ctx, tight_span);
  if (!tail.is_valid()) return tail;
  if (!tail.is_empty()) return body.fail();

  TokenSpan rdelim_body(tight_span.end, body.end);
  tail = Tag<"rdelim", cap_punct<">">>::match(ctx, rdelim_body);

  return tail;
};

// This is weird because we _dont_ want to include the trailing '>' of the
// template param list when we're matching the decls in it.

TokenSpan match_tdecl_list(CContext& ctx, TokenSpan body) {
  auto tail1 = Tag<"ldelim", cap_punct<"<">>::match(ctx, body);

  auto tight_span = get_template_span(body);
  if (!tight_span.is_valid()) return body.fail();

  using pattern =
  comma_separated<
    Tag<
      "decl",
      CaptureAnon<Ref<match_declaration_exp>, CNodeDeclaration>
    >
  >;

  auto tail2 = pattern::match(ctx, tight_span);

  if (!tail2.is_valid() || !tail2.is_empty()) return body.fail();

  auto tail3 = Tag<"rdelim", cap_punct<">">>::match(ctx, TokenSpan(tail2.end, body.end));

  return tail3;
};

using cap_tdecl_list = CaptureAnon<Ref<match_tdecl_list>, CNodeList>;

//------------------------------------------------------------------------------

using cap_class_name   = CaptureAnon<Ref<&CContext::match_class_name>,        CNodeIdentifier>;
using cap_struct_name  = CaptureAnon<Ref<&CContext::match_struct_name>,       CNodeIdentifier>;
using cap_union_name   = CaptureAnon<Ref<&CContext::match_union_name>,        CNodeIdentifier>;
using cap_enum_name    = CaptureAnon<Ref<&CContext::match_enum_name>,         CNodeIdentifier>;
using cap_typedef_name = CaptureAnon<Ref<&CContext::match_typedef_name>,      CNodeIdentifier>;
using cap_builtin_name = CaptureAnon<Ref<&CContext::match_builtin_type_base>, CNodeIdentifier>;

template<typename cap_name, typename node_type>
using cap_named_type =
CaptureAnon<
  Seq<
    Tag<"name", cap_name>,
    Opt<Tag<"template_args", cap_targ_list>>,

    // FIXME this is bleh
    Opt<
      Tag<
        "scope",
        CaptureAnon< Seq<cap_punct<"::">, cap_identifier>, CNode>
      >
    >,

    Any<Tag<"star", cap_punct<"*">>>
  >,
  node_type
>;

TokenSpan cap_type(CContext& ctx, TokenSpan body) {
  using pattern =
  Oneof<
    cap_named_type<cap_class_name,   CNodeClassType>,
    cap_named_type<cap_struct_name,  CNodeStructType>,
    cap_named_type<cap_union_name,   CNodeUnionType>,
    cap_named_type<cap_enum_name,    CNodeEnumType>,
    cap_named_type<cap_typedef_name, CNodeTypedefType>,
    cap_named_type<cap_builtin_name, CNodeBuiltinType>
  >;

  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_declaration_exp(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Any<
      Tag<"static", cap_keyword<"static">>,
      Tag<"const",  cap_keyword<"const">>
    >,
    Tag<"type",      Ref<cap_type>>,
    Tag<"name",      cap_identifier>,
    Any<Tag<"array", cap_index_list>>,
    Opt<Seq<
      Tag<"eq", cap_punct<"=">>,
      Tag<"value",   Ref<cap_expression>>
    >>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_declaration_stmt(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Any<
      Tag<"static", cap_keyword<"static">>,
      Tag<"const",  cap_keyword<"const">>
    >,
    Tag<"type",      Ref<cap_type>>,
    Tag<"name",      cap_identifier>,
    Any<Tag<"array", cap_index_list>>,
    Opt<Seq<
      Tag<"eq", cap_punct<"=">>,
      Tag<"value",   Ref<cap_expression>>
    >>,
    Tag<"semi", cap_punct<";">>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

using cap_field =
CaptureAnon<
  Seq<
    Tag<"decl", CaptureAnon<Ref<match_declaration_exp>, CNodeDeclaration>>,
    Tag<"semi", cap_punct<";">>
  >,
  CNodeField
>;

TokenSpan match_field_list(CContext& ctx, TokenSpan body) {
  using pattern = DelimitedBlock<
    Tag<"ldelim", cap_punct<"{">>,
    Oneof<
      cap_access,
      cap_constructor,
      cap_function,
      cap_enum,
      cap_field
    >,
    Tag<"rdelim", cap_punct<"}">>
  >;
  return pattern::match(ctx, body);
}

using cap_field_list = CaptureAnon<Ref<match_field_list>, CNodeList>;

//------------------------------------------------------------------------------

TokenSpan match_namespace(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Tag<"namespace", cap_keyword<"namespace">>,
    Tag<"name",      cap_identifier>,
    Tag<"fields",    cap_field_list>,
    Tag<"semi",      cap_punct<";">>
  >;
  return pattern::match(ctx, body);
}

using cap_namespace = CaptureAnon<Ref<match_namespace>, CNodeNamespace>;

//------------------------------------------------------------------------------

using cap_using =
CaptureAnon<
  Seq<
    Tag<"using",     cap_keyword<"using">>,
    Tag<"namespace", cap_keyword<"namespace">>,
    Tag<"name",      cap_identifier>,
    Tag<"semi",      cap_punct<";">>
  >,
  CNodeUsing
>;

//------------------------------------------------------------------------------

TokenSpan match_struct(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"struct", cap_keyword<"struct">>,
    Tag<"name",   Dispatch<cap_identifier, Ref<&CContext::add_struct2>>>,
    Tag<"body",   cap_field_list>,
    Tag<"semi",   cap_punct<";">>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_struct = CaptureAnon<Ref<match_struct>, CNodeStruct>;

//------------------------------------------------------------------------------

TokenSpan union_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = match_identifier(ctx, body);
  if (tail.is_valid()) {
    ctx.add_union(body.begin);
    return tail;
  } else {
    auto tail = ctx.match_typedef_name(body);
    if (tail.is_valid()) {
      // Already typedef'd
      return tail;
    } else {
      return body.fail();
    }
  }
}

using cap_union_type_name = CaptureAnon<Ref<union_type_adder>, CNodeIdentifier>;

TokenSpan match_union(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"union", cap_keyword<"union">>,
    Tag<"name",  cap_union_type_name>,
    Tag<"body",  cap_field_list>,
    Tag<"semi",  cap_punct<";">>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan class_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = match_identifier(ctx, body);
  if (tail.is_valid()) {
    ctx.add_class(body.begin);
    return tail;
  } else
  {
    auto tail = ctx.match_typedef_name(body);
    if (tail.is_valid()) {
      // Already typedef'd
      return tail;
    } else {
      return body.fail();
    }
  }
}

using cap_class_type_name = CaptureAnon<Ref<class_type_adder>, CNodeIdentifier>;

TokenSpan match_class(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"class", cap_keyword<"class">>,
    Tag<"name",  cap_class_type_name>,
    Tag<"body",  cap_field_list>,
    Tag<"semi",  cap_punct<";">>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_class = CaptureAnon<Ref<match_class>, CNodeClass>;

//------------------------------------------------------------------------------

using cap_template = CaptureAnon<
  Seq<
    Tag<"template", cap_keyword<"template">>,
    Tag<"params", cap_tdecl_list>,
    Tag<"class", cap_class>
  >,
  CNodeTemplate
>;

//------------------------------------------------------------------------------

TokenSpan enum_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = match_identifier(ctx, body);
  if (tail.is_valid()) {
    ctx.add_enum(body.begin);
    return tail;
  } else {
      auto tail = ctx.match_typedef_name(body);
      if (tail.is_valid()) {
      // Already typedef'd
      return tail;
    } else {
      return body.fail();
    }
  }
}

using cap_enum_type_name = CaptureAnon<Ref<enum_type_adder>, CNodeIdentifier>;

TokenSpan match_enumerator(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Tag<"name",    cap_identifier>,
    Opt<Seq<
      Tag<"eq",    cap_punct<"=">>,
      Tag<"value", Ref<cap_expression>>
    >>
  >;
  return pattern::match(ctx, body);
}

using cap_enumerator_name = CaptureAnon<Ref<match_enumerator>, CNodeEnumerator>;

TokenSpan match_enumerator_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"{">>,
    Tag<"enumerator", cap_enumerator_name>,
    Tag<"comma", cap_punct<",">>,
    Tag<"rdelim", cap_punct<"}">>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enum(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Tag<"enum", cap_keyword<"enum">>,
    Opt<
      Tag<"class", cap_keyword<"class">>
    >,
    Opt<
      Tag<"name", cap_enum_type_name>
    >,
    Opt<Seq<
      Tag<"colon", cap_punct<":">>,
      Tag<"base_type", Ref<cap_type>>
    >>,
    Tag<"body", CaptureAnon<Ref<match_enumerator_list>, CNodeList>>,
    Opt<comma_separated<Tag<"decl", Ref<cap_expression>>>>,
    Tag<"semi", cap_punct<";">>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_function(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Opt<Tag<"static",  cap_keyword<"static">>>,
    Tag<"return_type", Ref<cap_type>>,
    Tag<"name",        cap_identifier>,
    Tag<"params",      cap_decl_list>,
    Opt<Tag<"const",   cap_keyword<"const">>>,
    Tag<"body",        Ref<cap_compound>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_constructor(CContext& ctx, TokenSpan body) {
  using initializers = Seq<
    cap_punct<":">,
    Tag<
      "init",
      CaptureAnon<
        comma_separated<
          Tag<"item",
            CaptureAnon<
              Seq<
                Tag<"name", cap_identifier>,
                Tag<"value", cap_exp_list>
              >,
              CNode
            >
          >
        >,
        CNodeList
      >
    >
  >;

  using cap_class_name = CaptureAnon<Ref<&CContext::match_class_name>, CNodeIdentifier>;


  using pattern =
  Seq<
    Tag<"name",   cap_class_name>,
    Tag<"params", cap_decl_list>,
    Opt<initializers>,
    //Opt<Tag<"init", initializers>>,
    Tag<"body",   Ref<cap_compound>>
  >;
  // clang-format om
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan cap_compound(CContext& ctx, TokenSpan body) {

  auto old_tail = ctx.top_tail;

  auto span = body;

  span = Tag<"ldelim", cap_punct<"{">>::match(ctx, span);
  if (!span) return span;

  auto new_node = ctx.create_node<CNodeCompound>();
  ctx.push_scope2(&new_node->scope);

  span = Any<Ref<match_statement>>::match(ctx, span);
  if (!span) {
    ctx.pop_scope2();
    return span;
  }

  span = Tag<"rdelim", cap_punct<"}">>::match(ctx, span);
  if (!span) {
    ctx.pop_scope2();
    return span;
  }

  if (span) {
    ctx.merge_node(new_node, old_tail);
    TokenSpan node_span = {body.begin, span.begin};
    new_node->init(nullptr, node_span, 0);
  }

  ctx.pop_scope2();
  return span;
}

//------------------------------------------------------------------------------
// FIXME we should cap the condition/step differently or something

TokenSpan cap_assignment(CContext& ctx, TokenSpan body);

TokenSpan match_for(CContext& ctx, TokenSpan body) {
  // clang-format off

  using exp_or_decl =
  Oneof<
    Ref<cap_assignment>,
    Ref<cap_expression>,
    CaptureAnon<Ref<match_declaration_exp>, CNodeDeclaration>
  >;

  using pattern =
  Seq<
    Tag<"for",       cap_keyword<"for">>,
    Tag<"ldelim",    cap_punct<"(">>,
    Tag<"init",      Opt<exp_or_decl>>,
    cap_punct<";">,
    Tag<"condition", Opt<Ref<cap_expression>>>,
    cap_punct<";">,
    Tag<"step",      Opt<Ref<cap_expression>>>,
    Tag<"rdelim",    cap_punct<")">>,
    Tag<"body",      cap_statement>
  >;
  // clang-format on
  return pattern::match(ctx, body);
};

using cap_for = CaptureAnon<Ref<match_for>, CNodeFor>;

//------------------------------------------------------------------------------

TokenSpan match_if(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Tag<"if",        cap_keyword<"if">>,
    Tag<"condition", cap_exp_list>,
    Tag<"body_true", cap_statement>,
    Opt<Seq<
      Tag<"else",       cap_keyword<"else">>,
      Tag<"body_false", cap_statement>
    >>
  >;
  return pattern::match(ctx, body);
}

using cap_if = CaptureAnon<Ref<match_if>, CNodeIf>;

//------------------------------------------------------------------------------

TokenSpan match_return(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Tag<"return", cap_keyword<"return">>,
    Opt<Tag<"value", Ref<cap_expression>>>,
    Tag<"semi", cap_punct<";">>
  >;
  return pattern::match(ctx, body);
}

using cap_return = CaptureAnon<Ref<match_return>, CNodeReturn>;

//------------------------------------------------------------------------------

TokenSpan match_case_body(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Tag<
    "body",
    CaptureAnon<
      Some<
        Seq<
          Not<Ref<match_keyword<"case">>>,
          Not<Ref<match_keyword<"default">>>,
          Ref<match_statement>
        >
      >,
      CNodeList
    >
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

TokenSpan match_case(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"case",      cap_keyword<"case">>,
    Tag<"condition", Ref<cap_expression>>,
    Tag<"colon",     cap_punct<":">>,
    Opt<Ref<match_case_body>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_case = CaptureAnon<Ref<match_case>, CNodeCase>;

TokenSpan match_default(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"default", cap_keyword<"default">>,
    Tag<"colon",   cap_punct<":">>,
    Opt<Ref<match_case_body>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_default = CaptureAnon<Ref<match_default>, CNodeDefault>;

//------------------------------------------------------------------------------

TokenSpan match_switch(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"switch",     cap_keyword<"switch">>,
    Tag<"condition",  Ref<cap_expression>>,
    Tag<"body",
      CaptureAnon<
        Seq<
          Tag<"ldelim",     cap_punct<"{">>,
          Any<
            Tag<"case", cap_case>,
            Tag<"default", cap_default>
          >,
          Tag<"rdelim",     cap_punct<"}">>
        >,
        CNodeList
      >
    >
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_switch  = CaptureAnon<Ref<match_switch>, CNodeSwitch>;

//------------------------------------------------------------------------------

TokenSpan match_while(CContext& ctx, TokenSpan body) {
  // clang-format on
  using pattern =
  Seq<
    Tag<"while", cap_keyword<"while">>,
    Tag<"cond",  cap_exp_list>,
    Tag<"body",  cap_statement>
  >;
  // clang-format off
  return pattern::match(ctx, body);
}

using cap_while = CaptureAnon<Ref<match_while>, CNodeWhile>;

//------------------------------------------------------------------------------

TokenSpan match_do_while(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"do",    cap_keyword<"do">>,
    Tag<"body",  cap_statement>,
    Tag<"while", cap_keyword<"while">>,
    Tag<"cond",  cap_exp_list>,
    Tag<"semi",  cap_punct<";">>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_do_while = CaptureAnon<Ref<match_do_while>, CNodeDoWhile>;

//------------------------------------------------------------------------------

TokenSpan match_continue(CContext& ctx, TokenSpan body) {
  using pattern = Ref<match_keyword<"continue">>;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

static void extract_declarator(CContext& ctx, CNode* decl) {
  if (auto name = decl->child("name")) {
    if (auto id = name->child("identifier")) {
      ctx.add_typedef(id->span.begin);
    }
  }
}

static void extract_declarator_list(CContext& ctx, CNode* decls) {
  if (!decls) return;
  for (auto child = decls->child_head; child; child = child->node_next) {

    if (child->tag_is("decl")) {
      extract_declarator(ctx, child);
    }
    else {
      matcheroni_assert(false);
    }
  }
}

static void extract_type(CContext& ctx) {
  auto node = ctx.top_tail;

  if (auto child = node->child("union")) {
    extract_declarator_list(ctx, child->child("decls"));
    return;
  }

  if (auto child = node->child("struct")) {
    extract_declarator_list(ctx, child->child("decls"));
    return;
  }

  if (auto child = node->child("class")) {
    extract_declarator_list(ctx, child->child("decls"));
    return;
  }

  if (auto child = node->child("enum")) {
    extract_declarator_list(ctx, child->child("decls"));
    return;
  }

  if (auto child = node->child("decl")) {
    extract_declarator_list(ctx, child->child("decls"));
    return;
  }

  matcheroni_assert(false);
}

// FIXME this is probably broken since we took the decls off CNodeStruct::match and stuff

#if 0

TokenSpan match_typedef(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"typedef">>,
    Tag<
      "typedef",
      CaptureAnon<
        Oneof<
          Cap2<"struct", CNodeStruct>,
          Cap2<"union",  CNodeUnion>,
          Cap2<"class",  CNodeClass>,
          Cap2<"enum",   CNodeEnum>,
          Cap2<"decl",   CNodeDeclaration>
        >,
        CNode
      >
    >
  >;
  // clang-format on

  auto tail = pattern::match(ctx, body);
  if (tail.is_valid()) {
    //print_context(ctx.text_span, ctx, 40);
    extract_type(ctx);
  }

  return tail;
};

#endif

//------------------------------------------------------------------------------

TokenSpan cap_assignment(CContext& ctx, TokenSpan body) {

  using lvalue =
  CaptureAnon<
    Seq<
      Tag<"name",       cap_any_identifier>,
      Opt<Tag<"suffix", cap_index_list>>
    >,
    CNodeLValue
  >;

  using pattern =
  CaptureAnon<
    Seq<
      Tag<"lhs",  lvalue>,
      Tag<"op",   cap_assignment_op>,
      Tag<"rhs",  Ref<cap_expression>>
    >,
    CNodeAssignment
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template<typename P>
using statement_wrapper =
CaptureAnon<
  Seq<
    Tag<"exp",  P>,
    Tag<"semi", cap_punct<";">>
  >,
  CNodeExpStatement
>;

//----------

TokenSpan match_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Oneof<
    cap_for,
    cap_if,
    cap_switch,
    cap_while,
    Ref<cap_compound>,
    cap_do_while,
    cap_return,
    cap_using,

    statement_wrapper<cap_keyword<"break">>,
    statement_wrapper<cap_keyword<"continue">>,
    statement_wrapper<Ref<cap_assignment>>,
    statement_wrapper<Ref<cap_expression>>,
    statement_wrapper<CaptureAnon<Ref<match_declaration_exp>, CNodeDeclaration>>

    // FIXME disallowing empty statements for now
    //,
    //cap_punct<";">
  >;
  // clang-format on
  auto result = pattern::match(ctx, body);
  if (result.is_valid()) {
    assert(ctx.top_tail->as<CNodeStatement>());
  }
  return result;
}

//------------------------------------------------------------------------------

TokenSpan match_translation_unit(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Any<
    Ref<cap_preproc>,
    cap_function,
    cap_namespace,
    cap_class,
    cap_struct,
    cap_union,
    cap_enum,
    cap_template,
    cap_using,
    statement_wrapper<CaptureAnon<Ref<match_declaration_exp>, CNodeDeclaration>>

    //cap_typedef FIXME
  >;
  // clang-format on

  return CaptureAnon<pattern, CNodeTranslationUnit>::match(ctx, body);
}

//------------------------------------------------------------------------------
