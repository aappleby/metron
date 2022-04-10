#pragma once
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>

//-----------------------------------------------------------------------------
// TinyLog - simple console log with color coding, indentation, and timestamps

struct TinyLog {
  uint32_t color = 0;
  bool muted = false;
  int indentation = 0;
  bool start_line = true;

  static TinyLog& get() {
    static TinyLog log;
    return log;
  }

  void indent() { indentation += 2; }
  void dedent() { indentation -= 2; }
  void mute()   { muted = true; }
  void unmute() { muted = false; }

  void set_color(uint32_t new_color) {
    if (muted) return;

    if (color == new_color) return;
    color = new_color;
    if (color == 0) {
      ::printf("\u001b[0m");
    } else {
      ::printf("\u001b[38;2;%d;%d;%dm", (color >> 0) & 0xFF,
        (color >> 8) & 0xFF, (color >> 16) & 0xFF);
    }
  }

  void print_prefix() {
    if (muted) return;

    timespec ts;
    (void)timespec_get(&ts, TIME_UTC);
    uint64_t now = ts.tv_sec * 1000000000ull + ts.tv_nsec;

    static uint64_t time_origin = 0;
    if (!time_origin) time_origin = now;

    set_color(0);
    ::printf("[%07.3f] ", double(now - time_origin) / 1.0e9);
  }

  void print_buffer(uint32_t color, const char* buffer, int len) {
    if (muted) return;
    set_color(color);
    for (int i = 0; i < len; i++) {
      auto c = buffer[i];
      if (start_line) {
        print_prefix();
        set_color(color);
        for (int j = 0; j < indentation; j++) {
          if (!muted) putchar(' ');
        }
        start_line = false;
      }

      if (!muted) putchar(c);
      if (c == '\n') start_line = true;
    }
    set_color(0);
    fflush(stdout);
  }

  void printf(uint32_t color, const char* format = "", ...) {
    if (muted) return;
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, 256, format, args);
    va_end(args);
    print_buffer(color, buffer, len);
  }
};

#define LOG(...)   TinyLog::get().printf(0x00000000, __VA_ARGS__);
#define LOG_R(...) TinyLog::get().printf(0x008080FF, __VA_ARGS__);
#define LOG_G(...) TinyLog::get().printf(0x0080FF80, __VA_ARGS__);
#define LOG_B(...) TinyLog::get().printf(0x00FFA0A0, __VA_ARGS__);

#define LOG_C(...) TinyLog::get().printf(0x00FFFF80, __VA_ARGS__);
#define LOG_M(...) TinyLog::get().printf(0x00FF80FF, __VA_ARGS__);
#define LOG_Y(...) TinyLog::get().printf(0x0080FFFF, __VA_ARGS__);
#define LOG_W(...) TinyLog::get().printf(0x00FFFFFF, __VA_ARGS__);

#define LOG_INDENT() TinyLog::get().indent()
#define LOG_DEDENT() TinyLog::get().dedent()

struct LogIndenter {
  LogIndenter() { LOG_INDENT(); }
  ~LogIndenter() { LOG_DEDENT(); }
};

#define LOG_INDENT_SCOPE() LogIndenter indenter##__LINE__;

//-----------------------------------------------------------------------------
