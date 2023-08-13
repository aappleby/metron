// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

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

#include <assert.h>

using namespace matcheroni;
using namespace parseroni;

TokenSpan match_access_specifier   (CContext& ctx, TokenSpan body);
TokenSpan match_class              (CContext& ctx, TokenSpan body);
TokenSpan match_compound_statement (CContext& ctx, TokenSpan body);
TokenSpan match_constructor        (CContext& ctx, TokenSpan body);
TokenSpan match_declaration        (CContext& ctx, TokenSpan body);
TokenSpan match_enum               (CContext& ctx, TokenSpan body);
TokenSpan match_function           (CContext& ctx, TokenSpan body);
TokenSpan match_struct             (CContext& ctx, TokenSpan body);
TokenSpan match_template           (CContext& ctx, TokenSpan body);
TokenSpan match_union              (CContext& ctx, TokenSpan body);
TokenSpan match_parameter_list(CContext& ctx, TokenSpan body);
TokenSpan match_statement(CContext& ctx, TokenSpan body);
TokenSpan match_call_expression(CContext& ctx, TokenSpan body);
TokenSpan match_param_list(CContext& ctx, TokenSpan body);
TokenSpan match_arg_list(CContext& ctx, TokenSpan body);
TokenSpan match_template_args(CContext& ctx, TokenSpan body);

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
        Cap<"prefix", match_prefix, CNode>,
        And<match_base>
      >
    >,
    Cap<"base_type", match_base, CNode>,
    Opt<Cap<"suffix", match_suffix, CNode>>
  >;

  auto tail = pattern::match(ctx, body);
  return tail;
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
    Atom<LEX_IDENTIFIER>,
    Opt<Seq<
      // FIXME we should really handle scoped identifiers better...
      Ref<match_punct<"::">>,
      Ref<match_identifier>
    >>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TextSpan CContext::handle_include(TextSpan body) {
  //printf("CContext::handle_include ");
  //for (auto c = body.begin; c < body.end; c++) putc(*c, stdout);
  //putc('\n', stdout);
  //fflush(stdout);

  if (body.begin[0] == '<') return body.fail();

  std::string path(body.begin + 1, body.end - 1);

  if (path.find("metron_tools.h") != std::string::npos) {
    //LOG_Y("Skipping metron_tools.h\n");
    return body.fail();
  }

  //printf("path %s\n", body.begin);

  //utils::print_span("Include: ", body);

  CSourceFile* include_file = nullptr;
  Err err = repo->load_source(path, &include_file);

  if (!err) {
    //LOG_G("Parse OK\n");

    type_scope->merge(include_file->context.type_scope);
  }

  // type_scope

  /*
  auto full_path = find_file(path);
  if (full_path.empty()) {
    printf("Could not find file %s\n", path.c_str());
    return body.fail();
  }

  std::filesystem::path p(full_path);
  p.remove_filename();
  search_paths.push_back(p);
  auto bookmark = checkpoint();

#if 0
  // parse include here
  {
    auto blob = matcheroni::utils::read(full_path);
    auto source_span = matcheroni::utils::to_span(blob);

    CLexer lexer;
    lexer.lex(source_span);

    TokenSpan tok_span(lexer.tokens.data(), lexer.tokens.data() + lexer.tokens.size());

    CContext sub_context;
    auto tail = sub_context.parse(source_span, tok_span);
    if (!tail.is_valid() || !tail.is_empty()) {
      exit(-1);
    }
    else {
      //exit(0);
    }
  }
#endif

  rewind(bookmark);
  search_paths.pop_back();
  */

  return body.consume();
}

//------------------------------------------------------------------------------

TokenSpan match_preproc(CContext& ctx, TokenSpan body) {
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

TokenSpan match_constant(CContext& ctx, TokenSpan body) {
  using pattern =
  Oneof<
    Cap<"float",   Atom<LEX_FLOAT>, CNode>,
    Cap<"int",     Atom<LEX_INT>, CNode>,
    Cap<"char",    Atom<LEX_CHAR>, CNode>,
    Cap<"string",  Some<Atom<LEX_STRING>>, CNode>,
    Cap<"nullptr", Ref<match_literal<"nullptr">>, CNode>,
    Cap<"true",    Ref<match_literal<"true">>, CNode>,
    Cap<"false",   Ref<match_literal<"false">>, CNode>
  >;
  return pattern::match(ctx, body);
}

using constant = Ref<match_constant>;



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

TokenSpan match_qualifier(CContext& ctx, TokenSpan body) {
  TextSpan span = body.begin->text;
  if (SST<qualifiers>::match(span.begin, span.end)) {
    return body.advance(1);
  }
  else {
    return body.fail();
  }
}

using qualifier = Ref<match_qualifier>;

//------------------------------------------------------------------------------

TokenSpan match_access_specifier(CContext& ctx, TokenSpan body) {
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

using access_specifier = Ref<match_access_specifier>;

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

TokenSpan match_expression(CContext& ctx, TokenSpan body);

TokenSpan match_expression_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Cap<"expression", Ref<match_expression>, CNode>,
    Atom<','>,
    Atom<')'>
  >;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

TokenSpan match_expression_braces(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'{'>,
    Cap<"expression", Ref<match_expression>, CNode>,
    Atom<','>,
    Atom<'}'>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_subscript(CContext& ctx, TokenSpan body) {
  /*
  NodeSuffixSubscript() {
    precedence = 2;
    assoc = 2;
  }
  */

  using pattern =
  DelimitedList<
    Atom<'['>,
    Cap<"expression", Ref<match_expression>, CNode>,
    Atom<','>,
    Atom<']'>
  >;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

// FIXME basic_tock_with_return isn't rewinding after matching call_name

TokenSpan match_call_expression(CContext& ctx, TokenSpan body) {
  using pattern = Seq<
    Cap<"call_name", Ref<match_identifier>, CNode>,
    Opt<Cap<"call_template_args", Ref<match_template_args>, CNode>>,
    Cap<"call_args", Ref<match_arg_list>, CNode>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

// clang-format off
using ExpressionPrefixOp =
Oneof<
  Cap<"cast",      Ref<match_prefix_cast>,     CNode>,
  Cap<"preinc",    Ref<match_prefix_op<"++">>, CNode>,
  Cap<"predec",    Ref<match_prefix_op<"--">>, CNode>,
  Cap<"preplus",   Ref<match_prefix_op<"+">>,  CNode>,
  Cap<"preminus",  Ref<match_prefix_op<"-">>,  CNode>,
  Cap<"prebang",   Ref<match_prefix_op<"!">>,  CNode>,
  Cap<"pretilde",  Ref<match_prefix_op<"~">>,  CNode>,
  Cap<"prestar",   Ref<match_prefix_op<"*">>,  CNode>,
  Cap<"preamp",    Ref<match_prefix_op<"&">>,  CNode>
>;
// clang-format on


//----------------------------------------

// clang-format off
using ExpressionCore =
Oneof<
  Cap<"call_expression", Ref<match_call_expression>, CNode>,
  Cap<"paren",           Ref<match_expression_list>, CNode>,
  //Cap<"init",         NodeInitializerList>,
  Cap<"braces",          Ref<match_expression_braces>, CNode>,
  Cap<"identifier",      Ref<match_identifier>, CNode>,
  Cap<"constant",        Ref<match_constant>, CNode>
>;
// clang-format on

//----------------------------------------

// clang-format off
using ExpressionSuffixOp =
Oneof<
  //Cap<"initializer", NodeSuffixInitializerList>,  // must be before NodeSuffixBraces
  Cap<"braces",      Ref<match_expression_braces>, CNode>,
  Cap<"paren",       Ref<match_expression_list>, CNode>,
  Cap<"subscript",   Ref<match_subscript>, CNode>,
  Cap<"postinc",     Ref<match_suffix_op<"++">>, CNode>,
  Cap<"postdec",     Ref<match_suffix_op<"--">>, CNode>
>;
// clang-format on


//----------------------------------------

// clang-format off
using unit_pattern =
Seq<
  Any<Cap<"prefix", ExpressionPrefixOp, CNode>>,
  ExpressionCore,
  Any<Cap<"suffix", ExpressionSuffixOp, CNode>>
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
      Cap<"then", comma_separated<Ref<match_expression>>, CNode>
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

TokenSpan match_expression(CContext& ctx, TokenSpan body) {

  using pattern =
  Seq<
    Cap<"unit", unit_pattern, CNode>,
    Any<
      Seq<
        Cap<"op", Ref<match_binary_op2>, CNode>,
        Cap<"unit", unit_pattern, CNode>
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
        auto node = new Ref<match_expression>Prefix();
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
      auto node = new Ref<match_expression>Binary();
      node->init(na->tok_a(), nb->tok_b());
    }
    else if (ox->precedence == oy->precedence) {
      matcheroni_assert(ox->assoc == oy->assoc);

      if (ox->assoc == 1) {
        // Left to right
        // "a + b - c" -> "(a + b) - c"
        auto node = new Ref<match_expression>Binary();
        node->init(na->tok_a(), nb->tok_b());
      }
      else if (ox->assoc == -1) {
        // Right to left
        // "a = b = c" -> "a = (b = c)"
        auto node = new Ref<match_expression>Binary();
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

    auto node = new Ref<match_expression>Binary();
    node->init(nb->tok_a(), nc->tok_b());
  }
#endif

#if 0
  if (auto tail = SpanTernaryOp::match(ctx, cursor, b)) {
    auto node = new Ref<match_expression>Ternary();
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

TokenSpan match_param_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Cap<"param", Ref<match_declaration>, CNode>,
    Atom<','>,
    Atom<')'>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_arg_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Cap<"param", Ref<match_expression>, CNode>,
    Atom<','>,
    Atom<')'>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_array_suffix(CContext& ctx, TokenSpan body) {
  using pattern = Seq<Atom<'['>, Ref<match_expression>, Atom<']'>>;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

const CToken* find_matching_delimiter(const CToken* begin, const CToken* end) {
  assert(begin->text.begin[0] == '<');

  for (auto cursor = begin + 1; cursor < end; cursor++) {
    if (cursor->text.begin[0] == '>') return cursor;
    if (cursor->text.begin[0] == '<') cursor = find_matching_delimiter(cursor, end);
    if (!cursor) return cursor;
  }

  return nullptr;
}

TokenSpan match_template_args(CContext& ctx, TokenSpan body) {
  if (body.begin->text.begin[0] != '<') return body.fail();

  auto ldelim = body.begin;
  auto rdelim = find_matching_delimiter(body.begin, body.end);

  if (rdelim == nullptr) return body.fail();

  using pattern =
  comma_separated<
    Cap<"arg", Ref<match_expression>, CNode>
  >;

  auto tail = pattern::match(ctx, TokenSpan(ldelim + 1, rdelim));

  if (tail.is_valid() && tail.is_empty()) {
    return TokenSpan(rdelim + 1, body.end);
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
    Oneof<
      // These have to be NodeIdentifier because "void foo(struct S);"
      // is valid even without the definition of S.
      Seq<Ref<match_keyword<"class">>,  Oneof<Ref<match_identifier>, Ref<&CContext::match_class_name>>>,
      Seq<Ref<match_keyword<"union">>,  Oneof<Ref<match_identifier>, Ref<&CContext::match_union_name>>>,
      Seq<Ref<match_keyword<"struct">>, Oneof<Ref<match_identifier>, Ref<&CContext::match_struct_name>>>,
      Seq<Ref<match_keyword<"enum">>,   Oneof<Ref<match_identifier>, Ref<&CContext::match_enum_name>>>,

      Ref<&CContext::match_class_name>,
      Ref<&CContext::match_struct_name>,
      Ref<&CContext::match_union_name>,
      Ref<&CContext::match_enum_name>,

      Ref<match_builtin_type>,
      Ref<&CContext::match_typedef_name>
    >,
    Opt<Cap<"template_args", Ref<match_template_args>, CNode>>,
    Opt<Seq<
      Ref<match_punct<"::">>,
      Ref<match_identifier>
    >>,
    Any<Cap<"star", Atom<'*'>, CNode>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_decl_prefix(CContext& ctx, TokenSpan body) {
  using pattern =
  Oneof<
    Cap<"static", Ref<match_keyword<"static">>, CNode>,
    Cap<"const",  Ref<match_keyword<"const">>,  CNode>
  >;
  return pattern::match(ctx, body);
 }

//------------------------------------------------------------------------------

TokenSpan match_declaration(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Any<Ref<match_decl_prefix>>,
    Cap<"decl_type", Ref<match_type>, CNode>,
    Cap<"decl_name", Ref<match_identifier>, CNode>,
    Any<Cap<"decl_array", Ref<match_array_suffix>, CNode>>,
    Opt<Seq<
      Atom<'='>,
      Cap<"decl_value", Ref<match_expression>, CNode>
    >>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_parameter_list(CContext& ctx, TokenSpan body) {
  using pattern = DelimitedList<
    Atom<'('>,
    Cap<"param", Ref<match_declaration>, CNode>,
    Atom<','>,
    Atom<')'>
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
    Cap<"access",      Ref<match_access_specifier>, CNode>,
    Cap<"constructor", Ref<match_constructor>, CNode>,
    Cap<"function",    Ref<match_function>, CNode>,
    Cap<"struct",      Ref<match_struct>, CNode>,
    Cap<"union",       Ref<match_union>, CNode>,
    Cap<"template",    Ref<match_template>, CNode>,
    Cap<"class",       Ref<match_class>, CNode>,
    Cap<"enum",        Ref<match_enum>, CNode>,
    Cap<"decl",        Ref<match_declaration>, CNode>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_field_list(CContext& ctx, TokenSpan body) {
  using pattern = DelimitedBlock< Atom<'{'>, Ref<match_field>, Atom<'}'>>;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_namespace(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Ref<match_keyword<"namespace">>,
    Cap<"name",   Ref<match_identifier>, CNode>,
    Cap<"fields", Ref<match_field_list>, CNode>
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
    Ref<match_keyword<"struct">>,
    Ref<struct_type_adder>,
    Cap<"fields", Opt<Ref<match_field_list>>, CNode>
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
    Ref<match_keyword<"union">>,
    Ref<union_type_adder>,
    Cap<"fields", Ref<match_field_list>, CNode>
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
    Ref<match_keyword<"class">>,
    Cap<"name", Ref<class_type_adder>, CNode>,
    Cap<"body", Opt<Ref<match_field_list>>, CNode>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_template_params(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'<'>,
    Cap<"param", Ref<match_declaration>, CNode>,
    Atom<','>,
    Atom<'>'>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_template(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Ref<match_keyword<"template">>,
    Cap<"template_params", Ref<match_template_params>, CNode>,
    Oneof<
      Cap<"template_class",    Ref<match_class>,    CNodeClass>,
      Cap<"template_function", Ref<match_function>, CNodeFunction>
    >
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
    Cap<"name", Ref<match_identifier>, CNode>,
    Opt<Seq<
      Atom<'='>,
      Cap<"value", Ref<match_expression>, CNode>
    >>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enumerator_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'{'>,
    Cap<"enumerator", Ref<match_enumerator>, CNode>,
    Atom<','>,
    Atom<'}'>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enum(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Ref<match_keyword<"enum">>,
    Opt<Ref<match_keyword<"class">>>,
    Opt<Capture<"name", Ref<enum_type_adder>, CNode>>,
    Cap<"type", Opt<Seq<Atom<':'>, Ref<match_type>>>, CNode>,
    Cap<"body", Ref<match_enumerator_list>,      CNode>,
    Opt<comma_separated<Cap<"decl", Ref<match_expression>, CNode>>>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

#if 0

struct NodeDesignation : public CNode, public PatternWrapper<NodeDesignation> {
  // clang-format off
  using pattern =
  Some<
    Seq<Atom<'['>, NodeConstant, Atom<']'>>,
    Seq<Atom<'['>, NodeIdentifier::pattern, Atom<']'>>,
    Seq<Atom<'.'>, NodeIdentifier::pattern>
  >;
  // clang-format on
};

struct NodeInitializerList : public CNode, public PatternWrapper<NodeInitializerList> {
  using pattern = DelimitedList<
      Atom<'{'>,
      Seq<Opt<Seq<NodeDesignation, Atom<'='>>,
              Seq<NodeIdentifier::pattern, Atom<':'>>  // This isn't in the C grammar but
                                              // compndlit-1.c uses it?
              >,
          NodeInitializer>,
      Atom<','>, Atom<'}'>>;
};

struct NodeSuffixInitializerList : public CNode, public PatternWrapper<NodeSuffixInitializerList> {
  NodeSuffixInitializerList() {
    precedence = 2;
    assoc = 2;
  }

  // clang-format off
  using pattern =
  DelimitedList<
    Atom<'{'>,
    Seq<
      Opt<
        Seq<NodeDesignation, Atom<'='>>,
        Seq<NodeIdentifier::pattern, Atom<':'>>  // This isn't in the C grammar but compndlit-1.c uses it?
      >,
      Cap<"initializer", NodeInitializer>
    >,
    Atom<','>,
    Atom<'}'>
  >;
  // clang-format on
};

struct NodeInitializer : public CNode, public PatternWrapper<NodeInitializer> {
  using pattern = Oneof<NodeInitializerList, Ref<match_expression>>;
};

#endif

//------------------------------------------------------------------------------

TokenSpan match_function(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Cap<"type",   Ref<match_type>, CNode>,
    Cap<"name",   Ref<match_identifier>, CNode>,
    Cap<"params", Ref<match_parameter_list>, CNode>,
    Opt<Cap<"const",  Ref<match_keyword<"const">>, CNode>>,
    Cap<"body",   Ref<match_compound_statement>, CNode>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_constructor(CContext& ctx, TokenSpan body) {
  // clang-format off

  using initializers = Seq<
    Atom<':'>,
    cookbook::comma_separated<
      Seq<
        Ref<match_identifier>,
        Ref<match_expression_list>
      >
    >
  >;

  using pattern =
  Seq<
    Ref<&CContext::match_class_name>,
    Cap<"params", Ref<match_parameter_list>, CNode>,
    Cap<"init",   Opt<initializers>, CNode>,
    Cap<"body",   Ref<match_compound_statement>, CNode>
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

TokenSpan match_compound_statement(CContext& ctx, TokenSpan body) {
  using pattern =
  Ref<change_scope<
    DelimitedBlock<
      Atom<'{'>,
      Cap<"statement", Ref<match_statement>, CNode>,
      Atom<'}'>
    >
  >>;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

TokenSpan match_for_statement(CContext& ctx, TokenSpan body) {
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
        Seq<comma_separated<Ref<match_expression>>,  Atom<';'>>,
        Seq<comma_separated<Ref<match_declaration>>, Atom<';'>>,
        Atom<';'>
      >,
      CNode
    >,
    Cap<"condition", Opt<comma_separated<Ref<match_expression>>>, CNode>,
    Atom<';'>,
    Cap<"step", Opt<comma_separated<Ref<match_expression>>>, CNode>,
    Atom<')'>,
    Oneof<
      Ref<match_compound_statement>,
      Ref<match_statement>
    >
  >;
  // clang-format on
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

TokenSpan match_if_statement(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Ref<match_keyword<"if">>,
    Cap<"condition", Ref<match_expression_list>, CNode>,
    Cap<"then", Ref<match_statement>, CNode>,
    Opt<Seq<
      Ref<match_keyword<"else">>,
      Cap<"else", Ref<match_statement>, CNode>
    >>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_return_statement(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Ref<match_keyword<"return">>,
    Opt<
      Cap<"value", Ref<match_expression>, CNode>
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
        Ref<match_statement>
      >
    >,
    CNode
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

TokenSpan match_case_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"case">>,
    Cap<"condition", Ref<match_expression>, CNode>,
    Atom<':'>,
    Opt<Ref<match_case_body>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

TokenSpan match_default_statement(CContext& ctx, TokenSpan body) {
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

TokenSpan match_switch_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"switch">>,
    Cap<"condition", Ref<match_expression>, CNode>,
    Atom<'{'>,
    Any<
      Cap<"case",    Ref<match_case_statement>, CNode>,
      Cap<"default", Ref<match_default_statement>, CNode>
    >,
    Atom<'}'>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_while_statement(CContext& ctx, TokenSpan body) {
  // clang-format on
  using pattern =
  Seq<
    Ref<match_keyword<"while">>,
    Ref<match_expression_list>,
    Cap<"body", Ref<match_statement>, CNode>
  >;
  // clang-format off
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_dowhile_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"do">>,
    Cap<"body", Ref<match_statement>, CNode>,
    Ref<match_keyword<"while">>,
    Ref<match_expression_list>,
    Atom<';'>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_break_statement(CContext& ctx, TokenSpan body) {
  using pattern = Seq<Ref<match_keyword<"break">>, Atom<';'>>;
  return pattern::match(ctx, body);
}

TokenSpan match_continue_statement(CContext& ctx, TokenSpan body) {
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

// FIXME this is probably broken since we took the decls off the match_struct and stuff

TokenSpan match_typedef(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"typedef">>,
    Cap<
      "typedef",
      Oneof<
        Cap<"struct", Ref<match_struct>, CNode>,
        Cap<"union",  Ref<match_union>, CNode>,
        Cap<"class",  Ref<match_class>, CNode>,
        Cap<"enum",   Ref<match_enum>, CNode>,
        Cap<"decl",   Ref<match_declaration>, CNode>
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

//------------------------------------------------------------------------------

TokenSpan match_statement(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Oneof<
    // All of these have keywords or something first
    Cap<"class",       Seq<Ref<match_class>,       Atom<';'>>, CNode>,
    Cap<"struct",      Seq<Ref<match_struct>,      Atom<';'>>, CNode>,
    Cap<"union",       Seq<Ref<match_union>,       Atom<';'>>, CNode>,
    Cap<"enum",        Seq<Ref<match_enum>,        Atom<';'>>, CNode>,
    Cap<"typedef",     Seq<Ref<match_typedef>,     Atom<';'>>, CNode>,
    Cap<"declaration", Seq<Ref<match_declaration>, Atom<';'>>, CNode>,

    Cap<"for",      Ref<match_for_statement>, CNode>,
    Cap<"if",       Ref<match_if_statement>, CNode>,
    Cap<"return",   Ref<match_return_statement>, CNode>,
    Cap<"switch",   Ref<match_switch_statement>, CNode>,
    Cap<"dowhile",  Ref<match_dowhile_statement>, CNode>,
    Cap<"while",    Ref<match_while_statement>, CNode>,
    Cap<"compound", Ref<match_compound_statement>, CNode>,
    Cap<"break",    Ref<match_break_statement>, CNode>,
    Cap<"continue", Ref<match_continue_statement>, CNode>,

    // These don't - but they might confuse a keyword with an identifier...
    Cap<"function", Ref<match_function>, CNode>,

    // If declaration is before expression, we parse "x = 1;" as a declaration
    // because it matches a declarator (bare identifier) + initializer list :/
    Cap<"expression",  Seq<Ref<match_expression>,  Atom<';'>>, CNode>,

    // Extra semicolons
    Atom<';'>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_translation_unit(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Any<
    Oneof<
      Cap<"class",       Seq<Ref<match_class>,       Atom<';'>>, CNodeClass>,
      Cap<"struct",      Seq<Ref<match_struct>,      Atom<';'>>, CNodeStruct>,
      Cap<"union",       Seq<Ref<match_union>,       Atom<';'>>, CNodeUnion>,
      Cap<"enum",        Seq<Ref<match_enum>,        Atom<';'>>, CNodeEnum>,
      Cap<"template",    Seq<Ref<match_template>,    Atom<';'>>, CNodeTemplate>,
      Cap<"declaration", Seq<Ref<match_declaration>, Atom<';'>>, CNodeDeclaration>,

      Cap<"typedef",     Ref<match_typedef>,         CNodeTypedef>,
      Cap<"preproc",     Ref<match_preproc>,         CNodePreproc>,
      Cap<"function",    Ref<match_function>,        CNodeFunction>,
      Cap<"namespace",   Ref<match_namespace>,       CNodeNamespace>,
      Atom<';'>
    >
  >;
  // clang-format on

  return Cap<"translation_unit", pattern, CNode>::match(ctx, body);
}

//------------------------------------------------------------------------------
