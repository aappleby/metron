#include "Platform.h"

#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <functional>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

#ifdef __GNUC__
#include <execinfo.h>
#include <sys/stat.h>
#include <unistd.h>

#include <csignal>
#endif

#include "TreeSymbols.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

// KCOV_OFF

uint64_t timestamp() {
  timespec ts;
  (void)timespec_get(&ts, TIME_UTC);
  uint64_t now = ts.tv_sec * 1000000000ull + ts.tv_nsec;
  return now;
}

//------------------------------------------------------------------------------

void print_escaped(char s) {
  if (s == '\n')
    printf("\\n");
  else if (s == '\r')
    printf("\\r");
  else if (s == '\t')
    printf("\\t");
  else if (s == '"')
    printf("\\\"");
  else if (s == '\\')
    printf("\\\\");
  else
    printf("%c", s);
}

void print_escaped(const char* source, int a, int b) {
  printf("\"");
  for (; a < b; a++) {
    print_escaped(source[a]);
  }
  printf("\"");
}

/*
void print_escaped(char s) {
  if      (s == '\n') printf("\\n");
  else if (s == '\r') printf("\\r");
  else if (s == '\t') printf("\\t");
  else if (s == '"')  printf("\\\"");
  else if (s == '\\') printf("\\\\");
  else                printf("%c", s);
}
*/

//------------------------------------------------------------------------------
// GCC platform

#ifdef __GNUC__

int plat_mkdir(const char* path, int mode) { return mkdir(path, mode); }

void dprintf(const char* format, ...) {
  // static char buffer[256];
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

void print_stacktrace(void) {
  void* array[1024];
  size_t size = backtrace(array, 1024);
  char** strings = backtrace_symbols(array, size);
  for (int i = 0; i < size; i++) printf("%s\n", strings[i]);
  free(strings);
}

void debugbreak() {
  print_stacktrace();
  exit(-1);
}

#endif

//------------------------------------------------------------------------------
// Visual Studio platform

#ifdef _MSC_VER

#include <Windows.h>
#include <direct.h>

void debugbreak() { __debugbreak(); }
int plat_mkdir(const char* path, int mode) { return _mkdir(path); }
void print_stacktrace() {}

#endif

// KCOV_ON
