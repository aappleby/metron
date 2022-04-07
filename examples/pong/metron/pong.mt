#ifndef PONG_H
#define PONG_H

#include "metron_tools.h"

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

  logic<1> border() const {
    return (px <= 7) || (px >= 633) || (py <= 7) || (py >= 473);
  }

  logic<1> paddle() const {
    return (px >= pad_x - 63) &&
           (px <= pad_x + 63) &&
           (py >= pad_y -  3) &&
           (py <= pad_y +  3);
  }

  logic<1> ball() const {
    return (px >= ball_x - 7) &&
           (px <= ball_x + 7) &&
           (py >= ball_y - 7) &&
           (py <= ball_y + 7);
  }

  logic<1> checker() const {
    return px[3] ^ py[3];
  }

  //----------------------------------------

  void init() {
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

  void tock() {
    vga_hsync = !((px >= 656) && (py <= 751));
    vga_vsync = !((py >= 490) && (py <= 491));

    vga_R = 0;
    vga_G = 0;
    vga_B = 0;

    if ((px < 640) && (py < 480)) {
      vga_R = border() | paddle() | ball() | checker();
      vga_G = border() | paddle() | ball();
      vga_B = border() | paddle() | ball();
    }
  }

  //----------------------------------------

  void tick(logic<1> in@quad_a, logic<1> in@quad_b) {
    @px = px + 1;
    @py = py;

    //----------
    // Update screen coord

    if (@px == 800) {
      @px = 0;
      @py = @py + 1;
    }
    if (@py == 525) @py = 0;

    //----------
    // Update quadrature encoder

    logic<1> quad_dir  = quad_a[1] ^ quad_b[0];
    logic<1> quad_step = quad_a[1] ^ quad_a[0] ^ quad_b[1] ^ quad_b[0];

    if (quad_step) {
      @pad_x = pad_x + quad_dir ? 1 : 0;
      if (@pad_x < 120) @pad_x = 120;
      if (@pad_x > 520) @pad_x = 520;
    }

    @quad_a = quad_a << 1 | in@quad_a;
    @quad_b = quad_b << 1 | in@quad_b;

    //----------
    // Update ball

    if (border() | paddle()) {
      if((px == ball_x - 7) & (py == ball_y + 0)) @ball_dx = 1;
      if((px == ball_x + 7) & (py == ball_y + 0)) @ball_dx = 0;
      if((px == ball_x + 0) & (py == ball_y - 7)) @ball_dy = 1;
      if((px == ball_x + 0) & (py == ball_y + 7)) @ball_dy = 0;
    }

    if (@px == 0 && @py == 0) {
      @ball_x = ball_x + (@ball_dx ? 1 : -1);
      @ball_y = ball_y + (@ball_dy ? 1 : -1);
    }
  }

  //----------------------------------------

private:

  logic<10> px;
  logic<10> py;

  logic<10> pad_x;
  logic<10> pad_y;

  logic<10> ball_x;
  logic<10> ball_y;

  logic<1>  ball_dx;
  logic<1>  ball_dy;

  logic<2>  quad_a;
  logic<2>  quad_b;
};

//------------------------------------------------------------------------------

#endif // PONG_H
