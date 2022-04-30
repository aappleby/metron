#include "MtUtils.h"

#include <assert.h>
#include <stdio.h>

//------------------------------------------------------------------------------

ContextState merge_action(ContextState state, ContextAction action) {
  // clang-format off

  ContextState result = CTX_INVALID;

  if (action == CTX_READ) {
    switch (state) {
      case CTX_NONE:     result = CTX_INPUT;   break;
      case CTX_INPUT:    result = CTX_INPUT;   break;
      case CTX_OUTPUT:   result = CTX_SIGNAL;  break;
      case CTX_MAYBE:    result = CTX_INVALID; break;
      case CTX_SIGNAL:   result = CTX_SIGNAL;  break;
      case CTX_REGISTER: result = CTX_INVALID; break;
      case CTX_INVALID:  result = CTX_INVALID; break;
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
    }
  }
  // clang-format on

  return result;
}

//-----------------------------------------------------------------------------

ContextState merge_branch(ContextState ma, ContextState mb) {
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
    ContextState table[6][6] = {
      {CTX_NONE,     CTX_INPUT,    CTX_MAYBE,    CTX_MAYBE,    CTX_INVALID, CTX_REGISTER},
      {CTX_INPUT,    CTX_INPUT,    CTX_REGISTER, CTX_REGISTER, CTX_INVALID, CTX_REGISTER},
      {CTX_MAYBE,    CTX_REGISTER, CTX_OUTPUT,   CTX_MAYBE,    CTX_SIGNAL,  CTX_REGISTER},
      {CTX_MAYBE,    CTX_REGISTER, CTX_MAYBE,    CTX_MAYBE,    CTX_INVALID, CTX_REGISTER},
      {CTX_INVALID,  CTX_INVALID,  CTX_SIGNAL,   CTX_INVALID,  CTX_SIGNAL,  CTX_INVALID },
      {CTX_REGISTER, CTX_REGISTER, CTX_REGISTER, CTX_REGISTER, CTX_INVALID, CTX_REGISTER},
    };
    // clang-format on

    assert(table[ma][mb] == table[mb][ma]);
    return table[ma][mb];
  }

  return CTX_INVALID;
}

//------------------------------------------------------------------------------
