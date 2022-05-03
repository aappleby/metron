#pragma once
#include <stdarg.h>
#include <stdio.h>

#include "Log.h"

//------------------------------------------------------------------------------

enum class SEV_TYPE { INFO = 1, WARN = 2, ERR = 4 };

//------------------------------------------------------------------------------

class ErrType {
 public:
  ErrType(SEV_TYPE v, const char* file, int line, const char* func,
          const char* format, ...);
  SEV_TYPE sev;
};

//------------------------------------------------------------------------------

class Err {
 public:
  Err();
  Err(const ErrType& et);

  Err& operator<<(const Err& e);
  Err& operator<<(const ErrType& e);
  bool has_info() const;
  bool has_warn() const;
  bool has_err() const;

  operator bool() const { return has_err(); }

 private:
  int err;
};

//------------------------------------------------------------------------------

#define INFO(...) \
  ErrType(SEV_TYPE::INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define WARN(...) \
  ErrType(SEV_TYPE::WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define ERR(...) \
  ErrType(SEV_TYPE::ERR, __FILE__, __LINE__, __func__, __VA_ARGS__)

//------------------------------------------------------------------------------
