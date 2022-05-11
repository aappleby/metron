#include "Err.h"

#include <stdarg.h>
#include <stdio.h>

#include "Log.h"

// KCOV_OFF

//------------------------------------------------------------------------------

ErrType::ErrType(SEV_TYPE v, const char* file, int line, const char* func,
                 const char* format, ...)
    : sev(v) {
  va_list args;
  va_start(args, format);

  if (v == SEV_TYPE::INFO) {
    // KCOV_OFF
    LOG_G("Info @ %s : %d : %s\n", file, line, func);
    LOG_G("  ");
    LOG_GV(format, args);
    // KCOV_ON
  } else if (v == SEV_TYPE::WARN) {
    LOG_Y("Warning @ %s : %d : %s\n", file, line, func);
    LOG_Y("  ");
    LOG_YV(format, args);
  } else if (v == SEV_TYPE::ERR) {
    LOG_R("Error @ %s : %d : %s\n", file, line, func);
    LOG_R("  ");
    LOG_RV(format, args);
  }

  va_end(args);
}

//------------------------------------------------------------------------------

Err::Err() : err(0) {}

Err::Err(const ErrType& et) : err(int(et.sev)) {}

Err& Err::operator<<(const Err& e) {
  if (e.err & int(SEV_TYPE::ERR)) {
    //err = err;
  }
  err |= e.err;
  return *this;
}

Err& Err::operator<<(const ErrType& e) {
  if (int(e.sev) & int(SEV_TYPE::ERR)) {
    //err = err;
  }

  err |= int(e.sev);
  return *this;
}

bool Err::has_info() const { return err & 1; }
bool Err::has_warn() const { return err & 2; }
bool Err::has_err() const { return err & 4; }

//------------------------------------------------------------------------------

// KCOV_ON
