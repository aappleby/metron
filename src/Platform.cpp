#include "Platform.h"

#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>

#include <algorithm>
#include <functional>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "TreeSymbols.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

// KCOV_OFF

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

#include <sys/stat.h>
#include <unistd.h>

#include <csignal>

typedef int64_t LARGE_INTEGER;

int plat_mkdir(const char* path, int mode) { return mkdir(path, mode); }

void dprintf(const char* format, ...) {
  //static char buffer[256];
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

/*
  {
    std::string cwd;
    cwd.resize(FILENAME_MAX);
    getcwd(cwd.data(),FILENAME_MAX);
    LOG_R("cwd %s\n", cwd);
  }
*/


#include <stdio.h>
#include <stdlib.h>

/* Paste this on the file you want to debug. */
#include <stdio.h>
#include <execinfo.h>
void print_trace(void) {
  void *array[1024];
  size_t size = backtrace(array, 1024);
  char **strings = backtrace_symbols(array, size);
  for (int i = 0; i < size; i++) printf("%s\n", strings[i]);
  free(strings);
}

void debugbreak() {
  print_trace();
  raise(SIGTRAP);
}


#endif

//------------------------------------------------------------------------------
// Visual Studio platform

#ifdef _MSC_VER

#include <Windows.h>
#include <direct.h>

#pragma warning(disable : 4996)  // unsafe fopen

void debugbreak() { __debugbreak(); }

int plat_mkdir(const char* path, int mode) { return _mkdir(path); }

/*
  {
    std::string cwd;
    cwd.resize(FILENAME_MAX);
    getcwd(cwd.data(),FILENAME_MAX);
    LOG_R("cwd %s\n", cwd);
  }
*/


void print_trace() {
}

#endif

// KCOV_ON
