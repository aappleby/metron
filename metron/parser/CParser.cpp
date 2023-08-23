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

TokenSpan match_statement     (CContext& ctx, TokenSpan body);
TokenSpan match_targ_list     (CContext& ctx, TokenSpan body);
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
TokenSpan match_access        (CContext& ctx, TokenSpan body);
TokenSpan match_type_name     (CContext& ctx, TokenSpan body);
TokenSpan match_call          (CContext& ctx, TokenSpan body);

using cap_targ_list   = CaptureAnon<Ref<match_targ_list>,  CNodeList>;
using cap_statement   = CaptureAnon<Ref<match_statement>,  CNodeList>;
using cap_exp_list    = CaptureAnon<Ref<match_exp_list>,   CNodeList>;
using cap_expression  = CaptureAnon<Ref<match_expression>, CNodeExpression>;
using cap_access      = CaptureAnon<Ref<match_access>,     CNodeAccess>;
using cap_type_name   = CaptureAnon<Ref<match_type_name>,  CNodeTypeName>;
using cap_call        = CaptureAnon<Ref<match_call>,       CNodeCall>;
using cap_decl_list   = CaptureAnon<Ref<match_decl_list>,  CNodeList>;
using cap_index_list  = CaptureAnon<Ref<match_index_list>, CNodeList>;
using cap_compound    = CaptureAnon<Ref<match_compound>,   CNodeCompound>;
using cap_function    = CaptureAnon<Ref<match_function>,   CNodeFunction>;
//using cap_typedef    = CaptureAnon<Ref<match_typedef>,    CNodeTypedef;
using cap_union       = CaptureAnon<Ref<match_union>, CNodeUnion>;
using cap_constructor = CaptureAnon<Ref<match_constructor>,  CNodeConstructor>;
using cap_declaration = CaptureAnon<Ref<match_declaration>, CNodeDeclaration>;

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

//template <StringParam match_tag, typename pattern, typename node_type>
//using Cap = TraceToken<match_tag, Capture<match_tag, pattern, node_type>>;
//using Cap = Capture<match_tag, pattern, node_type>;

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

template<StringParam lit>
using cap_keyword = CaptureAnon<Ref<match_keyword<lit>>, CNodeKeyword>;

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

template<StringParam p>
using cap_punct = CaptureAnon<Ref<match_punct<p>>, CNodePunct>;

//------------------------------------------------------------------------------
// Our builtin types are any sequence of prefixes followed by a builtin type

TokenSpan match_builtin_type(CContext& ctx, TokenSpan body) {
  return ctx.match_builtin_type_base(body);
}

using cap_builtin_type = CaptureAnon<Ref<match_builtin_type>, CNodeTypeName>;

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
        cap_punct<"::">
      >
    >,
    Tag<"identifier", cap_identifier>
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

using cap_preproc = CaptureAnon<Ref<match_preproc>, CNodePreproc>;


//------------------------------------------------------------------------------

using cap_constant =
CaptureAnon<
  Oneof<
    Atom<LEX_FLOAT>,
    Atom<LEX_INT>,
    Atom<LEX_CHAR>,
    Some<Atom<LEX_STRING>>,
    Ref<match_keyword<"nullptr">>,
    Ref<match_keyword<"true">>,
    Ref<match_keyword<"false">>
  >,
  CNodeConstant
>;

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

/*
NodePrefixCast() {
  precedence = 3;
  assoc = -2;
}
*/

using cap_cast =
CaptureAnon<
  Seq<
    Tag<"ldelim", cap_punct<"(">>,
    Tag<"type",   cap_type_name>,
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

// clang-format off
using cap_exp_prefix =
Oneof<
  cap_cast,
  cap_punct<"++">,
  cap_punct<"--">,
  cap_punct<"+">,
  cap_punct<"-">,
  cap_punct<"!">,
  cap_punct<"~">,
  cap_punct<"*">,
  cap_punct<"&">
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

// clang-format off
using cap_exp_suffix =
Oneof<
  cap_exp_list,
  CaptureAnon<Ref<match_index_list>,      CNodeList>,
  CaptureAnon<Ref<match_suffix_op<"++">>, CNodePunct>,
  CaptureAnon<Ref<match_suffix_op<"--">>, CNodePunct>
>;
// clang-format on


//----------------------------------------

TokenSpan match_exp_unit(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern = Seq<
    Any<Tag<"prefix", cap_exp_prefix>>,
        Tag<"core",   cap_exp_core>,
    Any<Tag<"suffix", cap_exp_suffix>>
  >;
  // clang-format on

  return pattern::match(ctx, body);
}

using cap_exp_unit = CaptureAnon<Ref<match_exp_unit>, CNodeUnit>;

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
    Opt<comma_separated<cap_expression>>,
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


using cap_assignment_op = CaptureAnon<Ref<match_assignment_ops>, CNodeOperator>;

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
    Tag<"unit",   cap_exp_unit>,
    Any<Seq<
      Tag<"op",   Ref<cap_binary_ops>>,
      Tag<"unit", cap_exp_unit>
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

//------------------------------------------------------------------------------

/*
TokenSpan match_lvalue(CContext& ctx, TokenSpan body) {
  using pattern = Oneof<
    cap_any_identifier
  >;


}

TokenSpan match_assignment(CContext& ctx, TokenSpan body) {
  using pattern = Seq<
}
*/

//------------------------------------------------------------------------------

TokenSpan match_decl_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"(">>,
    Tag<"decl",   cap_declaration>,
    cap_punct<",">,
    Tag<"rdelim", cap_punct<")">>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_exp_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"(">>,
    Tag<"exp",    cap_expression>,
    cap_punct<",">,
    Tag<"rdelim", cap_punct<")">>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_index_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"[">>,
    Tag<"exp",    cap_expression>,
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

  tail = cap_punct<"<">::match(ctx, body);
  if (!tail.is_valid()) return tail;

  tail = comma_separated<cap_expression>::match(ctx, tight_span);
  if (!tail.is_valid()) return tail;
  if (!tail.is_empty()) return body.fail();

  TokenSpan rdelim_body(tight_span.end, body.end);
  tail = cap_punct<">">::match(ctx, rdelim_body);

  return tail;
};


TokenSpan match_tdecl_list(CContext& ctx, TokenSpan body) {
  auto tight_span = get_template_span(body);
  if (!tight_span.is_valid()) return body.fail();

  using pattern = comma_separated<Tag<"decl", cap_declaration>>;
  auto tail = pattern::match(ctx, tight_span);

  if (tail.is_valid() && tail.is_empty()) {
    return TokenSpan(tight_span.end + 1, body.end);
  }
  else {
    return body.fail();
  }
};

using cap_tdecl_list = CaptureAnon<Ref<match_tdecl_list>, CNodeList>;

//------------------------------------------------------------------------------

TokenSpan match_type(CContext& ctx, TokenSpan body) {
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

      Tag<"class_name",   CaptureAnon<Ref<&CContext::match_class_name>,   CNodeTypeName>>,
      Tag<"struct_name",  CaptureAnon<Ref<&CContext::match_struct_name>,  CNodeTypeName>>,
      Tag<"union_name",   CaptureAnon<Ref<&CContext::match_union_name>,   CNodeTypeName>>,
      Tag<"enum_name",    CaptureAnon<Ref<&CContext::match_enum_name>,    CNodeTypeName>>,
      Tag<"typedef_name", CaptureAnon<Ref<&CContext::match_typedef_name>, CNodeTypeName>>,
      Tag<"builtin_name", cap_builtin_type>
    >,
    Opt<Tag<"template_args", cap_targ_list>>,
    Any<Tag<"star",          cap_punct<"*">>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_type = CaptureAnon<Ref<match_type>, CNodeType>;

//------------------------------------------------------------------------------

TokenSpan match_declaration(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Any<
      cap_keyword<"static">,
      cap_keyword<"const">
    >,

    Tag<"decl_type", cap_type>,
    Tag<"decl_name", cap_identifier>,
    Any<
      Tag<"decl_array", cap_index_list>
    >,
    Opt<Seq<
      cap_punct<"=">,
      Tag<"decl_value",   cap_expression>
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

using cap_field = CaptureAnon<Ref<match_declaration>,  CNodeField>;

TokenSpan match_field_list(CContext& ctx, TokenSpan body) {
  using pattern = DelimitedBlock<
    Tag<"ldelim", cap_punct<"{">>,
    Oneof<
      cap_punct<";">,
      cap_access,
      cap_constructor,
      cap_function,
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
    Tag<"fields",    cap_field_list>
  >;
  return pattern::match(ctx, body);
}

using cap_namespace = CaptureAnon<Ref<match_namespace>, CNodeNamespace>;

//------------------------------------------------------------------------------

TokenSpan match_struct(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Tag<"struct",  cap_keyword<"struct">>,
    Tag<"name",    Dispatch<cap_identifier, Ref<&CContext::add_struct2>>>,
    Tag<"body",    cap_field_list>
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
    Tag<"body",  cap_field_list>
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
    cap_keyword<"class">,
    Tag<"name",  cap_class_type_name>,
    Tag<"body",  cap_field_list>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

using cap_class = CaptureAnon<Ref<match_class>, CNodeClass>;

//------------------------------------------------------------------------------

using cap_template = CaptureAnon<
  Seq<
    cap_keyword<"template">,
    Tag<"params", cap_tdecl_list>,
    cap_class
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
      Tag<"value", cap_expression>
    >>
  >;
  return pattern::match(ctx, body);
}

using cap_enumerator_name = CaptureAnon<Ref<match_enumerator>, CNodeText>;

TokenSpan match_enumerator_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Tag<"ldelim", cap_punct<"{">>,
    cap_enumerator_name,
    cap_punct<",">,
    Tag<"rdelim", cap_punct<"}">>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enum(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    cap_keyword<"enum">,
    Opt<
      cap_keyword<"class">
    >,
    Opt<
      Tag<"name", cap_enum_type_name>
    >,
    Opt<Seq<
      cap_punct<":">,
      Tag<"base_type", cap_type>
    >>,
    Tag<"body", CaptureAnon<Ref<match_enumerator_list>, CNodeList>>,
    Opt<
      comma_separated<
        Tag<"decl", cap_expression>
      >
    >
  >;
  return pattern::match(ctx, body);
}

using cap_enum =  CaptureAnon<Ref<match_enum>, CNodeEnum>;

//------------------------------------------------------------------------------

TokenSpan match_function(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Opt<Tag<"static",  cap_keyword<"static">>>,
    Tag<"return_type", cap_type>,
    Tag<"name",        cap_identifier>,
    Tag<"params",      cap_decl_list>,
    Opt<Tag<"const",   cap_keyword<"const">>>,
    Tag<"body",        cap_compound>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_constructor(CContext& ctx, TokenSpan body) {
  // clang-format off

  using initializers = Seq<
    cap_punct<":">,
    comma_separated<
      Seq<
        Tag<"name",  cap_identifier>,
        Tag<"value", cap_exp_list>
      >
    >
  >;

  using cap_class_name = CaptureAnon<Ref<&CContext::match_class_name>, CNodeTypeName>;


  using pattern =
  Seq<
    Tag<"name",   cap_class_name>,
    Tag<"params", cap_decl_list>,
    Tag<"init",   CaptureAnon<Opt<initializers>, CNodeList>>,
    Tag<"body",   cap_compound>
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
      Ref<match_statement>,
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
    Tag<"for",       cap_keyword<"for">>,
    Tag<"ldelim",    cap_punct<"(">>,
    Tag<"init",      CaptureAnon<Opt<exp_or_decl>, CNodeList>>,
    cap_punct<";">,
    Tag<"condition", Opt<cap_expression>>,
    cap_punct<";">,
    Tag<"step",      Opt<cap_expression>>,
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
    Tag<"then",      cap_statement>,
    Opt<Seq<
      Tag<"else",    cap_keyword<"else">>,
      Tag<"body",    cap_statement>
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
    Tag<"value",  Opt<cap_expression>>
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
      CNode
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
    Tag<"condition", cap_expression>,
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
    Tag<"condition",  cap_expression>,
    Tag<"ldelim",     cap_punct<"{">>,
    Any<
      Tag<"case", cap_case>,
      Tag<"default", cap_default>
    >,
    Tag<"rdelim",     cap_punct<"}">>
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
    Tag<"cond",  cap_exp_list>
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

using cap_assignment =
CaptureAnon<
  Seq<cap_any_identifier, cap_assignment_op, cap_expression>,
  CNodeAssignment
>;

//------------------------------------------------------------------------------
// If declaration is before statemetn, we parse "x = 1;" as a declaration
// because it matches a declarator (bare identifier) + initializer list :/

// FIXME - is that still true?

TokenSpan match_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Oneof<
    cap_punct<";">,
    cap_for,
    cap_if,
    cap_return,
    cap_switch,
    cap_do_while,
    cap_while,
    cap_compound,
    cap_keyword<"break">,
    cap_keyword<"continue">,
    cap_assignment,
    cap_expression,
    cap_declaration
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_translation_unit(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Any<
    cap_punct<";">,
    cap_class,
    cap_struct,
    cap_union,
    cap_enum,
    cap_template,
    cap_declaration,
    cap_preproc,
    cap_function,
    cap_namespace
    //cap_typedef FIXME
  >;
  // clang-format on

  return CaptureAnon<pattern, CNodeTranslationUnit>::match(ctx, body);
}

//------------------------------------------------------------------------------
