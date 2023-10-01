// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CLexer.hpp"

#include "matcheroni/Cookbook.hpp"
#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "metron/CToken.hpp"
#include "metron/SST.hpp"

using namespace matcheroni;

template <typename M>
using ticked = Seq<Opt<Atom<'\''>>, M>;

// clang-format off
CToken    next_lexeme      (TextMatchContext& ctx, TextSpan body);
TextSpan  match_space      (TextMatchContext& ctx, TextSpan body);
TextSpan  match_newline    (TextMatchContext& ctx, TextSpan body);
TextSpan  match_string     (TextMatchContext& ctx, TextSpan body);
TextSpan  match_char       (TextMatchContext& ctx, TextSpan body);
TextSpan  match_keyword    (TextMatchContext& ctx, TextSpan body);
TextSpan  match_identifier (TextMatchContext& ctx, TextSpan body);
TextSpan  match_comment    (TextMatchContext& ctx, TextSpan body);
TextSpan  match_preproc    (TextMatchContext& ctx, TextSpan body);
TextSpan  match_float      (TextMatchContext& ctx, TextSpan body);
TextSpan  match_int        (TextMatchContext& ctx, TextSpan body);
TextSpan  match_punct      (TextMatchContext& ctx, TextSpan body);
TextSpan  match_splice     (TextMatchContext& ctx, TextSpan body);
TextSpan  match_formfeed   (TextMatchContext& ctx, TextSpan body);
TextSpan  match_eof        (TextMatchContext& ctx, TextSpan body);
// clang-format on

//------------------------------------------------------------------------------

bool CLexer::lex(const std::string& source, std::vector<Lexeme>& out_lexemes) {
  TextSpan source_span = utils::to_span(source);

  out_lexemes.clear();
  out_lexemes.reserve(65536);
  out_lexemes.push_back(Lexeme(LEX_BOF, source_span.begin, source_span.begin));

  current_row = 0;
  current_col = 0;
  in_indent = true;

  TextMatchContext ctx;
  while (source_span.is_valid()) {
    auto lex = next_lexeme(ctx, source_span);
    lex.row = current_row;
    lex.col = current_col;
    lex.indent = current_indent;
    out_lexemes.push_back(lex);
    if (lex.type == LEX_INVALID) {
      return false;
    }
    if (lex.type == LEX_EOF) break;

    while(source_span.begin < lex.text_end) {
      if (*source_span.begin == '\n') {
        current_indent = 0;
        current_col = 0;
        current_row++;
        in_indent = true;
      }
      else {
        if (in_indent) {
          if (*source_span.begin == ' ') {
            current_indent++;
          }
          else {
            in_indent = false;
          }
        }
        current_col++;
      }

      source_span.begin++;
    }
  }

  return true;
}

//------------------------------------------------------------------------------

Lexeme CLexer::next_lexeme(TextMatchContext& ctx, TextSpan body) {
  if (auto tail = match_space(ctx, body)  ) return Lexeme(LEX_SPACE,   body.begin, tail.begin);
  if (auto tail = match_newline(ctx, body)) return Lexeme(LEX_NEWLINE, body.begin, tail.begin);
  if (auto tail = match_string(ctx, body) ) return Lexeme(LEX_STRING,  body.begin, tail.begin);

  // Match char needs to come before match identifier because of its possible
  // L'_' prefix...
  if (auto tail = match_char(ctx, body)   ) return Lexeme(LEX_CHAR, body.begin, tail.begin);

  if (auto tail = match_identifier(ctx, body)) {
    if (SST<c_keywords>::match(body.begin, tail.begin)) {
      return Lexeme(LEX_KEYWORD, body.begin, tail.begin);
    } else {
      return Lexeme(LEX_IDENTIFIER, body.begin, tail.begin);
    }
  }

  if (auto tail = match_comment(ctx, body) ) return Lexeme(LEX_COMMENT,  body.begin, tail.begin);
  if (auto tail = match_preproc(ctx, body) ) return Lexeme(LEX_PREPROC,  body.begin, tail.begin);
  if (auto tail = match_float(ctx, body)   ) return Lexeme(LEX_FLOAT,    body.begin, tail.begin);
  if (auto tail = match_int(ctx, body)     ) return Lexeme(LEX_INT,      body.begin, tail.begin);
  if (auto tail = match_punct(ctx, body)   ) return Lexeme(LEX_PUNCT,    body.begin, tail.begin);
  if (auto tail = match_splice(ctx, body)  ) return Lexeme(LEX_SPLICE,   body.begin, tail.begin);
  if (auto tail = match_formfeed(ctx, body)) return Lexeme(LEX_FORMFEED, body.begin, tail.begin);
  if (auto tail = match_eof(ctx, body)     ) return Lexeme(LEX_EOF,      body.begin, tail.begin);
  if (auto tail = match_string(ctx, body)  ) return Lexeme(LEX_STRING,   body.begin, tail.begin);

  return Lexeme(LEX_INVALID, nullptr, body.begin);
}

//------------------------------------------------------------------------------
// Misc helpers

TextSpan match_eof(TextMatchContext& ctx, TextSpan body) {
  if (body.is_empty()) return body;
  if (*body.begin == 0) return TextSpan(body.begin, body.begin);
  return body.fail();
}

TextSpan match_formfeed(TextMatchContext& ctx, TextSpan body) {
  return Atom<'\f'>::match(ctx, body);
}

TextSpan match_space(TextMatchContext& ctx, TextSpan body) {
  using ws = Atom<' ', '\t'>;
  using pattern = Some<ws>;
  return pattern::match(ctx, body);
}

TextSpan match_newline(TextMatchContext& ctx, TextSpan body) {
  using pattern = Seq<Opt<Atom<'\r'>>, Atom<'\n'>>;
  auto tail = pattern::match(ctx, body);
  return tail;
}

//------------------------------------------------------------------------------
// 6.4.4.1 Integer constants

TextSpan match_int(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using digit                = Range<'0', '9'>;
  using nonzero_digit        = Range<'1', '9'>;

  using decimal_constant     = Seq<nonzero_digit, Any<ticked<digit>>>;

  using hexadecimal_prefix         = Oneof<Lit<"0x">, Lit<"0X">>;
  using hexadecimal_digit          = Range<'0','9','a','f','A','F'>;
  using hexadecimal_digit_sequence = Seq<hexadecimal_digit, Any<ticked<hexadecimal_digit>>>;
  using hexadecimal_constant       = Seq<hexadecimal_prefix, hexadecimal_digit_sequence>;

  using binary_prefix         = Oneof<Lit<"0b">, Lit<"0B">>;
  using binary_digit          = Atom<'0','1'>;
  using binary_digit_sequence = Seq<binary_digit, Any<ticked<binary_digit>>>;
  using binary_constant       = Seq<binary_prefix, binary_digit_sequence>;

  using octal_digit        = Range<'0', '7'>;
  using octal_constant     = Seq<Atom<'0'>, Any<ticked<octal_digit>>>;

  using unsigned_suffix        = Atom<'u', 'U'>;
  using long_suffix            = Atom<'l', 'L'>;
  using long_long_suffix       = Oneof<Lit<"ll">, Lit<"LL">>;
  using bit_precise_int_suffix = Oneof<Lit<"wb">, Lit<"WB">>;

  // This is a little odd because we have to match in longest-suffix-first order
  // to ensure we capture the entire suffix
  using integer_suffix = Oneof<
    Seq<unsigned_suffix,  long_long_suffix>,
    Seq<unsigned_suffix,  long_suffix>,
    Seq<unsigned_suffix,  bit_precise_int_suffix>,
    Seq<unsigned_suffix>,

    Seq<long_long_suffix,       Opt<unsigned_suffix>>,
    Seq<long_suffix,            Opt<unsigned_suffix>>,
    Seq<bit_precise_int_suffix, Opt<unsigned_suffix>>
  >;

  // GCC allows i or j in addition to the normal suffixes for complex-ified types :/...
  using complex_suffix = Atom<'i', 'j'>;

  // Octal has to be _after_ bin/hex so we don't prematurely match the prefix
  using integer_constant =
  Seq<
    Oneof<
      decimal_constant,
      hexadecimal_constant,
      binary_constant,
      octal_constant
    >,
    Seq<
      Opt<complex_suffix>,
      Opt<integer_suffix>,
      Opt<complex_suffix>
    >
  >;

  return integer_constant::match(ctx, body);
  // clang-format on
}

//------------------------------------------------------------------------------
// 6.4.3 Universal character names

TextSpan match_universal_character_name(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using n_char = NotAtom<'}','\n'>;
  using n_char_sequence = Some<n_char>;
  using named_universal_character = Seq<Lit<"\\N{">, n_char_sequence, Lit<"}">>;

  using hexadecimal_digit = Range<'0','9','a','f','A','F'>;
  using hex_quad = Rep<4, hexadecimal_digit>;

  using universal_character_name = Oneof<
    Seq< Lit<"\\u">, hex_quad >,
    Seq< Lit<"\\U">, hex_quad, hex_quad >,
    Seq< Lit<"\\u{">, Any<hexadecimal_digit>, Lit<"}">>,
    named_universal_character
  >;
  // clang-format on

  return universal_character_name::match(ctx, body);
}

//------------------------------------------------------------------------------
// Basic UTF8 support

TextSpan match_utf8(TextMatchContext& ctx, TextSpan body) {
  // matching 1-byte utf breaks things in match_identifier
  using utf8_char =
  Oneof<
    cookbook::utf8_twobyte,
    cookbook::utf8_threebyte,
    cookbook::utf8_fourbyte
  >;
  //using utf8_char = Oneof<utf8_onebyte, utf8_twobyte, utf8_threebyte, utf8_fourbyte>;

  return utf8_char::match(ctx, body);
}

TextSpan match_utf8_bom(TextMatchContext& ctx, TextSpan body) {
  return cookbook::utf8_bom::match(ctx, body);
}

//------------------------------------------------------------------------------
// 6.4.2 Identifiers - GCC allows dollar signs in identifiers?

TextSpan match_identifier(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using digit = Range<'0', '9'>;

  // Not sure if this should be in here
  using latin1_ext = Range<128,255>;

  using nondigit = Oneof<
    Range<'a', 'z'>,
    Range<'A', 'Z'>,
    Atom<'_'>,
    Atom<'$'>,
    Ref<match_universal_character_name>,
    latin1_ext, // One of the GCC test files requires this
    Ref<match_utf8>  // Lots of GCC test files for unicode
  >;

  using identifier = Seq<nondigit, Any<digit, nondigit>>;
  // clang-format on

  return identifier::match(ctx, body);
}

//------------------------------------------------------------------------------
// 6.4.4.2 Float constants

TextSpan match_float(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using float_suffix = Oneof<
    Atom<'f'>, Atom<'l'>, Atom<'F'>, Atom<'L'>,
    // Decimal floats, GCC thing
    Lit<"df">, Lit<"dd">, Lit<"dl">,
    Lit<"DF">, Lit<"DD">, Lit<"DL">
  >;

  using digit = Range<'0', '9'>;

  using digit_sequence = Seq<digit, Any<ticked<digit>>>;

  using fractional_constant = Oneof<
    Seq<Opt<digit_sequence>, Atom<'.'>, digit_sequence>,
    Seq<digit_sequence, Atom<'.'>>
  >;

  using sign = Atom<'+','-'>;

  using hexadecimal_digit          = Range<'0','9','a','f','A','F'>;
  using hexadecimal_digit_sequence = Seq<hexadecimal_digit, Any<ticked<hexadecimal_digit>>>;
  using hexadecimal_fractional_constant = Oneof<
    Seq<Opt<hexadecimal_digit_sequence>, Atom<'.'>, hexadecimal_digit_sequence>,
    Seq<hexadecimal_digit_sequence, Atom<'.'>>
  >;

  using exponent_part        = Seq<Atom<'e', 'E'>, Opt<sign>, digit_sequence>;
  using binary_exponent_part = Seq<Atom<'p', 'P'>, Opt<sign>, digit_sequence>;

  // GCC allows i or j in addition to the normal suffixes for complex-ified types :/...
  using complex_suffix = Atom<'i', 'j'>;

  using decimal_float_constant = Oneof<
    Seq< fractional_constant, Opt<exponent_part>, Opt<complex_suffix>, Opt<float_suffix>, Opt<complex_suffix> >,
    Seq< digit_sequence, exponent_part,           Opt<complex_suffix>, Opt<float_suffix>, Opt<complex_suffix> >
  >;

  using hexadecimal_prefix         = Oneof<Lit<"0x">, Lit<"0X">>;

  using hexadecimal_float_constant = Seq<
    hexadecimal_prefix,
    Oneof<hexadecimal_fractional_constant, hexadecimal_digit_sequence>,
    binary_exponent_part,
    Opt<complex_suffix>,
    Opt<float_suffix>,
    Opt<complex_suffix>
  >;

  using float_constant = Oneof<
    decimal_float_constant,
    hexadecimal_float_constant
  >;
  // clang-format on

  return float_constant::match(ctx, body);
}

//------------------------------------------------------------------------------
// Escape sequences

TextSpan match_escape_sequence(TextMatchContext& ctx, TextSpan body) {
  // This is what's in the spec...
  // using simple_escape_sequence      = Seq<Atom<'\\'>,
  // Charset<"'\"?\\abfnrtv">>;

  // ...but GCC adds \e and \E, and '\(' '\{' '\[' '\%' are warnings but allowed

  // clang-format off
  using simple_escape_sequence      = Seq<Atom<'\\'>, Charset<"'\"?\\abfnrtveE({[%">>;

  using octal_digit = Range<'0', '7'>;
  using octal_escape_sequence = Oneof<
    Seq<Atom<'\\'>, octal_digit, Opt<octal_digit>, Opt<octal_digit>>,
    Seq<Lit<"\\o{">, Any<octal_digit>, Lit<"}">>
  >;

  using hexadecimal_digit = Range<'0','9','a','f','A','F'>;
  using hexadecimal_escape_sequence = Oneof<
    Seq<Lit<"\\x">, Some<hexadecimal_digit>>,
    Seq<Lit<"\\x{">, Any<hexadecimal_digit>, Lit<"}">>
  >;

  using escape_sequence = Oneof<
    simple_escape_sequence,
    octal_escape_sequence,
    hexadecimal_escape_sequence,
    Ref<match_universal_character_name>
  >;
  // clang-format on

  return escape_sequence::match(ctx, body);
}

//------------------------------------------------------------------------------
// 6.4.4.4 Character constants

TextSpan match_char(TextMatchContext& ctx, TextSpan body) {
  // Multi-character character literals are allowed by spec, but their meaning
  // is implementation-defined...

  // clang-format off
  using c_char             = Oneof<Ref<match_escape_sequence>, NotAtom<'\'', '\\', '\n'>>;
  //using c_char_sequence    = Some<c_char>;

  using encoding_prefix    = Oneof<Lit<"u8">, Atom<'u', 'U', 'L'>>; // u8 must go first

  // The spec disallows empty character constants, but...
  //using character_constant = Seq< Opt<encoding_prefix>, Atom<'\''>, c_char_sequence, Atom<'\''> >;

  // ...in GCC they're only a warning.
  using character_constant = Seq< Opt<encoding_prefix>, Atom<'\''>, Any<c_char>, Atom<'\''> >;
  // clang-format on

  return character_constant::match(ctx, body);
}

//------------------------------------------------------------------------------
// 6.4.5 String literals

TextSpan match_cooked_string_literal(TextMatchContext& ctx, TextSpan body) {
  // Note, we add splices here since we're matching before preproccessing.

  // clang-format off
  using s_char          = Oneof<Ref<match_splice>, Ref<match_escape_sequence>, NotAtom<'"', '\\', '\n'>>;
  using s_char_sequence = Some<s_char>;
  using encoding_prefix = Oneof<Lit<"u8">, Atom<'u', 'U', 'L'>>; // u8 must go first
  using string_literal  = Seq<Opt<encoding_prefix>, Atom<'"'>, Opt<s_char_sequence>, Atom<'"'>>;
  // clang-format on

  return string_literal::match(ctx, body);
}

//----------------------------------------
// Raw string literals from the C++ spec

TextSpan match_raw_string_literal(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using encoding_prefix    = Oneof<Lit<"u8">, Atom<'u', 'U', 'L'>>; // u8 must go first

  // We ignore backslash in d_char for similar splice-related reasons
  //using d_char          = NotAtom<' ', '(', ')', '\\', '\t', '\v', '\f', '\n'>;
  using d_char          = NotAtom<' ', '(', ')', '\t', '\v', '\f', '\n'>;
  using d_char_sequence = Some<d_char>;
  using backref_type    = Opt<d_char_sequence>;

  using r_terminator =
  Seq<
    Atom<')'>,
    MatchBackref<"raw_delim", char, backref_type>,
    Atom<'"'>
  >;

  using r_char          = Seq<Not<r_terminator>, AnyAtom>;
  using r_char_sequence = Some<r_char>;

  using raw_string_literal =
  Seq<
    Opt<encoding_prefix>,
    Atom<'R'>,
    Atom<'"'>,
    StoreBackref<"raw_delim", char, backref_type>,
    Atom<'('>,
    Opt<r_char_sequence>,
    Atom<')'>,
    MatchBackref<"raw_delim", char, backref_type>,
    Atom<'"'>
  >;
  // clang-format on

  return raw_string_literal::match(ctx, body);
}

//----------------------------------------

TextSpan match_string(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using any_string = Oneof<
    Ref<match_cooked_string_literal>,
    Ref<match_raw_string_literal>
  >;
  // clang-format on

  return any_string::match(ctx, body);
}

//------------------------------------------------------------------------------
// 6.4.6 Punctuators

TextSpan match_punct(TextMatchContext& ctx, TextSpan body) {
  // We're just gonna match these one punct at a time
  using punctuator = Charset<"-,;:!?.()[]{}*/&#%^+<=>|~">;
  return punctuator::match(ctx, body);
}

//------------------------------------------------------------------------------
// 6.4.9 Comments

TextSpan match_oneline_comment(TextMatchContext& ctx, TextSpan body) {
  // Single-line comments
  using slc = Seq<Lit<"//">, Until<EOL>>;
  return slc::match(ctx, body);
}

TextSpan match_multiline_comment(TextMatchContext& ctx, TextSpan body) {
  // Multi-line non-nested comments
  using mlc_ldelim = Lit<"/*">;
  using mlc_rdelim = Lit<"*/">;
  using mlc = Seq<mlc_ldelim, Until<mlc_rdelim>, mlc_rdelim>;
  return mlc::match(ctx, body);
}

TextSpan match_comment(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using comment =
  Oneof<
    Ref<match_oneline_comment>,
    Ref<match_multiline_comment>
  >;
  // clang-format on

  return comment::match(ctx, body);
}

//------------------------------------------------------------------------------
// 5.1.1.2 : Lines ending in a backslash and a newline get spliced together
// with the following line.

TextSpan match_splice(TextMatchContext& ctx, TextSpan body) {

  // According to GCC it's only a warning to have whitespace between the
  // backslash and the newline... and apparently \r\n is ok too?

  // clang-format off
  using splice = Seq<
    Atom<'\\'>,
    Any<Atom<' ','\t'>>,
    Opt<Atom<'\r'>>,
    Any<Atom<' ','\t'>>,
    Atom<'\n'>
  >;
  // clang-format on

  return splice::match(ctx, body);
}

//------------------------------------------------------------------------------

TextSpan match_preproc(TextMatchContext& ctx, TextSpan body) {
  // clang-format off
  using pattern = Seq<
    Atom<'#'>,
    Some<Range<'a','z','A','Z'>>
  >;
  // clang-format on
  return pattern::match(ctx, body);
}

//------------------------------------------------------------------------------