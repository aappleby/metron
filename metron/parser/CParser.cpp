// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CParser.hpp"

#include "CConstants.hpp"
#include "CContext.hpp"
#include "CNode.hpp"
#include "CNodeClass.hpp"
#include "CNodeConstructor.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStruct.hpp"
#include "CNodeCall.hpp"
#include "CNodeType.hpp"
#include "CNodeExpression.hpp"
#include "CSourceFile.hpp"
#include "CSourceRepo.hpp"
#include "CToken.hpp"
#include "matcheroni/Cookbook.hpp"
#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Parseroni.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "NodeTypes.hpp"
#include "SST.hpp"
#include <assert.h>

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

TokenSpan match_any_statement (CContext& ctx, TokenSpan body);
TokenSpan match_texp_list     (CContext& ctx, TokenSpan body);
TokenSpan match_tdecl_list    (CContext& ctx, TokenSpan body);
TokenSpan match_decl_list     (CContext& ctx, TokenSpan body);
TokenSpan match_exp_list      (CContext& ctx, TokenSpan body);
TokenSpan match_index_list    (CContext& ctx, TokenSpan body);
TokenSpan match_expression    (CContext& ctx, TokenSpan body);
TokenSpan match_declaration   (CContext& ctx, TokenSpan body);
TokenSpan match_constructor   (CContext& ctx, TokenSpan body);
TokenSpan match_function      (CContext& ctx, TokenSpan body);
TokenSpan match_struct        (CContext& ctx, TokenSpan body);
TokenSpan match_class         (CContext& ctx, TokenSpan body);
TokenSpan match_compound      (CContext& ctx, TokenSpan body);
TokenSpan match_union         (CContext& ctx, TokenSpan body);
TokenSpan match_template      (CContext& ctx, TokenSpan body);
TokenSpan match_enum          (CContext& ctx, TokenSpan body);

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

template <StringParam match_tag, typename pattern, typename node_type>
//using Cap = TraceToken<match_tag, Capture<match_tag, pattern, node_type>>;
using Cap = Capture<match_tag, pattern, node_type>;

//------------------------------------------------------------------------------

template <typename P>
using comma_separated = Seq<P, Any<Seq<Atom<','>, P>>, Opt<Atom<','>>>;

template <typename P>
using opt_comma_separated = Opt<comma_separated<P>>;

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

template <StringParam lit>
TokenSpan match_literal(CContext& ctx, TokenSpan body) {
  if (!body.is_valid()) return body.fail();
  if (ctx.atom_cmp(*body.begin, lit.span()) != 0) return body.fail();
  return body.advance(1);
}

//------------------------------------------------------------------------------

template <StringParam lit>
inline TokenSpan match_punct(CContext& ctx, TokenSpan body) {
  if (!body.is_valid()) return body.fail();

  size_t lit_len = lit.str_len;
  const char* lit_val = lit.str_val;

  if (body.len() < lit.str_len) return body.fail();

  for (auto i = 0; i < lit.str_len; i++) {
    const CToken& tok_a = body.begin[0];
    if (ctx.atom_cmp(tok_a, LEX_PUNCT) != 0) return body.fail();
    if (ctx.atom_cmp(tok_a.text_begin()[0], lit.str_val[i]) != 0) return body.fail();
    body = body.advance(1);
  }

  return body;
}

//------------------------------------------------------------------------------
// Our builtin types are any sequence of prefixes followed by a builtin type

TokenSpan match_builtin_type(CContext& ctx, TokenSpan body) {
  /*
  using match_prefix = Ref<&CContext::match_builtin_type_prefix>;
  using match_base   = Ref<&CContext::match_builtin_type_base>;
  using match_suffix = Ref<&CContext::match_builtin_type_suffix>;

  // clang-format off
  using pattern =
  Seq<
    Any<
      Seq<
        Cap<"prefix", match_prefix, CNodeText>,
        And<match_base>
      >
    >,
    Cap<"base_type", match_base, CNodeTypeName>,
    Opt<Cap<"suffix", match_suffix, CNodeText>>
  >;

  auto tail = pattern::match(ctx, body);
  return tail;
  */

  return ctx.match_builtin_type_base(body);
}

//------------------------------------------------------------------------------
// Excluding builtins and typedefs from being identifiers changes the total
// number of parse nodes, but why?

// - Because "uint8_t *x = 5" gets misparsed as an expression if uint8_t matches
// as an identifier

TokenSpan match_identifier(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Not<Ref<match_builtin_type>>,
    Not<Ref<&CContext::match_typedef_name>>,
    Atom<LEX_IDENTIFIER>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_field_expression(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Some<
      Seq<
        Cap<"field_path", Atom<LEX_IDENTIFIER>, CNodeIdentifier>,
        Ref<match_punct<".">>
      >
    >,
    Cap<"identifier", Atom<LEX_IDENTIFIER>, CNodeIdentifier>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_qualified_identifier(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Some<
      Seq<
        Cap<"scope_path", Atom<LEX_IDENTIFIER>, CNodeIdentifier>,
        Ref<match_punct<"::">>
      >
    >,
    Cap<"identifier", Atom<LEX_IDENTIFIER>, CNodeIdentifier>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TextSpan CContext::handle_include(TextSpan body) {
  if (body.begin[0] == '<') return body.fail();

  std::string path(body.begin + 1, body.end - 1);

  if (path.find("metron_tools.h") != std::string::npos) {
    return body.fail();
  }

  CSourceFile* include_file = nullptr;
  Err err = repo->load_source(path, &include_file);

  if (!err) {
    type_scope->merge(include_file->context.type_scope);
  }

  return body.consume();
}

//------------------------------------------------------------------------------

TokenSpan match_preproc(CContext& ctx, TokenSpan body) {
  if (body.begin->lex_type() != LEX_PREPROC) return body.fail();

  std::string s(body.begin->text_begin(), body.begin->text_end());

  if (s.find("stdio") != std::string::npos) {
    for (auto t : stdio_typedefs) {
      ctx.type_scope->add_typedef(t);
    }
    return body.advance(1);
  }

  if (s.find("stdint") != std::string::npos) {
    for (auto t : stdint_typedefs) {
      ctx.type_scope->add_typedef(t);
    }
    return body.advance(1);
  }

  if (s.find("stddef") != std::string::npos) {
    for (auto t : stddef_typedefs) {
      ctx.type_scope->add_typedef(t);
    }
    return body.advance(1);
  }

  using namespace cookbook;

  {
    c_include_line<Ref<&CContext::handle_include>>::match(ctx, body.begin->as_text_span());
  }

  return body.advance(1);
};

//------------------------------------------------------------------------------

TokenSpan match_constant(CContext& ctx, TokenSpan body) {
  using pattern =
  Oneof<
    Cap<"float",   Atom<LEX_FLOAT>,               CNodeConstant>,
    Cap<"int",     Atom<LEX_INT>,                 CNodeConstant>,
    Cap<"char",    Atom<LEX_CHAR>,                CNodeConstant>,
    Cap<"string",  Some<Atom<LEX_STRING>>,        CNodeConstant>,
    Cap<"nullptr", Ref<match_literal<"nullptr">>, CNodeConstant>,
    Cap<"true",    Ref<match_literal<"true">>,    CNodeConstant>,
    Cap<"false",   Ref<match_literal<"false">>,   CNodeConstant>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template <StringParam lit>
TokenSpan match_prefix_op(CContext& ctx, TokenSpan body) {
  /*
  NodePrefixOp() {
    precedence = prefix_precedence(lit.str_val);
    assoc = prefix_assoc(lit.str_val);
  }
  */

  using pattern = Ref<match_punct<lit>>;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template <StringParam lit>
TokenSpan match_binary_op(CContext& ctx, TokenSpan body) {
  /*
  NodeBinaryOp() {
    precedence = binary_precedence(lit.str_val);
    assoc = binary_assoc(lit.str_val);
  }
  */

  using pattern = Ref<match_punct<lit>>;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template <StringParam lit>
TokenSpan match_suffix_op(CContext& ctx, TokenSpan body) {
  /*
  NodeSuffixOp() {
    precedence = suffix_precedence(lit.str_val);
    assoc = suffix_assoc(lit.str_val);
  }
  */

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

TokenSpan match_type_name(CContext& ctx, TokenSpan body);

TokenSpan match_prefix_cast(CContext& ctx, TokenSpan body) {
  /*
  NodePrefixCast() {
    precedence = 3;
    assoc = -2;
  }
  */

  using pattern = Seq<Atom<'('>, Ref<match_type_name>, Atom<')'>>;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

// FIXME basic_tock_with_return isn't rewinding after matching call_name

TokenSpan match_call(CContext& ctx, TokenSpan body) {
  using pattern = Seq<
    Oneof<
      Cap<"func_name",    Ref<match_qualified_identifier>, CNodeQualifiedIdentifier>,
      Cap<"func_name",    Ref<match_field_expression>,     CNodeFieldExpression>,
      Cap<"func_name",    Ref<match_identifier>,           CNodeIdentifier>
    >,
    Cap<"func_targs",     Opt<Ref<match_texp_list>>,       CNodeList>,
    Cap<"func_args",      Ref<match_exp_list>,             CNodeList>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

// clang-format off
using cap_exp_prefix =
Oneof<
  CaptureAnon<Ref<match_prefix_cast>,     CNodeCast>,
  CaptureAnon<Ref<match_prefix_op<"++">>, CNodePunct>,
  CaptureAnon<Ref<match_prefix_op<"--">>, CNodePunct>,
  CaptureAnon<Ref<match_prefix_op<"+">>,  CNodePunct>,
  CaptureAnon<Ref<match_prefix_op<"-">>,  CNodePunct>,
  CaptureAnon<Ref<match_prefix_op<"!">>,  CNodePunct>,
  CaptureAnon<Ref<match_prefix_op<"~">>,  CNodePunct>,
  CaptureAnon<Ref<match_prefix_op<"*">>,  CNodePunct>,
  CaptureAnon<Ref<match_prefix_op<"&">>,  CNodePunct>
>;
// clang-format on


//----------------------------------------

// clang-format off
using cap_exp_core =
Oneof<
  CaptureAnon<Ref<match_call>,                 CNodeCall>,
  CaptureAnon<Ref<match_exp_list>,             CNodeList>,
  CaptureAnon<Ref<match_qualified_identifier>, CNodeQualifiedIdentifier>,
  CaptureAnon<Ref<match_field_expression>,     CNodeFieldExpression>,
  CaptureAnon<Ref<match_identifier>,           CNodeIdentifier>,
  CaptureAnon<Ref<match_constant>,             CNodeConstant>
>;
// clang-format on

//----------------------------------------

// clang-format off
using cap_exp_suffix =
Oneof<
  CaptureAnon<Ref<match_exp_list>,        CNodeList>,
  CaptureAnon<Ref<match_index_list>,      CNodeList>,
  CaptureAnon<Ref<match_suffix_op<"++">>, CNodePunct>,
  CaptureAnon<Ref<match_suffix_op<"--">>, CNodePunct>
>;
// clang-format on


//----------------------------------------

// clang-format off
using cap_exp_unit =
CaptureAnon<
  Seq<
    Any<Label<"prefix", cap_exp_prefix>>,
        Label<"core",   cap_exp_core>,
    Any<Label<"suffix", cap_exp_suffix>>
  >,
  CNodeUnit
>;
// clang-format on

//----------------------------------------
// FIXME need to cap the then

TokenSpan match_ternary_op(CContext& ctx, TokenSpan body) {
  /*
  NodeTernaryOp() {
    precedence = 16;
    assoc = -1;
  }
  */

  using pattern =
  Seq<
    Ref<match_binary_op<"?">>,
    Opt<
      Cap<
        "then",
        comma_separated<Ref<match_expression>>,
        CNode
      >
    >,
    Ref<match_binary_op<":">>
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

  return CaptureAnon<pattern, CNodeOperator>::match(ctx, body);
}

TokenSpan cap_assignment_ops(CContext& ctx, TokenSpan body) {
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

  return CaptureAnon<pattern, CNodeOperator>::match(ctx, body);
}

//----------------------------------------

/*
Operators that have the same precedence are bound to their arguments in the
direction of their associativity. For example, the expression a = b = c is
parsed as a = (b = c), and not as (a = b) = c because of right-to-left
associativity of assignment, but a + b - c is parsed (a + b) - c and not
a + (b - c) because of left-to-right associativity of addition and
subtraction.
*/

TokenSpan match_expression(CContext& ctx, TokenSpan body) {

  using pattern =
  Seq<
    Label<"unit",   cap_exp_unit>,
    Any<Seq<
      Label<"op",   Ref<cap_binary_ops>>,
      Label<"unit", cap_exp_unit>
    >>
  >;

  auto tail = pattern::match(ctx, body);
  if (!tail.is_valid()) {
    return body.fail();
  }

  //auto tok_a = body.begin;
  //auto tok_b = tail.begin;

  /*
  while (0) {
    {
      auto c = tok_a;
      while (c && c < tok_b) {
        c = c->step_right();
      }
      printf("\n");
    }

    auto c = tok_a;
    while (c && c->span->precedence && c < tok_b) {
      c = c->step_right();
    }

    // ran out of units?
    if (c->span->precedence) break;

    auto l = c - 1;
    if (l && l >= tok_a) {
      if (l->span->assoc == -2) {
        auto node = new CNodeExpressionPrefix();
        node->init_node(ctx, l, c, l->span, c->span);
        continue;
      }
    }

    break;
  }
  */

  // Fold up as many nodes based on precedence as we can
#if 0
  while(1) {
    ParseNode*    na = nullptr;
    NodeOpBinary* ox = nullptr;
    ParseNode*    nb = nullptr;
    NodeOpBinary* oy = nullptr;
    ParseNode*    nc = nullptr;

    nc = (cursor - 1)->span->as_a<ParseNode>();
    oy = nc ? nc->left_neighbor()->as_a<NodeOpBinary>()   : nullptr;
    nb = oy ? oy->left_neighbor()->as_a<ParseNode>() : nullptr;
    ox = nb ? nb->left_neighbor()->as_a<NodeOpBinary>()   : nullptr;
    na = ox ? ox->left_neighbor()->as_a<ParseNode>() : nullptr;




    if (!na || !ox || !nb || !oy || !nc) break;

    if (na->tok_b() < a) break;

    if (ox->precedence < oy->precedence) {
      // Left-associate because right operator is "lower" precedence.
      // "a * b + c" -> "(a * b) + c"
      auto node = new CNodeExpressionBinary();
      node->init(na->tok_a(), nb->tok_b());
    }
    else if (ox->precedence == oy->precedence) {
      matcheroni_assert(ox->assoc == oy->assoc);

      if (ox->assoc == 1) {
        // Left to right
        // "a + b - c" -> "(a + b) - c"
        auto node = new CNodeExpressionBinary();
        node->init(na->tok_a(), nb->tok_b());
      }
      else if (ox->assoc == -1) {
        // Right to left
        // "a = b = c" -> "a = (b = c)"
        auto node = new CNodeExpressionBinary();
        node->init(nb->tok_a(), nc->tok_b());
      }
      else {
        matcheroni_assert(false);
      }
    }
    else {
      break;
    }
  }
#endif

  // Any binary operators left on the tokens are in increasing-precedence
  // order, but since there are no more operators we can just fold them up
  // right-to-left
#if 0
  while(1) {
    ParseNode*    nb = nullptr;
    NodeOpBinary* oy = nullptr;
    ParseNode*    nc = nullptr;

    nc = (cursor - 1)->span->as_a<ParseNode>();
    oy = nc ? nc->left_neighbor()->as_a<NodeOpBinary>()   : nullptr;
    nb = oy ? oy->left_neighbor()->as_a<ParseNode>() : nullptr;

    if (!nb || !oy || !nc) break;
    if (nb->tok_b() < a) break;

    auto node = new CNodeExpressionBinary();
    node->init(nb->tok_a(), nc->tok_b());
  }
#endif

#if 0
  if (auto tail = SpanTernaryOp::match(ctx, cursor, b)) {
    auto node = new CNodeExpressionTernary();
    node->init(a, tail - 1);
    cursor = tail;
  }
#endif

#if 0
  {
    const CToken* c = a;
    while(1) {
      if (auto tail = c->span->tok_b()) {
        c = tail + 1;
      }
      else {
        c++;
      }
      if (c == tok_b) break;
    }
  }
#endif

  return tail;
}

using cap_expression = CaptureAnon<Ref<match_expression>, CNodeExpression>;

//------------------------------------------------------------------------------

/*
TokenSpan match_lvalue(CContext& ctx, TokenSpan body) {
  using pattern = Oneof<
    Ref<match_qualified_identifier>, CNodeQualifiedIdentifier>,
    Ref<match_field_expression>,     CNodeFieldExpression>,
    Ref<match_identifier>,           CNodeIdentifier>
  >;


}

TokenSpan match_assignment(CContext& ctx, TokenSpan body) {
  using pattern = Seq<
}
*/

//------------------------------------------------------------------------------

template<auto c>
using cap_punct = CaptureAnon<Atom<c>, CNodePunct>;

TokenSpan match_decl_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Cap<"decl", Ref<match_declaration>, CNodeDeclaration>,
    Atom<','>,
    Atom<')'>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_exp_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Label<"exp", cap_expression>,
    Atom<','>,
    Atom<')'>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_index_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Label<"ldelim", cap_punct<'['>>,
    Label<"exp",    cap_expression>,
    Label<"comma",  cap_punct<','>>,
    Label<"rdelim", cap_punct<']'>>
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

/*
  DelimitedList<
    Cap<"ldelim",     Atom<'{'>,             CNodePunct>,
    Cap<"enumerator", Ref<match_enumerator>, CNodeText>,
    Cap<"comma",      Atom<','>,             CNodePunct>,
    Cap<"rdelim",     Atom<'}'>,             CNodePunct>
  >;
*/

TokenSpan match_texp_list(CContext& ctx, TokenSpan body) {
  auto tight_span = get_template_span(body);
  if (!tight_span.is_valid()) return body.fail();

  TokenSpan tail;

  tail = Cap<"ldelim", Atom<'<'>, CNodePunct>::match(ctx, body);
  if (!tail.is_valid()) return tail;

  tail = comma_separated<Cap<"exp", Ref<match_expression>, CNodeExpression>>::match(ctx, tight_span);
  if (!tail.is_valid()) return tail;
  if (!tail.is_empty()) return body.fail();

  TokenSpan rdelim_body(tight_span.end, body.end);
  tail = Cap<"rdelim", Atom<'>'>, CNodePunct>::match(ctx, rdelim_body);

  return tail;
};

TokenSpan match_tdecl_list(CContext& ctx, TokenSpan body) {
  auto tight_span = get_template_span(body);
  if (!tight_span.is_valid()) return body.fail();

  using pattern =
  comma_separated<
    Cap<"decl", Ref<match_declaration>, CNodeDeclaration>
  >;
  auto tail = pattern::match(ctx, tight_span);

  if (tail.is_valid() && tail.is_empty()) {
    return TokenSpan(tight_span.end + 1, body.end);
  }
  else {
    return body.fail();
  }
};

//------------------------------------------------------------------------------

TokenSpan match_type(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    //Cap<
    //  "type",
      Oneof<
        // These have to be NodeIdentifier because "void foo(struct S);"
        // is valid even without the definition of S.
        //Seq<Ref<match_keyword<"class">>,  Oneof<CNodeIdentifier, Ref<&CContext::match_class_name>>>,
        //Seq<Ref<match_keyword<"union">>,  Oneof<CNodeIdentifier, Ref<&CContext::match_union_name>>>,
        //Seq<Ref<match_keyword<"struct">>, Oneof<CNodeIdentifier, Ref<&CContext::match_struct_name>>>,
        //Seq<Ref<match_keyword<"enum">>,   Oneof<CNodeIdentifier, Ref<&CContext::match_enum_name>>>,

        Cap<"class_name",   Ref<&CContext::match_class_name>,   CNodeTypeName>,
        Cap<"struct_name",  Ref<&CContext::match_struct_name>,  CNodeTypeName>,
        Cap<"union_name",   Ref<&CContext::match_union_name>,   CNodeTypeName>,
        Cap<"enum_name",    Ref<&CContext::match_enum_name>,    CNodeTypeName>,
        Cap<"typedef_name", Ref<&CContext::match_typedef_name>, CNodeTypeName>,

        Cap<"builtin_name", Ref<match_builtin_type>,            CNodeTypeName>
      >,
    //  CNode
    //>,
    Opt<Cap<"type_args", Ref<match_texp_list>, CNodeList>>,
    Any<Cap<"star",  Atom<'*'>,                CNodeText>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_declaration(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Any<
      Cap<"static",       Ref<match_keyword<"static">>, CNodeKeyword>,
      Cap<"const",        Ref<match_keyword<"const">>,  CNodeKeyword>
    >,

    //Cap<"decl_type",      Ref<match_type>,              CNodeType>,
    Label<"decl_type", CaptureAnon<Ref<match_type>, CNodeType>>,

    Cap<"decl_name",      Ref<match_identifier>,        CNodeIdentifier>,
    Any<Cap<"decl_array", Ref<match_index_list>,        CNodeList>>,
    Opt<Seq<
      Cap<"eq",           Atom<'='>,                    CNodePunct>,
      Cap<"decl_value",   Ref<match_expression>,        CNodeExpression>
    >>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_type_name(CContext& ctx, TokenSpan body) {
  using pattern = Oneof<
    Ref<&CContext::match_class_name>,
    Ref<&CContext::match_struct_name>,
    Ref<&CContext::match_union_name>,
    Ref<&CContext::match_enum_name>,
    Ref<&CContext::match_typedef_name>,
    Ref<match_builtin_type>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_field(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Oneof<
    Cap<"semi",        Atom<';'>,               CNodePunct>,
    Cap<"access",      Ref<match_access>,       CNodeAccess>,
    Cap<"constructor", Ref<match_constructor>,  CNodeConstructor>,
    Cap<"function",    Ref<match_function>,     CNodeFunction>,
    Cap<"struct",      Ref<match_struct>,       CNodeStruct>,
    Cap<"union",       Ref<match_union>,        CNodeUnion>,
    Cap<"template",    Ref<match_template>,     CNodeTemplate>,
    Cap<"class",       Ref<match_class>,        CNodeClass>,
    Cap<"enum",        Ref<match_enum>,         CNodeEnum>,
    Cap<"field",       Ref<match_declaration>,  CNodeField>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_field_list(CContext& ctx, TokenSpan body) {
  using pattern = DelimitedBlock<
    Cap<"ldelim", Atom<'{'>, CNodePunct>,
    Ref<match_field>,
    Cap<"rdelim", Atom<'}'>, CNodePunct>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_namespace(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap<"namespace", Ref<match_keyword<"namespace">>, CNodeKeyword>,
    Cap<"name",      Ref<match_identifier>,           CNodeIdentifier>,
    Cap<"fields",    Ref<match_field_list>,           CNodeList>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan struct_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = match_identifier(ctx, body);
  if (tail.is_valid()) {
    ctx.add_struct(body.begin);
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

TokenSpan match_struct(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"struct",  Ref<match_keyword<"struct">>, CNodeKeyword>,
    Cap<"name",    Ref<struct_type_adder>,       CNodeIdentifier>,
    Cap<"body",    Ref<match_field_list>,        CNodeList>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

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

TokenSpan match_union(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"union", Ref<match_keyword<"union">>, CNodeKeyword>,
    Cap<"name",  Ref<union_type_adder>,       CNodeIdentifier>,
    Cap<"body",  Ref<match_field_list>,       CNodeList>
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

TokenSpan match_class(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"class", Ref<match_keyword<"class">>, CNodeKeyword>,
    Cap<"name",  Ref<class_type_adder>,       CNodeIdentifier>,
    Cap<"body",  Ref<match_field_list>,       CNodeList>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_template(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap<"template", Ref<match_keyword<"template">>, CNodeKeyword>,
    Cap<"params",   Ref<match_tdecl_list>,          CNodeList>,
    Cap<"class",    Ref<match_class>,               CNodeClass>
  >;
  return pattern::match(ctx, body);
}

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

TokenSpan match_enumerator(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap<"name",    Ref<match_identifier>, CNodeIdentifier>,
    Opt<Seq<
      Cap<"eq",    Atom<'='>,             CNodePunct>,
      Cap<"value", Ref<match_expression>, CNodeExpression>
    >>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enumerator_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Cap<"ldelim",     Atom<'{'>,             CNodePunct>,
    Cap<"enumerator", Ref<match_enumerator>, CNodeText>,
    Cap<"comma",      Atom<','>,             CNodePunct>,
    Cap<"rdelim",     Atom<'}'>,             CNodePunct>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enum(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap<"enum", Ref<match_keyword<"enum">>, CNodeKeyword>,
    Opt<
      Ref<match_keyword<"class">>
    >,
    Opt<
      Capture<"name", Ref<enum_type_adder>, CNodeIdentifier>
    >,
    Opt<Seq<
      Atom<':'>,
      Cap<"base_type", Ref<match_type>, CNodeType>
    >>,
    Cap<"body", Ref<match_enumerator_list>, CNodeList>,
    Opt<
      comma_separated<
        Cap<"decl", Ref<match_expression>, CNodeExpression>
      >
    >
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_function(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Opt<Cap<"static",  Ref<match_keyword<"static">>, CNodeKeyword>>,
    Cap<"return_type", Ref<match_type>,              CNodeType>,
    Cap<"name",        Ref<match_identifier>,        CNodeIdentifier>,
    Cap<"params",      Ref<match_decl_list>,         CNodeList>,
    Opt<Cap<"const",   Ref<match_keyword<"const">>,  CNodeKeyword>>,
    Cap<"body",        Ref<match_compound>,          CNodeCompound>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_constructor(CContext& ctx, TokenSpan body) {
  // clang-format off

  using initializers = Seq<
    Cap<"colon", Atom<':'>, CNodePunct>,
    cookbook::comma_separated<
      Seq<
        Cap<"name",  Ref<match_identifier>, CNodeIdentifier>,
        Cap<"value", Ref<match_exp_list>,   CNodeList>
      >
    >
  >;

  using pattern =
  Seq<
    Cap<"name",   Ref<&CContext::match_class_name>, CNodeTypeName>,
    Cap<"params", Ref<match_decl_list>,             CNodeList>,
    Cap<"init",   Opt<initializers>,                CNodeList>,
    Cap<"body",   Ref<match_compound>,              CNodeCompound>
  >;
  // clang-format om
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

template <typename P>
TokenSpan change_scope(CContext& ctx, TokenSpan body) {
  ctx.push_scope();
  auto tail = P::match(ctx, body);
  ctx.pop_scope();
  return tail;
}

TokenSpan match_compound(CContext& ctx, TokenSpan body) {
  using pattern =
  Ref<change_scope<
    DelimitedBlock<
      Atom<'{'>,
      Ref<match_any_statement>,
      Atom<'}'>
    >
  >>;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------
// FIXME we should cap the condition/step differently or something

TokenSpan match_for(CContext& ctx, TokenSpan body) {
  // clang-format off
  using exp_or_decl = Oneof<Ref<match_expression>, Ref<match_declaration>>;
  using pattern =
  Seq<
    Cap<"for",       Ref<match_keyword<"for">>,  CNodeKeyword>,
    Cap<"ldelim",    Atom<'('>,                  CNodePunct>,
    Cap<"init",      Opt<exp_or_decl>,           CNodeList>,
    Cap<"semi",      Atom<';'>,                  CNodePunct>,
    Cap<"condition", Opt<Ref<match_expression>>, CNodeList>,
    Cap<"semi",      Atom<';'>,                  CNodePunct>,
    Cap<"step",      Opt<Ref<match_expression>>, CNodeList>,
    Cap<"rdelim",    Atom<')'>,                  CNodePunct>,
    Cap<"body",      Ref<match_any_statement>,   CNodeList>
  >;
  // clang-format on
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

TokenSpan match_if(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap<"if",        Ref<match_keyword<"if">>,   CNodeKeyword>,
    Cap<"condition", Ref<match_exp_list>,        CNodeList>,
    Cap<"then",      Ref<match_any_statement>,   CNodeList>,
    Opt<Seq<
      Cap<"else",    Ref<match_keyword<"else">>, CNodeKeyword>,
      Cap<"body",    Ref<match_any_statement>,   CNodeList>
    >>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_return(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Ref<match_keyword<"return">>,
    Opt<
      Cap<"value", Ref<match_expression>, CNodeExpression>
    >
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_case_body(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Cap<
    "body",
    Some<
      Seq<
        Not<Ref<match_keyword<"case">>>,
        Not<Ref<match_keyword<"default">>>,
        Ref<match_any_statement>
      >
    >,
    CNode
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

TokenSpan match_case(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"case",      Ref<match_keyword<"case">>, CNodeKeyword>,
    Cap<"condition", Ref<match_expression>,      CNodeExpression>,
    Cap<"colon",     Atom<':'>,                  CNodePunct>,
    Opt<Ref<match_case_body>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

TokenSpan match_default(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"default", Ref<match_keyword<"default">>, CNodeKeyword>,
    Cap<"colon",   Atom<':'>,                     CNodePunct>,
    Opt<Ref<match_case_body>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_switch(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"switch",     Ref<match_keyword<"switch">>, CNodeKeyword>,
    Cap<"condition",  Ref<match_expression>,        CNodeExpression>,
    Cap<"ldelim",     Atom<'{'>,                    CNodePunct>,
    Any<
      Cap<"case",     Ref<match_case>,              CNodeCase>,
      Cap<"default",  Ref<match_default>,           CNodeDefault>
    >,
    Cap<"rdelim",     Atom<'}'>,                    CNodePunct>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_while(CContext& ctx, TokenSpan body) {
  // clang-format on
  using pattern =
  Seq<
    Cap<"while", Ref<match_keyword<"while">>, CNodeKeyword>,
    Cap<"cond",  Ref<match_exp_list>,         CNodeList>,
    Cap<"body",  Ref<match_any_statement>,    CNodeList>
  >;
  // clang-format off
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_do_while(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"do",    Ref<match_keyword<"do">>,    CNodeKeyword>,
    Cap<"body",  Ref<match_any_statement>,    CNodeList>,
    Cap<"while", Ref<match_keyword<"while">>, CNodeKeyword>,
    Cap<"cond",  Ref<match_exp_list>,         CNodeList>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_break(CContext& ctx, TokenSpan body) {
  using pattern = Ref<match_keyword<"break">>;
  return pattern::match(ctx, body);
}

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

  if (auto c = node->child("union")) {
    extract_declarator_list(ctx, c->child("decls"));
    return;
  }

  if (auto c = node->child("struct")) {
    extract_declarator_list(ctx, c->child("decls"));
    return;
  }

  if (auto c = node->child("class")) {
    extract_declarator_list(ctx, c->child("decls"));
    return;
  }

  if (auto c = node->child("enum")) {
    extract_declarator_list(ctx, c->child("decls"));
    return;
  }

  if (auto c = node->child("decl")) {
    extract_declarator_list(ctx, c->child("decls"));
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
    Cap<
      "typedef",
      Oneof<
        Cap2<"struct", CNodeStruct>,
        Cap2<"union",  CNodeUnion>,
        Cap2<"class",  CNodeClass>,
        Cap2<"enum",   CNodeEnum>,
        Cap2<"decl",   CNodeDeclaration>
      >,
      CNode
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
// If declaration is before statemetn, we parse "x = 1;" as a declaration
// because it matches a declarator (bare identifier) + initializer list :/

// FIXME - is that still true?

TokenSpan match_any_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Oneof<
    Cap<"semi",        Atom<';'>,              CNodePunct>,
    Cap<"for",         Ref<match_for>,         CNodeFor>,
    Cap<"if",          Ref<match_if>,          CNodeIf>,
    Cap<"return",      Ref<match_return>,      CNodeReturn>,
    Cap<"switch",      Ref<match_switch>,      CNodeSwitch>,
    Cap<"dowhile",     Ref<match_do_while>,    CNodeDoWhile>,
    Cap<"while",       Ref<match_while>,       CNodeWhile>,
    Cap<"compound",    Ref<match_compound>,    CNodeCompound>,
    Cap<"break",       Ref<match_break>,       CNodeBreak>,
    Cap<"continue",    Ref<match_continue>,    CNodeContinue>,
    Cap<"expression",  Ref<match_expression>,  CNodeExpression>,
    Cap<"declaration", Ref<match_declaration>, CNodeDeclaration>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_translation_unit(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Any<
    Cap<"semi",        Atom<';'>,               CNodePunct>,
    Cap<"class",       Ref<match_class>,        CNodeClass>,
    Cap<"struct",      Ref<match_struct>,       CNodeStruct>,
    Cap<"union",       Ref<match_union>,        CNodeUnion>,
    Cap<"enum",        Ref<match_enum>,         CNodeEnum>,
    Cap<"template",    Ref<match_template>,     CNodeTemplate>,
    Cap<"declaration", Ref<match_declaration>,  CNodeDeclaration>,
    Cap<"preproc",     Ref<match_preproc>,      CNodePreproc>,
    Cap<"function",    Ref<match_function>,     CNodeFunction>,
    Cap<"namespace",   Ref<match_namespace>,    CNodeNamespace>
    //Cap<"typedef",     Ref<match_typedef>,      CNodeTypedef>, FIXME
  >;
  // clang-format on

  return Cap<"translation_unit", pattern, CNodeTranslationUnit>::match(ctx, body);
}

//------------------------------------------------------------------------------
