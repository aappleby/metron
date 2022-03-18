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

#endif

//------------------------------------------------------------------------------

#ifdef __GNUC__

#endif

//------------------------------------------------------------------------------
