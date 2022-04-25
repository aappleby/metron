#include "MtUtils.h"

#include <assert.h>

//------------------------------------------------------------------------------

ContextState merge_action(ContextState state, ContextAction action) {
  // clang-format off
  if (action == CTX_WRITE) {
    switch (state) {
      case CTX_NONE:  return CTX_OUTPUT;
      case CTX_INPUT:    return CTX_REGISTER;
      case CTX_OUTPUT:   return CTX_OUTPUT;
      case CTX_SIGNAL:   return CTX_INVALID;
      case CTX_REGISTER: return CTX_REGISTER;
      case CTX_INVALID:  return CTX_INVALID;
    }
  } else if (action == CTX_READ) {
    switch (state) {
      case CTX_NONE:  return CTX_INPUT;
      case CTX_INPUT:    return CTX_INPUT;
      case CTX_OUTPUT:   return CTX_SIGNAL;
      case CTX_SIGNAL:   return CTX_SIGNAL;
      case CTX_REGISTER: return CTX_INVALID;
      case CTX_INVALID:  return CTX_INVALID;
    }
  }
  // clang-format on

  assert(false);
  return CTX_INVALID;
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
    if (ma == CTX_NONE && mb == CTX_SIGNAL) return CTX_INVALID;

    // order conflict
    if (ma == CTX_INPUT && mb == CTX_SIGNAL) return CTX_INVALID;
    if (ma == CTX_SIGNAL && mb == CTX_REGISTER) return CTX_INVALID;
  }

  return CTX_INVALID;
}

//------------------------------------------------------------------------------
