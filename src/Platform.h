#pragma once

//------------------------------------------------------------------------------

int plat_mkdir(const char* path, int mode);
void debugbreak();
void dprintf(const char* format = "", ...);
void print_escaped(char s);
void print_escaped(const char* source, int a, int b);

//------------------------------------------------------------------------------

#ifdef _MSC_VER

#pragma warning(disable : 4996)  // unsafe fopen
#pragma warning(disable : 26451) // Very picky arithmetic overflow warning
#pragma warning(disable : 26812) // Unscoped enum

#define CHECK_RETURN _Check_return_

#endif

//------------------------------------------------------------------------------

#ifdef __GNUC__

#define CHECK_RETURN __attribute__((warn_unused_result))

#endif


//------------------------------------------------------------------------------
