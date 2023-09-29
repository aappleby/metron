// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once
#include <array>

//------------------------------------------------------------------------------
// MUST BE SORTED CASE-SENSITIVE

constexpr std::array c_keywords = {
"auto",
"bool",
"break",
"case",
"char",
"class",
"const",
"continue",
"default",
"do",
"double",
"else",
"enum",
"extern",
"false",
"float",
"for",
"goto",
"if",
"inline",
"int",
"long",
"namespace",
"nullptr",
"offsetof",
"operator",
"private",
"protected",
"public",
"register",
"return",
"short",
//"signed",
"sizeof",
"static",
"static_cast",
"struct",
"switch",
"template",
"true",
"typedef",
"union",
//"unsigned",
"using",
"void",
"while",
};

//------------------------------------------------------------------------------
// MUST BE SORTED CASE-SENSITIVE

constexpr std::array builtin_type_base = {
  "bool",
  "char",
  "double",
  "float",
  "int",
  "logic", // FIXME THIS IS PROBABLY BAD
  "long",
  "short",
  //"signed",
  //"unsigned",
  "void",
};

//------------------------------------------------------------------------------
// MUST BE SORTED CASE-SENSITIVE

constexpr std::array builtin_type_prefix = {
  "long",
  "short",
  "signed",
  "unsigned"
};

//------------------------------------------------------------------------------
// MUST BE SORTED CASE-SENSITIVE

constexpr std::array builtin_type_suffix = {
  // Why, GCC, why?
  "_Complex",
  "__complex__",
};

//------------------------------------------------------------------------------
// MUST BE SORTED CASE-SENSITIVE

constexpr std::array qualifiers = {
  "auto",
  "const",
  "extern",
  "inline",
  "static",
};

//------------------------------------------------------------------------------
// MUST BE SORTED CASE-SENSITIVE

constexpr std::array binary_operators = {
  "!=",
  "%",
  "%=",
  "&",
  "&&",
  "&=",
  "*",
  "*=",
  "+",
  "+=",
  "-",
  "-=",
  "->",
  "->*",
  ".",
  ".*",
  "/",
  "/=",
  "::",
  "<",
  "<<",
  "<<=",
  "<=",
  "<=>",
  "=",
  "==",
  ">",
  ">=",
  ">>",
  ">>=",
  "^",
  "^=",
  "|",
  "|=",
  "||",
};

constexpr std::array stddef_typedefs = {
  "size_t",
  "ptrdiff_t",
  "nullptr_t",
  "max_align_t",
  "byte"
};

constexpr std::array stdio_typedefs = {
  "FILE",
  "fpos_t",
  "size_t",
};

constexpr std::array stdint_typedefs = {
  "int8_t",
  "int16_t",
  "int32_t",
  "int64_t",
  "int_fast8_t",
  "int_fast16_t",
  "int_fast32_t",
  "int_fast64_t",
  "int_least8_t",
  "int_least16_t",
  "int_least32_t",
  "int_least64_t",
  "intmax_t",
  "intptr_t",
  "uint8_t",
  "uint16_t",
  "uint32_t",
  "uint64_t",
  "uint_fast8_t",
  "uint_fast16_t",
  "uint_fast32_t",
  "uint_fast64_t",
  "uint_least8_t",
  "uint_least16_t",
  "uint_least32_t",
  "uint_least64_t",
  "uintmax_t",
  "uintptr_t",
};

//------------------------------------------------------------------------------

constexpr int prefix_precedence(const char* op) {
  if (__builtin_strcmp(op, "++")  == 0) return 3;
  if (__builtin_strcmp(op, "--")  == 0) return 3;
  if (__builtin_strcmp(op, "+")   == 0) return 3;
  if (__builtin_strcmp(op, "-")   == 0) return 3;
  if (__builtin_strcmp(op, "!")   == 0) return 3;
  if (__builtin_strcmp(op, "~")   == 0) return 3;
  if (__builtin_strcmp(op, "*")   == 0) return 3;
  if (__builtin_strcmp(op, "&")   == 0) return 3;

  // 2 type(a) type{a}
  // 3 (type)a sizeof a sizeof(a) co_await a
  // 3 new a new a[] delete a delete a[]
  // 16 throw a co_yield a
  return 0;
}

//------------------------------------------------------------------------------

constexpr int prefix_assoc(const char* op) {
  if (__builtin_strcmp(op, "++")  == 0) return -2;
  if (__builtin_strcmp(op, "--")  == 0) return -2;
  if (__builtin_strcmp(op, "+")   == 0) return -2;
  if (__builtin_strcmp(op, "-")   == 0) return -2;
  if (__builtin_strcmp(op, "!")   == 0) return -2;
  if (__builtin_strcmp(op, "~")   == 0) return -2;
  if (__builtin_strcmp(op, "*")   == 0) return -2;
  if (__builtin_strcmp(op, "&")   == 0) return -2;

  // 2 type(a) type{a}
  // 3 (type)a sizeof a sizeof(a) co_await a
  // 3 new a new a[] delete a delete a[]
  // 16 throw a co_yield a
  return 0;
}

//------------------------------------------------------------------------------

constexpr int binary_precedence(const char* op) {
  if (__builtin_strcmp(op, "::")  == 0) return 1;
  if (__builtin_strcmp(op, ".")   == 0) return 2;
  if (__builtin_strcmp(op, "->")  == 0) return 2;
  if (__builtin_strcmp(op, ".*")  == 0) return 4;
  if (__builtin_strcmp(op, "->*") == 0) return 4;
  if (__builtin_strcmp(op, "*")   == 0) return 5;
  if (__builtin_strcmp(op, "/")   == 0) return 5;
  if (__builtin_strcmp(op, "%")   == 0) return 5;
  if (__builtin_strcmp(op, "+")   == 0) return 6;
  if (__builtin_strcmp(op, "-")   == 0) return 6;
  if (__builtin_strcmp(op, "<<")  == 0) return 7;
  if (__builtin_strcmp(op, ">>")  == 0) return 7;
  if (__builtin_strcmp(op, "<=>") == 0) return 8;
  if (__builtin_strcmp(op, "<")   == 0) return 9;
  if (__builtin_strcmp(op, "<=")  == 0) return 9;
  if (__builtin_strcmp(op, ">")   == 0) return 9;
  if (__builtin_strcmp(op, ">=")  == 0) return 9;
  if (__builtin_strcmp(op, "==")  == 0) return 10;
  if (__builtin_strcmp(op, "!=")  == 0) return 10;
  if (__builtin_strcmp(op, "&")   == 0) return 11;
  if (__builtin_strcmp(op, "^")   == 0) return 12;
  if (__builtin_strcmp(op, "|")   == 0) return 13;
  if (__builtin_strcmp(op, "&&")  == 0) return 14;
  if (__builtin_strcmp(op, "||")  == 0) return 15;
  if (__builtin_strcmp(op, "?")   == 0) return 16;
  if (__builtin_strcmp(op, ":")   == 0) return 16;
  if (__builtin_strcmp(op, "=")   == 0) return 16;
  if (__builtin_strcmp(op, "+=")  == 0) return 16;
  if (__builtin_strcmp(op, "-=")  == 0) return 16;
  if (__builtin_strcmp(op, "*=")  == 0) return 16;
  if (__builtin_strcmp(op, "/=")  == 0) return 16;
  if (__builtin_strcmp(op, "%=")  == 0) return 16;
  if (__builtin_strcmp(op, "<<=") == 0) return 16;
  if (__builtin_strcmp(op, ">>=") == 0) return 16;
  if (__builtin_strcmp(op, "&=")  == 0) return 16;
  if (__builtin_strcmp(op, "^=")  == 0) return 16;
  if (__builtin_strcmp(op, "|=")  == 0) return 16;
  if (__builtin_strcmp(op, ",")   == 0) return 17;
  return 0;
}

//------------------------------------------------------------------------------

constexpr int binary_assoc(const char* op) {
  if (__builtin_strcmp(op, "::")  == 0) return 1;
  if (__builtin_strcmp(op, ".")   == 0) return 1;
  if (__builtin_strcmp(op, "->")  == 0) return 1;
  if (__builtin_strcmp(op, ".*")  == 0) return 1;
  if (__builtin_strcmp(op, "->*") == 0) return 1;
  if (__builtin_strcmp(op, "*")   == 0) return 1;
  if (__builtin_strcmp(op, "/")   == 0) return 1;
  if (__builtin_strcmp(op, "%")   == 0) return 1;
  if (__builtin_strcmp(op, "+")   == 0) return 1;
  if (__builtin_strcmp(op, "-")   == 0) return 1;
  if (__builtin_strcmp(op, "<<")  == 0) return 1;
  if (__builtin_strcmp(op, ">>")  == 0) return 1;
  if (__builtin_strcmp(op, "<=>") == 0) return 1;
  if (__builtin_strcmp(op, "<")   == 0) return 1;
  if (__builtin_strcmp(op, "<=")  == 0) return 1;
  if (__builtin_strcmp(op, ">")   == 0) return 1;
  if (__builtin_strcmp(op, ">=")  == 0) return 1;
  if (__builtin_strcmp(op, "==")  == 0) return 1;
  if (__builtin_strcmp(op, "!=")  == 0) return 1;
  if (__builtin_strcmp(op, "&")   == 0) return 1;
  if (__builtin_strcmp(op, "^")   == 0) return 1;
  if (__builtin_strcmp(op, "|")   == 0) return 1;
  if (__builtin_strcmp(op, "&&")  == 0) return 1;
  if (__builtin_strcmp(op, "||")  == 0) return 1;
  if (__builtin_strcmp(op, "?")   == 0) return -1;
  if (__builtin_strcmp(op, ":")   == 0) return -1;
  if (__builtin_strcmp(op, "=")   == 0) return -1;
  if (__builtin_strcmp(op, "+=")  == 0) return -1;
  if (__builtin_strcmp(op, "-=")  == 0) return -1;
  if (__builtin_strcmp(op, "*=")  == 0) return -1;
  if (__builtin_strcmp(op, "/=")  == 0) return -1;
  if (__builtin_strcmp(op, "%=")  == 0) return -1;
  if (__builtin_strcmp(op, "<<=") == 0) return -1;
  if (__builtin_strcmp(op, ">>=") == 0) return -1;
  if (__builtin_strcmp(op, "&=")  == 0) return -1;
  if (__builtin_strcmp(op, "^=")  == 0) return -1;
  if (__builtin_strcmp(op, "|=")  == 0) return -1;
  if (__builtin_strcmp(op, ",")   == 0) return 1;
  return 0;
}

//------------------------------------------------------------------------------

constexpr int suffix_precedence(const char* op) {
  if (__builtin_strcmp(op, "++")  == 0) return 2;
  if (__builtin_strcmp(op, "--")  == 0) return 2;
  return 0;
}

//------------------------------------------------------------------------------

constexpr int suffix_assoc(const char* op) {
  if (__builtin_strcmp(op, "++")  == 0) return 2;
  if (__builtin_strcmp(op, "--")  == 0) return 2;
  return 0;
}

//------------------------------------------------------------------------------
