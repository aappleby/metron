// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CParser.hpp"

#include "metrolib/core/Log.h"
#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Parseroni.hpp"
#include "matcheroni/Utilities.hpp"
#include "matcheroni/Cookbook.hpp"

#include "CConstants.hpp"
#include "CContext.hpp"
#include "CNode.hpp"
#include "CSourceFile.hpp"
#include "CSourceRepo.hpp"
#include "CToken.hpp"
#include "SST.hpp"
#include "NodeTypes.hpp"

#include <assert.h>

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

TokenSpan match_any_statement(CContext& ctx, TokenSpan body);

//------------------------------------------------------------------------------

template <StringParam match_name, typename P>
struct TraceToken {

  inline static void print_match2(TokenSpan body, TokenSpan tail, int width) {

    if (tail.is_valid()) {
      const char* a = body.begin->text.begin;
      const char* b = tail.begin->text.begin;
      const char* c = (body.end - 1)->text.end;
      utils::print_match(a, b, c, 0x80FF80, 0xCCCCCC, width);
    } else {
      const char* a = body.begin->text.begin;
      const char* b = tail.end->text.begin;
      const char* c = (body.end - 1)->text.end;
      utils::print_match(a, b, c, 0xCCCCCC, 0x8080FF, width);
    }
  }

  static TokenSpan match(CContext& ctx, TokenSpan body) {
    matcheroni_assert(body.is_valid());
    if (body.is_empty()) return body.fail();

    auto name = match_name.str_val;

    int depth = ctx.trace_depth++;

    print_match2(body, body, 40);
    utils::print_trellis(depth, name, "?", 0xCCCCCC);

    auto tail = P::match(ctx, body);
    depth = --ctx.trace_depth;

    print_match2(body, tail, 40);

    if (tail.is_valid()) {
      utils::print_trellis(depth, name, "!", 0x80FF80);
    }
    else {
      utils::print_trellis(depth, name, "X", 0x8080FF);
    }

    return tail;
  }
};

template <StringParam match_name, typename pattern, typename node_type>
//using Cap = TraceToken<match_name, Capture<match_name, pattern, node_type>>;
using Cap = Capture<match_name, pattern, node_type>;

template <StringParam match_name, typename pattern, typename node_type>
using Cap1 = Capture<match_name, pattern, node_type>;

template <StringParam match_name, typename node_type>
using Cap2 = Capture<match_name, node_type, node_type>;


template <StringParam match_name, typename pattern>
using Cap3 = Capture<match_name, pattern, CNode>;

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
    if (ctx.atom_cmp(tok_a.text.begin[0], lit.str_val[i]) != 0) return body.fail();
    body = body.advance(1);
  }

  return body;
}

//------------------------------------------------------------------------------
// Our builtin types are any sequence of prefixes followed by a builtin type

TokenSpan match_builtin_type(CContext& ctx, TokenSpan body) {
  using match_prefix = Ref<&CContext::match_builtin_type_prefix>;
  using match_base   = Ref<&CContext::match_builtin_type_base>;
  using match_suffix = Ref<&CContext::match_builtin_type_suffix>;

  // clang-format off
  using pattern =
  Seq<
    Any<
      Seq<
        Cap3<"prefix", match_prefix>,
        And<match_base>
      >
    >,
    Cap3<"base_type", match_base>,
    Opt<Cap3<"suffix", match_suffix>>
  >;

  auto tail = pattern::match(ctx, body);
  return tail;
}

//------------------------------------------------------------------------------
// Excluding builtins and typedefs from being identifiers changes the total
// number of parse nodes, but why?

// - Because "uint8_t *x = 5" gets misparsed as an expression if uint8_t matches
// as an identifier

TokenSpan CNodeIdentifier::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Not<Ref<match_builtin_type>>,
    Not<Ref<&CContext::match_typedef_name>>,
    Atom<LEX_IDENTIFIER>,
    Opt<Seq<
      // FIXME we should really handle scoped identifiers better...
      Ref<match_punct<"::">>,
      CNodeIdentifier
    >>
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

TokenSpan CNodePreproc::match(CContext& ctx, TokenSpan body) {
  if (body.begin->type != LEX_PREPROC) return body.fail();

  std::string s(body.begin->text.begin, body.begin->text.end);

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
    c_include_line<Ref<&CContext::handle_include>>::match(ctx, body.begin->text);
  }

  return body.advance(1);
};

//------------------------------------------------------------------------------

TokenSpan CNodeConstant::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Oneof<
    Cap3<"float",   Atom<LEX_FLOAT>>,
    Cap3<"int",     Atom<LEX_INT>>,
    Cap3<"char",    Atom<LEX_CHAR>>,
    Cap3<"string",  Some<Atom<LEX_STRING>>>,
    Cap3<"nullptr", Ref<match_literal<"nullptr">>>,
    Cap3<"true",    Ref<match_literal<"true">>>,
    Cap3<"false",   Ref<match_literal<"false">>>
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

template <StringParam lit>
using prefix_op = Ref<match_prefix_op<lit>>;

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

template <StringParam lit>
using binary_op = Ref<match_binary_op<lit>>;

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

TokenSpan CNodeQualifier::match(CContext& ctx, TokenSpan body) {
  TextSpan span = body.begin->text;
  if (SST<qualifiers>::match(span.begin, span.end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

//------------------------------------------------------------------------------

TokenSpan CNodeAccess::match(CContext& ctx, TokenSpan body) {
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

TokenSpan CNodeCall::match(CContext& ctx, TokenSpan body) {
  using pattern = Seq<
    Cap2<"func_name",     CNodeIdentifier>,
    Cap3<"template_args", CNodeTExpList>,
    Cap2<"func_args",     CNodeExpList>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

// clang-format off
using ExpressionPrefixOp =
Oneof<
  Cap3<"cast",      Ref<match_prefix_cast>>,
  Cap3<"preinc",    Ref<match_prefix_op<"++">>>,
  Cap3<"predec",    Ref<match_prefix_op<"--">>>,
  Cap3<"preplus",   Ref<match_prefix_op<"+">>>,
  Cap3<"preminus",  Ref<match_prefix_op<"-">>>,
  Cap3<"prebang",   Ref<match_prefix_op<"!">>>,
  Cap3<"pretilde",  Ref<match_prefix_op<"~">>>,
  Cap3<"prestar",   Ref<match_prefix_op<"*">>>,
  Cap3<"preamp",    Ref<match_prefix_op<"&">>>
>;
// clang-format on


//----------------------------------------

// clang-format off
using ExpressionCore =
Oneof<
  Cap2<"call",       CNodeCall>,
  Cap2<"paren",      CNodeIndexList>,
  Cap2<"identifier", CNodeIdentifier>,
  Cap2<"constant",   CNodeConstant>
>;
// clang-format on

//----------------------------------------

// clang-format off
using ExpressionSuffixOp =
Oneof<
  Cap2<"exp_list",   CNodeExpList>,
  Cap2<"index_list", CNodeIndexList>,
  Cap3<"postinc",    Ref<match_suffix_op<"++">>>,
  Cap3<"postdec",    Ref<match_suffix_op<"--">>>
>;
// clang-format on


//----------------------------------------

// clang-format off
using unit_pattern =
Seq<
  Any<Cap3<"prefix", ExpressionPrefixOp>>,
  ExpressionCore,
  Any<Cap3<"suffix", ExpressionSuffixOp>>
>;
// clang-format on

//----------------------------------------

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
      Cap3<"then", comma_separated<CNodeExpression>>
    >,
    Ref<match_binary_op<":">>
  >;
  return pattern::match(ctx, body);
}

//----------------------------------------

TokenSpan match_binary_op2(CContext& ctx, TokenSpan body) {
  matcheroni_assert(body.is_valid());

  if (ctx.atom_cmp(*body.begin, LEX_PUNCT)) {
    return body.fail();
  }

  // clang-format off
  switch (body.begin->text.begin[0]) {
    case '+':
      return Oneof<Ref<match_binary_op<"+=">>, Ref<match_binary_op<"+">>>::match(ctx, body);
    case '-':
      return Oneof<Ref<match_binary_op<"->*">>, Ref<match_binary_op<"->">>, Ref<match_binary_op<"-=">>, Ref<match_binary_op<"-">>>::match(ctx, body);
    case '*':
      return Oneof<Ref<match_binary_op<"*=">>, Ref<match_binary_op<"*">>>::match(ctx, body);
    case '/':
      return Oneof<Ref<match_binary_op<"/=">>, Ref<match_binary_op<"/">>>::match(ctx, body);
    case '=':
      return Oneof<Ref<match_binary_op<"==">>, Ref<match_binary_op<"=">>>::match(ctx, body);
    case '<':
      return Oneof<Ref<match_binary_op<"<<=">>, Ref<match_binary_op<"<=>">>, Ref<match_binary_op<"<=">>, Ref<match_binary_op<"<<">>, Ref<match_binary_op<"<">>>::match(ctx, body);
    case '>':
      return Oneof<Ref<match_binary_op<">>=">>, Ref<match_binary_op<">=">>, Ref<match_binary_op<">>">>, Ref<match_binary_op<">">>>::match(ctx, body);
    case '!':
      return Ref<match_binary_op<"!=">>::match(ctx, body);
    case '&':
      return Oneof<Ref<match_binary_op<"&&">>, Ref<match_binary_op<"&=">>, Ref<match_binary_op<"&">>>::match(ctx, body);
    case '|':
      return Oneof<Ref<match_binary_op<"||">>, Ref<match_binary_op<"|=">>, Ref<match_binary_op<"|">>>::match(ctx, body);
    case '^':
      return Oneof<Ref<match_binary_op<"^=">>, Ref<match_binary_op<"^">>>::match(ctx, body);
    case '%':
      return Oneof<Ref<match_binary_op<"%=">>, Ref<match_binary_op<"%">>>::match(ctx, body);
    case '.':
      return Oneof<Ref<match_binary_op<".*">>, Ref<match_binary_op<".">>>::match(ctx, body);
    case '?':
      return Ref<match_ternary_op>::match(ctx, body);

      // FIXME this is only for C++, and
      // case ':': return NodeBinaryOp<"::">::match(ctx, body);
      // default:  return nullptr;
  }
  // clang-format on

  return body.fail();
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

TokenSpan CNodeExpression::match(CContext& ctx, TokenSpan body) {

  using pattern =
  Seq<
    Cap3<"unit", unit_pattern>,
    Any<
      Seq<
        Cap3<"op", Ref<match_binary_op2>>,
        Cap3<"unit", unit_pattern>
      >
    >
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

//------------------------------------------------------------------------------

TokenSpan CNodeDeclList::match(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Cap2<"decl", CNodeDeclaration>,
    Atom<','>,
    Atom<')'>
  >;
  return pattern::match(ctx, body);
}

TokenSpan CNodeExpList::match(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Cap2<"exp", CNodeExpression>,
    Atom<','>,
    Atom<')'>
  >;
  return pattern::match(ctx, body);
}

TokenSpan CNodeIndexList::match(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'['>,
    Cap2<"exp", CNodeExpression>,
    Atom<','>,
    Atom<']'>
  >;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

const CToken* find_matching_delimiter(const CToken* begin, const CToken* end) {
  for (auto cursor = begin + 1; cursor < end; cursor++) {
    if (cursor->text.begin[0] == '>') return cursor;
    if (cursor->text.begin[0] == '<') cursor = find_matching_delimiter(cursor, end);
    if (!cursor) return cursor;
  }
  return nullptr;
}

TokenSpan get_template_span(TokenSpan body) {
  if (body.begin->text.begin[0] != '<') return body.fail();
  auto ldelim = body.begin;
  auto rdelim = find_matching_delimiter(body.begin, body.end);
  if (rdelim == nullptr) return body.fail();
  return TokenSpan(ldelim + 1, rdelim);
}

TokenSpan CNodeTExpList::match(CContext& ctx, TokenSpan body) {
  auto tight_span = get_template_span(body);
  if (!tight_span.is_valid()) return body.fail();

  using pattern = comma_separated<Cap2<"exp", CNodeExpression>>;
  auto tail = pattern::match(ctx, tight_span);

  if (tail.is_valid() && tail.is_empty()) {
    return TokenSpan(tight_span.end + 1, body.end);
  }
  else {
    return body.fail();
  }
};

TokenSpan CNodeTDeclList::match(CContext& ctx, TokenSpan body) {
  auto tight_span = get_template_span(body);
  if (!tight_span.is_valid()) return body.fail();

  using pattern = comma_separated<Cap2<"decl", CNodeDeclaration>>;
  auto tail = pattern::match(ctx, tight_span);

  if (tail.is_valid() && tail.is_empty()) {
    return TokenSpan(tight_span.end + 1, body.end);
  }
  else {
    return body.fail();
  }
};

//------------------------------------------------------------------------------

TokenSpan CNodeType::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Oneof<
      // These have to be NodeIdentifier because "void foo(struct S);"
      // is valid even without the definition of S.
      //Seq<Ref<match_keyword<"class">>,  Oneof<CNodeIdentifier, Ref<&CContext::match_class_name>>>,
      //Seq<Ref<match_keyword<"union">>,  Oneof<CNodeIdentifier, Ref<&CContext::match_union_name>>>,
      //Seq<Ref<match_keyword<"struct">>, Oneof<CNodeIdentifier, Ref<&CContext::match_struct_name>>>,
      //Seq<Ref<match_keyword<"enum">>,   Oneof<CNodeIdentifier, Ref<&CContext::match_enum_name>>>,

      Ref<&CContext::match_class_name>,
      Ref<&CContext::match_struct_name>,
      Ref<&CContext::match_union_name>,
      Ref<&CContext::match_enum_name>,

      Ref<match_builtin_type>,
      Ref<&CContext::match_typedef_name>
    >,
    Opt<Cap2<"targs", CNodeTExpList>>,
    Any<Cap<"star", Atom<'*'>, CNode>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_decl_prefix(CContext& ctx, TokenSpan body) {
  using pattern =
  Oneof<
    Cap<"static", Ref<match_keyword<"static">>, CNodeKeyword>,
    Cap<"const",  Ref<match_keyword<"const">>,  CNodeKeyword>
  >;
  return pattern::match(ctx, body);
 }

//------------------------------------------------------------------------------

TokenSpan CNodeDeclaration::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Any<Ref<match_decl_prefix>>,
    Cap2<"decl_type",      CNodeType>,
    Cap2<"decl_name",      CNodeIdentifier>,
    Any<Cap2<"decl_array", CNodeIndexList>>,
    Opt<Seq<
      Atom<'='>,
      Cap2<"decl_value", CNodeExpression>
    >>,
    Cap<"semi", Opt<Atom<';'>>, CNodePunct>
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
    Atom<';'>,
    Cap2<"access",      CNodeAccess>,
    Cap2<"constructor", CNodeConstructor>,
    Cap2<"function",    CNodeFunction>,
    Cap2<"struct",      CNodeStruct>,
    Cap2<"union",       CNodeUnion>,
    Cap2<"template",    CNodeTemplate>,
    Cap2<"class",       CNodeClass>,
    Cap2<"enum",        CNodeEnum>,
    Cap2<"decl",        CNodeDeclaration>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

TokenSpan CNodeFieldList::match(CContext& ctx, TokenSpan body) {
  using pattern = DelimitedBlock<
    Cap<"ldelim", Atom<'{'>, CNodePunct>,
    Ref<match_field>,
    Cap<"rdelim", Atom<'}'>, CNodePunct>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeNamespace::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap<"namespace", Ref<match_keyword<"namespace">>, CNodeKeyword>,
    Cap2<"name",     CNodeIdentifier>,
    Cap2<"fields",   CNodeFieldList>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan struct_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = CNodeIdentifier::match(ctx, body);
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

TokenSpan CNodeStruct::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"struct",  Ref<match_keyword<"struct">>, CNodeKeyword>,
    Cap<"name",    Ref<struct_type_adder>,       CNodeIdentifier>,
    Cap2<"fields", CNodeFieldList>,
    Cap<"semi",    Atom<';'>,                    CNodePunct>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan union_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = CNodeIdentifier::match(ctx, body);
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

TokenSpan CNodeUnion::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"union",   Ref<match_keyword<"union">>, CNodeKeyword>,
    Cap<"name",    Ref<union_type_adder>,       CNodeIdentifier>,
    Cap2<"fields", CNodeFieldList>,
    Cap<"semi",    Atom<';'>,                   CNodePunct>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan class_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = CNodeIdentifier::match(ctx, body);
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

TokenSpan CNodeClass::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"class", Ref<match_keyword<"class">>, CNodeKeyword>,
    Cap<"name",  Ref<class_type_adder>,       CNodeIdentifier>,
    Cap2<"body", CNodeFieldList>,
    Cap<"semi",  Atom<';'>,                   CNodePunct>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeTemplate::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap<"template", Ref<match_keyword<"template">>, CNodeKeyword>,
    Cap2<"params",  CNodeTDeclList>,
    Cap2<"class",   CNodeClass>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan enum_type_adder(CContext& ctx, TokenSpan body) {
  auto tail = CNodeIdentifier::match(ctx, body);
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
    Cap2<"name", CNodeIdentifier>,
    Opt<Seq<
      Cap3<"eq",    Atom<'='>>,
      Cap2<"value", CNodeExpression>
    >>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enumerator_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Cap3<"ldelim", Atom<'{'>>,
    Cap<"enumerator", Ref<match_enumerator>, CNode>,
    Atom<','>,
    Cap3<"rdelim", Atom<'}'>>
  >;
  return pattern::match(ctx, body);
}

TokenSpan CNodeEnum::match(CContext& ctx, TokenSpan body) {
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
      Cap2<"type", CNodeType>
    >>,
    Cap<"body", Ref<match_enumerator_list>, CNode>,
    Opt<
      comma_separated<
        Cap2<"decl", CNodeExpression>
      >
    >,
    Cap<"semi", Atom<';'>, CNodePunct>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeFunction::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap2<"type",   CNodeType>,
    Cap2<"name",   CNodeIdentifier>,
    Cap2<"params", CNodeDeclList>,
    Cap3<"const",  Opt<Ref<match_keyword<"const">>>>,
    Cap2<"body",   CNodeCompound>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeConstructor::match(CContext& ctx, TokenSpan body) {
  // clang-format off

  using initializers = Seq<
    Cap3<"colon", Atom<':'>>,
    cookbook::comma_separated<
      Seq<
        Cap2<"name",  CNodeIdentifier>,
        Cap2<"value", CNodeExpList>
      >
    >
  >;

  using pattern =
  Seq<
    Cap3<"name",   Ref<&CContext::match_class_name>>,
    Cap2<"params", CNodeDeclList>,
    Cap3<"init",   Opt<initializers>>,
    Cap2<"body",   CNodeCompound>
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

//------------------------------------------------------------------------------

TokenSpan CNodeCompound::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Ref<change_scope<
    DelimitedBlock<
      Atom<'{'>,
      Cap<"statement", Ref<match_any_statement>, CNode>,
      Atom<'}'>
    >
  >>;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

TokenSpan CNodeFor::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"for">>,
    Atom<'('>,
    Cap<
      "init",
      // This is _not_ the same as
      // Opt<Oneof<e, x>>, Atom<';'>
      Oneof<
        Seq<comma_separated<CNodeExpression>,  Atom<';'>>,
        Seq<comma_separated<CNodeDeclaration>>,
        Atom<';'>
      >,
      CNode
    >,
    Cap<"condition", Opt<comma_separated<CNodeExpression>>, CNodeList>,
    Atom<';'>,
    Cap<"step", Opt<comma_separated<CNodeExpression>>, CNodeList>,
    Atom<')'>,
    Ref<match_any_statement>
  >;
  // clang-format on
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

TokenSpan CNodeIf::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Cap3<"if",        Ref<match_keyword<"if">>>,
    Cap2<"condition", CNodeExpList>,
    Cap3<"then",      Ref<match_any_statement>>,
    Opt<Seq<
      Cap3<"else",    Ref<match_keyword<"else">>>,
      Cap3<"body",    Ref<match_any_statement>>
    >>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeReturn::match(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Ref<match_keyword<"return">>,
    Opt<
      Cap2<"value", CNodeExpression>
    >,
    Atom<';'>
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

TokenSpan CNodeCase::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"case">>,
    Cap2<"condition", CNodeExpression>,
    Atom<':'>,
    Opt<Ref<match_case_body>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

TokenSpan CNodeDefault::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"default">>,
    Atom<':'>,
    Ref<match_case_body>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeSwitch::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"switch",     Ref<match_keyword<"switch">>, CNode>,
    Cap2<"condition", CNodeExpression>,
    Cap<"ldelim",     Atom<'{'>, CNode>,
    Any<
      Cap2<"case",    CNodeCase>,
      Cap2<"default", CNodeDefault>
    >,
    Cap<"rdelim",     Atom<'}'>, CNode>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeWhile::match(CContext& ctx, TokenSpan body) {
  // clang-format on
  using pattern =
  Seq<
    Cap3<"while", Ref<match_keyword<"while">>>,
    Cap2<"cond",  CNodeExpList>,
    Cap3<"body",  Ref<match_any_statement>>
  >;
  // clang-format off
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeDoWhile::match(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap3<"do",    Ref<match_keyword<"do">>>,
    Cap3<"body",  Ref<match_any_statement>>,
    Cap3<"while", Ref<match_keyword<"while">>>,
    Cap2<"cond",  CNodeExpList>,
    Cap3<"semi",  Atom<';'>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan CNodeBreak::match(CContext& ctx, TokenSpan body) {
  using pattern = Seq<Ref<match_keyword<"break">>, Atom<';'>>;
  return pattern::match(ctx, body);
}

TokenSpan CNodeContinue::match(CContext& ctx, TokenSpan body) {
  using pattern = Seq<Ref<match_keyword<"continue">>, Atom<';'>>;
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

    if (strcmp(child->match_name, "decl") == 0) {
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

TokenSpan CNodeStatement::match(CContext& ctx, TokenSpan body) {
  using pattern = Seq<CNodeExpression, Atom<';'>>;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------
// If declaration is before statemetn, we parse "x = 1;" as a declaration
// because it matches a declarator (bare identifier) + initializer list :/

// FIXME - is that still true?

TokenSpan match_any_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Oneof<
    Cap2<"for",         CNodeFor>,
    Cap2<"if",          CNodeIf>,
    Cap2<"return",      CNodeReturn>,
    Cap2<"switch",      CNodeSwitch>,
    Cap2<"dowhile",     CNodeDoWhile>,
    Cap2<"while",       CNodeWhile>,
    Cap2<"compound",    CNodeCompound>,
    Cap2<"break",       CNodeBreak>,
    Cap2<"continue",    CNodeContinue>,
    Cap2<"statement",   CNodeStatement>,
    Cap2<"declaration", CNodeDeclaration>,
    Cap3<"semi",        Atom<';'>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_translation_unit(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Any<Oneof<
    Cap2<"class",       CNodeClass>,
    Cap2<"struct",      CNodeStruct>,
    Cap2<"union",       CNodeUnion>,
    Cap2<"enum",        CNodeEnum>,
    Cap2<"template",    CNodeTemplate>,
    Cap2<"declaration", CNodeDeclaration>,
    Cap2<"preproc",     CNodePreproc>,
    Cap2<"function",    CNodeFunction>,
    Cap2<"namespace",   CNodeNamespace>,
    // Cap2<"typedef",    CNodeTypedef>, FIXME
    Cap3<"semi",        Atom<';'>>
  >>;
  // clang-format on

  return Cap<"translation_unit", pattern, CNodeTranslationUnit>::match(ctx, body);
}

//------------------------------------------------------------------------------
