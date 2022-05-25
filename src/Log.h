#pragma once
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>

//-----------------------------------------------------------------------------
// TinyLog - simple console log with color coding, indentation, and timestamps

struct TinyLog {
  uint32_t _color = 0; // 0 = default color
  int _muted = 0;
  bool _mono = false;
  int _indentation = 0;
  bool _start_line = true;
  uint64_t _time_origin = 0;

  static TinyLog& get() {
    static TinyLog log;
    return log;
  }

  void indent() { _indentation += 2; }
  void dedent() { _indentation -= 2; }
  void mute()   { _muted++; }
  void unmute() { _muted--; }

  void mono()   { _mono = true; }
  void color()  { _mono = false; }

  void reset() {
    _color = 0;
    _muted = 0;
    _indentation = 0;
    _start_line = true;
    _time_origin = 0;
  }

  void set_color(uint32_t color) {
    if (_mono) return;
    if (color != _color) {
      if (color) {
        printf("\u001b[38;2;%d;%d;%dm", (color >> 0) & 0xFF,
          (color >> 8) & 0xFF, (color >> 16) & 0xFF);
        //printf("@");
      }
      else {
        printf("\u001b[0m");
        //printf("@");
      }
      _color = color;
    }
  }


  double timestamp() {
    timespec ts;
    (void)timespec_get(&ts, TIME_UTC);
    uint64_t now = ts.tv_sec * 1000000000ull + ts.tv_nsec;
    if (!_time_origin) _time_origin = now;
    return double(now - _time_origin) / 1.0e9;
  }

  void print_char(FILE* file, int c, uint32_t color) {
    if (_muted) return;

    if (_start_line) {
      _start_line = false;
      print(file, 0, "[%07.3f] ", timestamp());
      for (int j = 0; j < _indentation; j++) /*putchar(' ');*/ printf(" ");
    }

    if (c == '\n') {
      set_color(0);
      //printf("%c", c); //putchar(c);
      fputc(c, file);
      _start_line = true;
    }
    else {
      set_color(color);
      //printf("%c", c); //putchar(c);
      fputc(c, file);
    }
  }

  void print_buffer(FILE* file, uint32_t color, const char* buffer, int len) {
    for (int i = 0; i < len; i++) {
      print_char(file, buffer[i], color);
    }
  }

  void vprint(FILE* file, uint32_t color, const char* format, va_list args) {
    va_list args2;
    va_copy(args2, args);
    int size = vsnprintf(nullptr, 0, format, args2);
    va_end(args2);

    auto buffer = new char[size + 1];
    vsnprintf(buffer, size_t(size) + 1, format, args);
    print_buffer(file, color, buffer, size);
    delete[] buffer;
  }

  void print(FILE* file, uint32_t color, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprint(file, color, format, args);
    va_end(args);
  }
};

#define LOG(...)      TinyLog::get().print(stdout, 0x00000000, __VA_ARGS__)
#define LOG_C(c, ...) TinyLog::get().print(stdout, c, __VA_ARGS__)
#define LOG_R(...)    TinyLog::get().print(stdout, 0x008080FF, __VA_ARGS__)
#define LOG_G(...)    TinyLog::get().print(stdout, 0x0080FF80, __VA_ARGS__)
#define LOG_B(...)    TinyLog::get().print(stdout, 0x00FFA0A0, __VA_ARGS__)
#define LOG_Y(...)    TinyLog::get().print(stdout, 0x0080FFFF, __VA_ARGS__)
#define LOG_W(...)    TinyLog::get().print(stdout, 0x00FFFFFF, __VA_ARGS__)
#define LOG_INDENT()  TinyLog::get().indent()
#define LOG_DEDENT()  TinyLog::get().dedent()

#define LOG_CV(color, format, args) TinyLog::get().vprint(stdout, color, format, args)
#define LOG_RV(format, args)        TinyLog::get().vprint(stdout, 0x008080FF, format, args)
#define LOG_GV(format, args)        TinyLog::get().vprint(stdout, 0x0080FF80, format, args)
#define LOG_BV(format, args)        TinyLog::get().vprint(stdout, 0x00FFA0A0, format, args)
#define LOG_YV(format, args)        TinyLog::get().vprint(stdout, 0x0080FFFF, format, args)
#define LOG_WV(format, args)        TinyLog::get().vprint(stdout, 0x00FFFFFF, format, args)

struct LogIndenter {
  LogIndenter() { TinyLog::get().indent(); }
  ~LogIndenter() { TinyLog::get().dedent(); }
};

#define LINE_CAT1(X,Y) LINE_CAT2(X,Y)
#define LINE_CAT2(X,Y) X##Y
#define LOG_INDENT_SCOPE() LogIndenter LINE_CAT1(indenter, __LINE__)

//-----------------------------------------------------------------------------
