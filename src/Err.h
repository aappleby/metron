#pragma once
#include <stdio.h>
#include <stdarg.h>

#include "Log.h"

//------------------------------------------------------------------------------

enum class SEV_TYPE {
  INFO = 1,
  WARN = 2,
  ERR  = 4
};

//------------------------------------------------------------------------------

class ErrType {
public:
  ErrType(SEV_TYPE v, const char* file, int line, const char* func, const char* format, ...) : sev(v) {
    char msg[256];
    if (format) {
      va_list args;
      va_start(args, format);
      int len = vsnprintf(msg, 256, format, args);
      va_end(args);
    }

    if (v == SEV_TYPE::INFO) {
      LOG_G("Info @ %s : %d : %s\n", file, line, func);
      LOG_G("  %s\n", msg);
    }
    else if (v == SEV_TYPE::WARN) {
      LOG_Y("Warning @ %s : %d : %s\n", file, line, func);
      LOG_Y("  %s\n", msg);
    }
    else if (v == SEV_TYPE::ERR) {
      LOG_R("Error @ %s : %d : %s\n", file, line, func);
      LOG_R("  %s\n", msg);
    }
    else {
      LOG_R("Error type %d??? @ %s : %d\n", int(v), file, line);
    }

  }
  SEV_TYPE sev;
};

//------------------------------------------------------------------------------

class Err {
public:

  Err() : err(0) {}
  Err(const ErrType& et) : err(int(et.sev)) {}

  Err& operator<<(const Err& e) {
    /*
    if (e) {
    debugbreak();
    }
    */
    err |= e.err;
    return *this;
  }

  Err& operator<<(const ErrType& e) {
    /*
    if (e) {
    debugbreak();
    }
    */
    err |= int(e.sev);
    return *this;
  }

  bool has_info() const { return err & 1; }
  bool has_warn() const { return err & 2; }
  bool has_err()  const { return err & 4; }

private:
  int err;
};

#define INFO(...) ErrType(SEV_TYPE::INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define WARN(...) ErrType(SEV_TYPE::WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define ERR(...)  ErrType(SEV_TYPE::ERR,  __FILE__, __LINE__, __func__, __VA_ARGS__)

//------------------------------------------------------------------------------
