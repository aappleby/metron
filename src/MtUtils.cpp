#include "MtUtils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

//------------------------------------------------------------------------------

TraceState merge_action(TraceState state, TraceAction action) {
  // clang-format off

  TraceState result = CTX_INVALID;

  if (action == ACT_READ) {
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

  if (action == ACT_WRITE) {
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

const char* merge_message(TraceState state, TraceAction action) {
  // clang-format off

  TraceState result = CTX_INVALID;

  if (action == ACT_READ) {
    switch (state) {
      case CTX_NONE:     return "Field was untouched, is now an input";
      case CTX_INPUT:    return "Reading from an input is OK";
      case CTX_OUTPUT:   return "Reading from an output makes it a signal";
      case CTX_MAYBE:    return "Can't read from a maybe";
      case CTX_SIGNAL:   return "Reading a signal is OK";
      case CTX_REGISTER: return "Can't read from a register after it has been written";
      case CTX_INVALID:  return "Can't read from Invalid";
      case CTX_PENDING:  return "Can't read from Pending";
      case CTX_NIL:      return "Can't read from Nil";
    }
  }

  if (action == ACT_WRITE) {
    switch (state) {
      case CTX_NONE:     return "Field was untouched, is now an output";
      case CTX_INPUT:    return "Writing to an input makes it a register";
      case CTX_OUTPUT:   return "Overwriting an output is OK";
      case CTX_MAYBE:    return "Overwriting a maybe is OK";
      case CTX_SIGNAL:   return "Can't write a signal after it has been read";
      case CTX_REGISTER: return "Overwriting a register is OK";
      case CTX_INVALID:  return "Cant write to Invalid";
      case CTX_PENDING:  return "Cant write to Pending";
      case CTX_NIL:      return "Cant write to Nil";
    }
  }

  if (action == ACT_PUSH) {
    return "Push a copy of state stack top";
  }

  if (action == ACT_POP) {
    return "Pop state stack top";
  }

  if (action == ACT_SWAP) {
    return "Swap state stack top and next";
  }

  if (action == ACT_MERGE) {
    return "Merge branches";
  }

  // clang-format on

  return "### Fill in action messages ###";
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

const char* merge_branch_message(TraceState ma, TraceState mb) {
  if (ma == CTX_PENDING || mb == CTX_PENDING) {
    return "Can't merge a Pending";
  } else if (ma == CTX_NIL || mb == CTX_NIL) {
    return "Can't merge a Pending";
  } else if (ma == mb) {
    return "Merging identical branches is OK";
  } else if (ma == CTX_INVALID || mb == CTX_INVALID) {
    return "Merging invalid branches is an error";
  } else {
    // clang-format off
    const char* table[6][6] = {
      // CTX_NONE
      {
        "Nothing happened in either branch", /* CTX_NONE + CTX_NONE = CTX_NONE */
        "Read happened in one branch, this is now an input", /* CTX_NONE + CTX_INPUT = CTX_INPUT */
        "Only one branch wrote the field - this could be an output, could be a signal",/* CTX_NONE + CTX_OUTPUT = CTX_MAYBE */
        "Still don't know if this is an output or a signal",/* CTX_NONE + CTX_MAYBE = CTX_MAYBE */
        "Field was untouched in one branch and written and then read in the other branch - this is not valid",/* CTX_NONE + CTX_SIGNAL = CTX_INVALID */
        "One branch read and then wrote the field",/* CTX_NONE + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "One branch read the field, this is an input",/* CTX_INPUT + CTX_NONE = CTX_INPUT */
        "Merging identical branches is OK",/* CTX_INPUT + CTX_INPUT = CTX_INPUT */
        "Read in one branch, write in the other - must be a register",/* CTX_INPUT + CTX_OUTPUT = CTX_REGISTER */
        "Read in one branch, maybe write in the other - must be a register",/* CTX_INPUT + CTX_MAYBE = CTX_REGISTER */
        "Read in one branch, write-read in the other - invalid",/* CTX_INPUT + CTX_SIGNAL = CTX_INVALID */
        "Read in one branch, read-write in the other - must be a register",/* CTX_INPUT + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "Only one branch wrote the field - this could be an output, could be a signal",/* CTX_OUTPUT + CTX_NONE = CTX_MAYBE */
        "Read in one branch, write in the other - must be a register",/* CTX_OUTPUT + CTX_INPUT = CTX_REGISTER */
        "Merging identical branches is OK",/* CTX_OUTPUT + CTX_OUTPUT = CTX_OUTPUT */
        "Not all branches write - must be a maybe",/* CTX_OUTPUT + CTX_MAYBE = CTX_MAYBE */
        "Still a signal",/* CTX_OUTPUT + CTX_SIGNAL = CTX_SIGNAL */
        "Still a register",/* CTX_OUTPUT + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "Still don't know if this is an output or a signal",/* CTX_MAYBE + CTX_NONE = CTX_MAYBE */
        "Read in one branch, maybe write in the other - must be a register",/* CTX_MAYBE + CTX_INPUT = CTX_REGISTER */
        "Not all branches write - must be a maybe",/* CTX_MAYBE + CTX_OUTPUT = CTX_MAYBE */
        "Merging identical branches is OK",/* CTX_MAYBE + CTX_MAYBE = CTX_MAYBE */
        "A signal that only might be written in the other branch is invalid",/* CTX_MAYBE + CTX_SIGNAL = CTX_INVALID */
        "Still a register",/* CTX_MAYBE + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "WR in one branch, none in the other - invalid",/* CTX_SIGNAL + CTX_NONE = CTX_INVALID */
        "WR in one branch, R in the other - invalid",/* CTX_SIGNAL + CTX_INPUT = CTX_INVALID */
        "Still a signal",/* CTX_SIGNAL + CTX_OUTPUT = CTX_SIGNAL */
        "A signal that only might be written in the other branch is invalid",/* CTX_SIGNAL + CTX_MAYBE = CTX_INVALID */
        "Merging identical branches is OK",/* CTX_SIGNAL + CTX_SIGNAL = CTX_SIGNAL */
        "Signal in one branch, register in the other - invalid",/* CTX_SIGNAL + CTX_REGISTER = CTX_INVALID */
      },
      {
        "Still a register",/* CTX_REGISTER + CTX_NONE = CTX_REGISTER */
        "Still a register",/* CTX_REGISTER + CTX_INPUT = CTX_REGISTER */
        "Still a register",/* CTX_REGISTER + CTX_OUTPUT = CTX_REGISTER */
        "Still a register",/* CTX_REGISTER + CTX_MAYBE = CTX_REGISTER */
        "Register in one branch, signal in the other - invalid",/* CTX_REGISTER + CTX_SIGNAL = CTX_INVALID */
        "Merging identical branches is OK",/* CTX_REGISTER + CTX_REGISTER = CTX_REGISTER */
      },
    };
    // clang-format on

    return table[ma][mb];
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
