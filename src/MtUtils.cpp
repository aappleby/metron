#include "MtUtils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

//------------------------------------------------------------------------------

TraceState merge_action(TraceState state, TraceAction action) {
  // clang-format off

  TraceState result = CTX_INVALID;

  if (action == CTX_READ) {
    switch (state) {
      case CTX_NONE:     result = CTX_INPUT;   break;
      case CTX_INPUT:    result = CTX_INPUT;   break;
      case CTX_OUTPUT:   result = CTX_SIGNAL;  break;
      case CTX_MAYBE:    result = CTX_INVALID; break;
      case CTX_SIGNAL:   result = CTX_SIGNAL;  break;
      case CTX_REGISTER: result = CTX_INVALID; break;
      case CTX_INVALID:  result = CTX_INVALID; break;
      case CTX_PENDING:  result = CTX_INVALID; break;
      case CTX_NIL:      result = CTX_INVALID; break;
    }
  }

  if (action == CTX_WRITE) {
    switch (state) {
      case CTX_NONE:     result = CTX_OUTPUT;   break;
      case CTX_INPUT:    result = CTX_REGISTER; break;
      case CTX_OUTPUT:   result = CTX_OUTPUT;   break;
      case CTX_MAYBE:    result = CTX_OUTPUT;   break;
      case CTX_SIGNAL:   result = CTX_INVALID;  break;
      case CTX_REGISTER: result = CTX_REGISTER; break;
      case CTX_INVALID:  result = CTX_INVALID;  break;
      case CTX_PENDING:  result = CTX_INVALID; break;
      case CTX_NIL:      result = CTX_INVALID; break;
    }
  }
  // clang-format on

  if (result == CTX_INVALID) {
    //printf("invalid!\n");
      int x = 1;
      x++;
  }

  return result;
}

//-----------------------------------------------------------------------------

TraceState merge_branch(TraceState ma, TraceState mb) {
  if (ma == CTX_PENDING) {
    return mb;
  } else if (mb == CTX_PENDING) {
    return ma;
  } else if (ma == mb) {
    return ma;
  } else if (ma == CTX_INVALID || mb == CTX_INVALID) {
    return CTX_INVALID;
  } else {
    // clang-format off
    TraceState table[6][6] = {
      {CTX_NONE,     CTX_INPUT,    CTX_MAYBE,    CTX_MAYBE,    CTX_INVALID, CTX_REGISTER},
      {CTX_INPUT,    CTX_INPUT,    CTX_REGISTER, CTX_REGISTER, CTX_INVALID, CTX_REGISTER},
      {CTX_MAYBE,    CTX_REGISTER, CTX_OUTPUT,   CTX_MAYBE,    CTX_SIGNAL,  CTX_REGISTER},
      {CTX_MAYBE,    CTX_REGISTER, CTX_MAYBE,    CTX_MAYBE,    CTX_INVALID, CTX_REGISTER},
      {CTX_INVALID,  CTX_INVALID,  CTX_SIGNAL,   CTX_INVALID,  CTX_SIGNAL,  CTX_INVALID },
      {CTX_REGISTER, CTX_REGISTER, CTX_REGISTER, CTX_REGISTER, CTX_INVALID, CTX_REGISTER},
    };
    // clang-format on

    assert(table[ma][mb] == table[mb][ma]);

    auto result = table[ma][mb];

    if (result  == CTX_INVALID) {
      //printf("invalid!\n");
      int x = 1;
      x++;
    }

    return result;
  }
}

//------------------------------------------------------------------------------

// KCOV_OFF
std::string str_printf(const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  std::string result;
  result.resize(size + 1);
  va_start(args, fmt);
  vsnprintf(result.data(), size_t(size + 1), fmt, args);
  va_end(args);
  assert(result.back() == 0);
  return result;
}
// KCOV_ON

//------------------------------------------------------------------------------
