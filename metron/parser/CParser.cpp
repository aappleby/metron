// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Parseroni.hpp"
#include "matcheroni/Utilities.hpp"

#include "CConstants.hpp"
#include "CToken.hpp"
#include "CNode.hpp"
#include "CContext.hpp"
#include "SST.hpp"

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

#if 0
template <StringParam match_name, typename P>
struct TraceToken {

  template<typename node_type>
  inline static void print_match2(TokenSpan span, TokenSpan tail, int width) {

    if (tail.is_valid()) {
      const char* tail_a = tail.begin->text_head();
      print_match(text_a, tail_a, text_b, 0x80FF80, 0xCCCCCC, width);
    } else {
      const char* tail_b = tail.end->text_head();
      print_match(text_a, tail_b, text_b, 0xCCCCCC, 0x8080FF, width);
    }
  }

  TokenSpan match(CContext& ctx, TokenSpan body) {
    matcheroni_assert(body.is_valid());
    if (body.is_empty()) return body.fail();

    auto text = TextSpan(body.begin->begin, (body.end - 1)->end);
    auto name = match_name.str_val;

    int depth = ctx.trace_depth++;

    print_match2(text, text, 40);
    print_trellis(depth, name, "?", 0xCCCCCC);

    //print_bar(ctx.trace_depth++, text, name, "?");
    auto tail = P::match(ctx, body);
    depth = --ctx.trace_depth;

    print_match2(body, tail, 40);

    if (tail.is_valid()) {
      print_trellis(depth, name, "!", 0x80FF80);
      printf("\n");
      utils::print_context(ctx.text_span, ctx, 40);
      printf("\n");
    }
    else {
      print_trellis(depth, name, "X", 0x8080FF);
    }


    //print_bar(--ctx.trace_depth, text, name, tail.is_valid() ? "OK" : "X");

    return tail;
  }
};
#endif

//------------------------------------------------------------------------------

template <typename P>
using comma_separated = Seq<P, Any<Seq<Atom<','>, P>>, Opt<Atom<','>>>;

template <typename P>
using opt_comma_separated = Opt<comma_separated<P>>;

template <StringParam match_name, typename pattern>
using Cap = parseroni::Capture<match_name, pattern, CNode>;

//using Cap = TraceToken<match_name, Capture<match_name, pattern, CNode>>;

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
        Capture<"prefix", match_prefix, CNode>,
        And<match_base>
      >
    >,
    Capture<"base_type", match_base, CNode>,
    Opt<Capture<"suffix", match_suffix, CNode>>
  >;

  return pattern::match(ctx, body);
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

//------------------------------------------------------------------------------

TokenSpan match_preproc(CContext& ctx, TokenSpan body) {
  using pattern = Atom<LEX_PREPROC>;

  auto tail = pattern::match(ctx, body);
  if (tail.is_valid()) {
    std::string s(body.begin->text.begin, (tail.begin - 1)->text.end);

    if (s.find("stdio") != std::string::npos) {
      for (auto t : stdio_typedefs) {
        ctx.type_scope->add_typedef(t);
      }
    }

    if (s.find("stdint") != std::string::npos) {
      for (auto t : stdint_typedefs) {
        ctx.type_scope->add_typedef(t);
      }
    }

    if (s.find("stddef") != std::string::npos) {
      for (auto t : stddef_typedefs) {
        ctx.type_scope->add_typedef(t);
      }
    }

  }
  return tail;
};

//------------------------------------------------------------------------------

TokenSpan match_constant(CContext& ctx, TokenSpan body) {
  using pattern =
  Oneof<
    Capture<"float",  Atom<LEX_FLOAT>, CNode>,
    Capture<"int",    Atom<LEX_INT>, CNode>,
    Capture<"char",   Atom<LEX_CHAR>, CNode>,
    Capture<"string", Some<Atom<LEX_STRING>>, CNode>
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

TokenSpan match_expression_paren(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'('>,
    Capture<"expression", Ref<match_expression>, CNode>,
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
    Capture<"expression", Ref<match_expression>, CNode>,
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
    Capture<"expression", Ref<match_expression>, CNode>,
    Atom<','>,
    Atom<']'>
  >;
  return pattern::match(ctx, body);
};

//------------------------------------------------------------------------------

// clang-format off
using ExpressionPrefixOp =
Oneof<
  Capture<"cast",      Ref<match_prefix_cast>,     CNode>,
  Capture<"preinc",    Ref<match_prefix_op<"++">>, CNode>,
  Capture<"predec",    Ref<match_prefix_op<"--">>, CNode>,
  Capture<"preplus",   Ref<match_prefix_op<"+">>,  CNode>,
  Capture<"preminus",  Ref<match_prefix_op<"-">>,  CNode>,
  Capture<"prebang",   Ref<match_prefix_op<"!">>,  CNode>,
  Capture<"pretilde",  Ref<match_prefix_op<"~">>,  CNode>,
  Capture<"prestar",   Ref<match_prefix_op<"*">>,  CNode>,
  Capture<"preamp",    Ref<match_prefix_op<"&">>,  CNode>
>;
// clang-format on


//----------------------------------------

// clang-format off
using ExpressionCore =
Oneof<
  Capture<"paren",        Ref<match_expression_paren>, CNode>,
  //Capture<"init",         NodeInitializerList>,
  Capture<"braces",       Ref<match_expression_braces>, CNode>,
  Capture<"identifier",   Ref<match_identifier>, CNode>,
  Capture<"constant",     Ref<match_constant>, CNode>
>;
// clang-format on

//----------------------------------------

// clang-format off
using ExpressionSuffixOp =
Oneof<
  //Capture<"initializer", NodeSuffixInitializerList>,  // must be before NodeSuffixBraces
  Capture<"braces",      Ref<match_expression_braces>, CNode>,
  Capture<"paren",       Ref<match_expression_paren>, CNode>,
  Capture<"subscript",   Ref<match_subscript>, CNode>,
  Capture<"postinc",     Ref<match_suffix_op<"++">>, CNode>,
  Capture<"postdec",     Ref<match_suffix_op<"--">>, CNode>
>;
// clang-format on


//----------------------------------------

// clang-format off
using unit_pattern =
Seq<
  Any<
    Capture<"prefix", ExpressionPrefixOp, CNode>
  >,
  ExpressionCore,
  Any<
    Capture<"suffix", ExpressionSuffixOp, CNode>
  >
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
      Capture<"then", comma_separated<Ref<match_expression>>, CNode>
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
    Capture<"unit", unit_pattern, CNode>,
    Any<
      Seq<
        Capture<"op", Ref<match_binary_op2>, CNode>,
        Capture<"unit", unit_pattern, CNode>
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
    Capture<"param", Ref<match_declaration>, CNode>,
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

TokenSpan match_template_args(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'<'>,
    Capture<"arg", Ref<match_expression>, CNode>,
    Atom<','>,
    Atom<'>'>
  >;
  return pattern::match(ctx, body);
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
    Opt<Ref<match_template_args>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_declaration(CContext& ctx, TokenSpan body) {
  using pattern =
  Seq<
    Capture<"type", Ref<match_type>, CNode>,
    Capture<"name", Ref<match_identifier>, CNode>,
    Opt<Seq<
      Atom<'='>,
      Capture<"value", Ref<match_expression>, CNode>
    >>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_parameter_list(CContext& ctx, TokenSpan body) {
  using pattern = DelimitedList<
    Atom<'('>,
    Capture<"param", Ref<match_declaration>, CNode>,
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
    Capture<"access",      Ref<match_access_specifier>, CNode>,
    Capture<"constructor", Ref<match_constructor>, CNode>,
    Capture<"function",    Ref<match_function>, CNode>,
    Capture<"struct",      Ref<match_struct>, CNode>,
    Capture<"union",       Ref<match_union>, CNode>,
    Capture<"template",    Ref<match_template>, CNode>,
    Capture<"class",       Ref<match_class>, CNode>,
    Capture<"enum",        Ref<match_enum>, CNode>,
    Capture<"decl",        Ref<match_declaration>, CNode>
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
    Capture<"name",   Ref<match_identifier>, CNode>,
    Capture<"fields", Ref<match_field_list>, CNode>
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
    Capture<"fields", Opt<Ref<match_field_list>>, CNode>
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
    Capture<"fields", Ref<match_field_list>, CNode>
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
    Ref<class_type_adder>,
    Capture<"body", Opt<Ref<match_field_list>>, CNode>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_template_params(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'<'>,
    Capture<"param", Ref<match_declaration>, CNode>,
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
    Capture<"params", Ref<match_template_params>, CNode>,
    Capture<"class", Ref<match_class>, CNode>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------
// FIXME should probably have a few diffeerent versions instead of all the opts

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
    Capture<"name", Ref<match_identifier>, CNode>,
    Opt<Seq<
      Atom<'='>,
      Capture<"value", Ref<match_expression>, CNode>
    >>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enumerator_list(CContext& ctx, TokenSpan body) {
  using pattern =
  DelimitedList<
    Atom<'{'>,
    Capture<"enumerator", Ref<match_enumerator>, CNode>,
    Atom<','>,
    Atom<'}'>
  >;
  return pattern::match(ctx, body);
}

TokenSpan match_enum(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<match_keyword<"enum">>,
    Opt<Ref<match_keyword<"class">>>,
    Ref<enum_type_adder>,
    Capture<"type", Opt<Seq<Atom<':'>, Ref<match_type>>>, CNode>,
    Capture<"body", Ref<match_enumerator_list>,      CNode>
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
      Capture<"initializer", NodeInitializer>
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
    Capture<"type",   Ref<match_type>, CNode>,
    Capture<"name",   Ref<match_identifier>, CNode>,
    Capture<"params", Ref<match_parameter_list>, CNode>,
    Capture<"body",   Ref<match_compound_statement>, CNode>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_constructor(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Seq<
    Ref<&CContext::match_class_name>,
    Capture<"params", Ref<match_parameter_list>, CNode>,
    Capture<"body",   Ref<match_compound_statement>, CNode>
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
      Capture<"statement", Ref<match_statement>, CNode>,
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
    Capture<
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
    Capture<"condition", Opt<comma_separated<Ref<match_expression>>>, CNode>,
    Atom<';'>,
    Capture<"step", Opt<comma_separated<Ref<match_expression>>>, CNode>,
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
    Capture<"condition", Ref<match_expression_paren>, CNode>,
    Capture<"then", Ref<match_statement>, CNode>,
    Opt<Seq<
      Ref<match_keyword<"else">>,
      Capture<"else", Ref<match_statement>, CNode>
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
      Capture<"value", Ref<match_expression>, CNode>
    >,
    Atom<';'>
  >;
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------

TokenSpan match_case_body(CContext& ctx, TokenSpan body) {
  // clang-format off
  using pattern =
  Capture<
    "body",
    Any<
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
    Capture<"condition", Ref<match_expression>, CNode>,
    Atom<':'>,
    Ref<match_case_body>
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
    Capture<"condition", Ref<match_expression>, CNode>,
    Atom<'{'>,
    Any<
      Capture<"case",    Ref<match_case_statement>, CNode>,
      Capture<"default", Ref<match_default_statement>, CNode>
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
    Ref<match_expression_paren>,
    Capture<"body", Ref<match_statement>, CNode>
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
    Capture<"body", Ref<match_statement>, CNode>,
    Ref<match_keyword<"while">>,
    Ref<match_expression_paren>,
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
    Capture<
      "typedef",
      Oneof<
        Capture<"struct", Ref<match_struct>, CNode>,
        Capture<"union",  Ref<match_union>, CNode>,
        Capture<"class",  Ref<match_class>, CNode>,
        Capture<"enum",   Ref<match_enum>, CNode>,
        Capture<"decl",   Ref<match_declaration>, CNode>
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
    Capture<"class",       Seq<Ref<match_class>,       Atom<';'>>, CNode>,
    Capture<"struct",      Seq<Ref<match_struct>,      Atom<';'>>, CNode>,
    Capture<"union",       Seq<Ref<match_union>,       Atom<';'>>, CNode>,
    Capture<"enum",        Seq<Ref<match_enum>,        Atom<';'>>, CNode>,
    Capture<"typedef",     Seq<Ref<match_typedef>,     Atom<';'>>, CNode>,
    Capture<"declaration", Seq<Ref<match_declaration>, Atom<';'>>, CNode>,

    Capture<"for",      Ref<match_for_statement>, CNode>,
    Capture<"if",       Ref<match_if_statement>, CNode>,
    Capture<"return",   Ref<match_return_statement>, CNode>,
    Capture<"switch",   Ref<match_switch_statement>, CNode>,
    Capture<"dowhile",  Ref<match_dowhile_statement>, CNode>,
    Capture<"while",    Ref<match_while_statement>, CNode>,
    Capture<"compound", Ref<match_compound_statement>, CNode>,
    Capture<"break",    Ref<match_break_statement>, CNode>,
    Capture<"continue", Ref<match_continue_statement>, CNode>,

    // These don't - but they might confuse a keyword with an identifier...
    Capture<"function", Ref<match_function>, CNode>,

    // If declaration is before expression, we parse "x = 1;" as a declaration
    // because it matches a declarator (bare identifier) + initializer list :/
    Capture<"expression",  Seq<Ref<match_expression>,  Atom<';'>>, CNode>,

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
      Capture<"class",       Seq<Ref<match_class>,       Atom<';'>>, CNode>,
      Capture<"struct",      Seq<Ref<match_struct>,      Atom<';'>>, CNode>,
      Capture<"union",       Seq<Ref<match_union>,       Atom<';'>>, CNode>,
      Capture<"enum",        Seq<Ref<match_enum>,        Atom<';'>>, CNode>,
      Capture<"template",    Seq<Ref<match_template>,    Atom<';'>>, CNode>,
      Capture<"declaration", Seq<Ref<match_declaration>, Atom<';'>>, CNode>,

      Capture<"typedef",     Ref<match_typedef>,         CNode>,
      Capture<"preproc",     Ref<match_preproc>,         CNode>,
      Capture<"function",    Ref<match_function>,        CNode>,
      Capture<"namespace",   Ref<match_namespace>,       CNode>,
      Atom<';'>
    >
  >;
  // clang-format on

  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------
