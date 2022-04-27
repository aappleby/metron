#include "MtUtils.h"

#include <assert.h>
#include <stdio.h>

//------------------------------------------------------------------------------

ContextState merge_action(ContextState state, ContextAction action) {
  // clang-format off

  ContextState result = CTX_INVALID;

  if (action == CTX_WRITE) {
    switch (state) {
      case CTX_NONE:     result = CTX_OUTPUT; break;
      case CTX_INPUT:    result = CTX_REGISTER; break;
      case CTX_OUTPUT:   result = CTX_OUTPUT; break;
      case CTX_SIGNAL:   result = CTX_INVALID; break;
      case CTX_REGISTER: result = CTX_REGISTER; break;
      case CTX_INVALID:  result = CTX_INVALID; break;
    }
  } else if (action == CTX_READ) {
    switch (state) {
      case CTX_NONE:     result = CTX_INPUT; break;
      case CTX_INPUT:    result = CTX_INPUT; break;
      case CTX_OUTPUT:   result = CTX_SIGNAL; break;
      case CTX_SIGNAL:   result = CTX_SIGNAL; break;
      case CTX_REGISTER: result = CTX_INVALID; break;
      case CTX_INVALID:  result = CTX_INVALID; break;
    }
  }
  // clang-format on

  printf("%s + %s = %s\n", to_string(state), to_string(action),
         to_string(result));

  if (result == CTX_INVALID) {
    int x = 0;
    x++;
  }

  return result;
}

//-----------------------------------------------------------------------------

ContextState merge_branch(ContextState ma, ContextState mb) {
  if (ma == mb) {
    return ma;
  } else if (ma == CTX_INVALID || mb == CTX_INVALID) {
    return CTX_INVALID;
  } else {
    // promote
    if (ma == CTX_NONE && mb == CTX_INPUT) return CTX_INPUT;
    if (ma == CTX_OUTPUT && mb == CTX_SIGNAL) return CTX_SIGNAL;
    if (ma == CTX_NONE && mb == CTX_REGISTER) return CTX_REGISTER;
    if (ma == CTX_INPUT && mb == CTX_REGISTER) return CTX_REGISTER;
    if (ma == CTX_OUTPUT && mb == CTX_REGISTER) return CTX_REGISTER;

    // half-write, infer reg
    if (ma == CTX_NONE && mb == CTX_OUTPUT) return CTX_REGISTER;
    if (ma == CTX_INPUT && mb == CTX_OUTPUT) return CTX_REGISTER;

    // half-write, bad signal
    if (ma == CTX_NONE && mb == CTX_SIGNAL) {
      return CTX_INVALID;
    }

    // order conflict
    if (ma == CTX_INPUT && mb == CTX_SIGNAL) {
      return CTX_INVALID;
    }
    if (ma == CTX_SIGNAL && mb == CTX_REGISTER) {
      return CTX_INVALID;
    }
  }

  return CTX_INVALID;
}

//------------------------------------------------------------------------------
