#include "NewLexer.hpp"

#include "main/main.h"
#include "metrolib/core/Log.h"
#include "matcheroni/Utilities.hpp"

using namespace matcheroni;

//------------------------------------------------------------------------------

int test_newlex(Options& opts) {
  LOG_R("Testing new lexer\n");

  std::string src_blob = utils::read(opts.src_name);

  printf("%s\n", src_blob.c_str());

  LOG("");
  return 0;
}

//------------------------------------------------------------------------------

template <typename M>
using ticked       = Seq<Opt<Atom<'\''>>, M>;

using dec_prefix   = Range<'1', '9'>;
using dec_digit    = Range<'0', '9'>;
using dec_digits   = Seq<dec_digit, Any<ticked<dec_digit>>>;
using dec_const    = Seq<dec_prefix, Any<ticked<dec_digit>>>;

using hex_prefix   = Oneof<Lit<"0x">, Lit<"0X">>;
using hex_digit    = Ranges<'0','9','a','f','A','F'>;
using hex_digits   = Seq<hex_digit, Any<ticked<hex_digit>>>;
using hex_const    = Seq<hex_prefix, hex_digits>;

using bin_prefix   = Oneof<Lit<"0b">, Lit<"0B">>;
using bin_digit    = Atoms<'0','1'>;
using bin_digits   = Seq<bin_digit, Any<ticked<bin_digit>>>;
using bin_const    = Seq<bin_prefix, bin_digits>;

using int_const    = Oneof<dec_const, hex_const, bin_const>;

using sign         = Atoms<'+','-'>;
using float_exp    = Seq<Atoms<'e', 'E'>, Opt<sign>, Some<dec_digit>>;
using float_suffix = Atom<'f'>;
using float_const  = Seq<dec_const, Atom<'.'>, dec_digits, Opt<float_exp>, Opt<float_suffix>>;

using nondigit     = Ranges<'a', 'z', 'A', 'Z', '_', '_'>;
using identifier   = Seq<nondigit, Any<dec_digit, nondigit>>;
using ws           = Some<Atoms<' ', '\t'>>;
using preproc      = Seq<Atom<'#'>, identifier>;
using splice       = Seq<Atom<'\\'>, Atom<'\n'>>;
using line         = Seq<Any<splice, NotAtom<'\n'>>, Atom<'\n'>>;
using slc          = Seq<Lit<"//">, Until<EOL>>;
using mlc_ldelim   = Lit<"/*">;
using mlc_rdelim   = Lit<"*/">;
using mlc          = Seq<mlc_ldelim, Until<mlc_rdelim>, mlc_rdelim>;
using newline      = Seq<Any<Atom<'\r'>>, Atom<'\n'>>;

using punct = Oneof<
  Lit<"...">, Lit<"<<=">, Lit<">>=">,

  Lit<"--">, Lit<"-=">, Lit<"->">, Lit<"!=">,
  Lit<"*=">, Lit<"/=">, Lit<"&&">, Lit<"&=">,
  Lit<"##">, Lit<"%=">, Lit<"^=">, Lit<"++">,
  Lit<"+=">, Lit<"<<">, Lit<"<=">, Lit<"==">,
  Lit<">=">, Lit<">>">, Lit<"|=">, Lit<"||">,
  Lit<"::">, // This isn't in the C spec, but we need it because C++

  Lit<"-">, Lit<",">, Lit<";">, Lit<":">,
  Lit<"!">, Lit<"?">, Lit<".">, Lit<"(">,
  Lit<")">, Lit<"[">, Lit<"]">, Lit<"{">,
  Lit<"}">, Lit<"*">, Lit<"/">, Lit<"&">,
  Lit<"#">, Lit<"%">, Lit<"^">, Lit<"+">,
  Lit<"<">, Lit<"=">, Lit<">">, Lit<"|">,
  Lit<"~">
>;


//------------------------------------------------------------------------------
