#ifndef PONG_H
#define PONG_H
#include "metron/tools/metron_tools.h"

//------------------------------------------------------------------------------

/*
640x480x60
Pixel clock 25.175 MHz

Screen X    [000-639]
Front porch [640-655]
HSync       [656-751]
Back porch  [752-799]

Screen Y    [000-479]
Front porch [480-489]
VSync       [490-491]
Back porch  [492-524]
*/

class Pong {
 public:
  logic<1> vga_hsync;
  logic<1> vga_vsync;
  logic<1> vga_R;
  logic<1> vga_G;
  logic<1> vga_B;

  //----------------------------------------

  Pong() {
    px = 0;
    py = 0;

    ball_x = 320;
    ball_y = 240;

    ball_dx = 1;
    ball_dy = 1;

    pad_x = 240;
    pad_y = 400;

    quad_a = 0;
    quad_b = 0;
  }

  //----------------------------------------

  logic<10> pix_x() const { return px; }
  logic<10> pix_y() const { return py; }

  //----------------------------------------

  void tock_video() {
    vga_hsync = !((px >= 656) && (py <= 751));
    vga_vsync = !((py >= 490) && (py <= 491));

    if ((px < 640) && (py < 480)) {
      vga_R = in_border() | in_paddle() | in_ball() | in_checker();
      vga_G = in_border() | in_paddle() | in_ball();
      vga_B = in_border() | in_paddle() | in_ball();
    } else {
      vga_R = 0;
      vga_G = 0;
      vga_B = 0;
    }
  }

  //----------------------------------------

  void tock_game(logic<1> in_quad_a, logic<1> in_quad_b) {
    tick(in_quad_a, in_quad_b);
  }

  //----------------------------------------

 private:
  void tick(logic<1> in_quad_a, logic<1> in_quad_b) {
    logic<10> new_px = px + 1;
    logic<10> new_py = py;

    //----------
    // Update screen coord

    if (new_px == 800) {
      new_px = 0;
      new_py = new_py + 1;
    }

    if (new_py == 525) {
      new_py = 0;
    }

    //----------
    // Update quadrature encoder

    logic<1> quad_dir = quad_a[1] ^ quad_b[0];
    logic<1> quad_step = quad_a[1] ^ quad_a[0] ^ quad_b[1] ^ quad_b[0];

    logic<10> new_pad_x = pad_x;
    logic<10> new_pad_y = pad_y;

    if (quad_step) {
      new_pad_x = pad_x + quad_dir ? 1 : 0;
      if (new_pad_x < 120) new_pad_x = 120;
      if (new_pad_x > 520) new_pad_x = 520;
    }

    //----------
    // Update in_ball

    logic<10> new_ball_x = ball_x;
    logic<10> new_ball_y = ball_y;
    logic<1> new_ball_dx = ball_dx;
    logic<1> new_ball_dy = ball_dy;

    if (in_border() | in_paddle()) {
      if ((px == ball_x - 7) && (py == ball_y + 0)) new_ball_dx = 1;
      if ((px == ball_x + 7) && (py == ball_y + 0)) new_ball_dx = 0;
      if ((px == ball_x + 0) && (py == ball_y - 7)) new_ball_dy = 1;
      if ((px == ball_x + 0) && (py == ball_y + 7)) new_ball_dy = 0;
    }

    if (new_px == 0 && new_py == 0) {
      new_ball_x = ball_x + (new_ball_dx ? 1 : -1);
      new_ball_y = ball_y + (new_ball_dy ? 1 : -1);
    }

    //----------
    // Commit

    px = new_px;
    py = new_py;

    pad_x = new_pad_x;
    pad_y = new_pad_y;

    ball_x = new_ball_x;
    ball_y = new_ball_y;

    ball_dx = new_ball_dx;
    ball_dy = new_ball_dy;

    quad_a = quad_a << 1 | in_quad_a;
    quad_b = quad_b << 1 | in_quad_b;
  }

  //----------------------------------------

  logic<1> in_border() const {
    return (px <= 7) || (px >= 633) || (py <= 7) || (py >= 473);
  }

  logic<1> in_paddle() const {
    return (px >= pad_x - 63) && (px <= pad_x + 63) && (py >= pad_y - 3) &&
           (py <= pad_y + 3);
  }

  logic<1> in_ball() const {
    return (px >= ball_x - 7) && (px <= ball_x + 7) && (py >= ball_y - 7) &&
           (py <= ball_y + 7);
  }

  logic<1> in_checker() const { return px[3] ^ py[3]; }

  logic<10> px;
  logic<10> py;

  logic<10> pad_x;
  logic<10> pad_y;

  logic<10> ball_x;
  logic<10> ball_y;

  logic<1> ball_dx;
  logic<1> ball_dy;

  logic<2> quad_a;
  logic<2> quad_b;
};

//------------------------------------------------------------------------------

#endif
