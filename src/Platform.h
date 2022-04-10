#pragma once
#include <stdio.h>
#include <stdarg.h>

//------------------------------------------------------------------------------

int plat_mkdir(const char* path, int mode);
void debugbreak();
void dprintf(const char* format = "", ...);
void print_escaped(char s);
void print_escaped(const char* source, int a, int b);

//------------------------------------------------------------------------------

#ifdef _MSC_VER

#pragma warning(disable : 4996)   // unsafe fopen
#pragma warning(disable : 26451)  // Very picky arithmetic overflow warning
#pragma warning(disable : 26812)  // Unscoped enum

#define CHECK_RETURN _Check_return_

#endif

//------------------------------------------------------------------------------

#ifdef __GNUC__

#define CHECK_RETURN __attribute__((warn_unused_result))

#endif

//------------------------------------------------------------------------------

enum class SEV_TYPE {
  INFO = 1,
  WARN = 2,
  ERR  = 4
};

class ErrType {
 public:
  ErrType(SEV_TYPE v, const char* file, int line, const char* format, ...) : sev(v) {
    printf("\n");
    if (v == SEV_TYPE::INFO) {
      printf("Info @ %s : %d\n", file, line);
    }
    else if (v == SEV_TYPE::WARN) {
      printf("Warning @ %s : %d\n", file, line);
    }
    else if (v == SEV_TYPE::ERR) {
      printf("Error @ %s : %d\n", file, line);
    }
    else {
      printf("Error type %d??? @ %s : %d\n", int(v), file, line);
    }

    if (format) {
      va_list args;
      va_start(args, format);
      char msg[256];
      int len = vsnprintf(msg, 256, format, args);
      va_end(args);
      printf("  %s\n", msg);
    }
  }
  SEV_TYPE sev;
};

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

#define INFO(...) ErrType(SEV_TYPE::INFO, __FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) ErrType(SEV_TYPE::WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ERR(...)  ErrType(SEV_TYPE::ERR,  __FILE__, __LINE__, __VA_ARGS__)

//------------------------------------------------------------------------------
