#ifndef PONG_H
#define PONG_H
#include "metron/metron_tools.h"

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
    px_ = 0;
    py_ = 0;

    ball_x_ = 320;
    ball_y_ = 240;

    ball_dx_ = 1;
    ball_dy_ = 1;

    pad_x_ = 240;
    pad_y_ = 400;

    quad_a_ = 0;
    quad_b_ = 0;
  }

  //----------------------------------------

  logic<10> pix_x() const { return px_; }
  logic<10> pix_y() const { return py_; }

  //----------------------------------------

  void tock_video() {
    vga_hsync = !((px_ >= 656) && (py_ <= 751));
    vga_vsync = !((py_ >= 490) && (py_ <= 491));

    if ((px_ < 640) && (py_ < 480)) {
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
    logic<10> new_px = px_ + 1;
    logic<10> new_py = py_;

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

    logic<1> quad_dir = quad_a_[1] ^ quad_b_[0];
    logic<1> quad_step = quad_a_[1] ^ quad_a_[0] ^ quad_b_[1] ^ quad_b_[0];

    logic<10> new_pad_x = pad_x_;
    logic<10> new_pad_y = pad_y_;

    if (quad_step) {
      new_pad_x = pad_x_ + quad_dir ? 1 : 0;
      if (new_pad_x < 120) new_pad_x = 120;
      if (new_pad_x > 520) new_pad_x = 520;
    }

    //----------
    // Update in_ball

    logic<10> new_ball_x = ball_x_;
    logic<10> new_ball_y = ball_y_;
    logic<1> new_ball_dx = ball_dx_;
    logic<1> new_ball_dy = ball_dy_;

    if (in_border() | in_paddle()) {
      if ((px_ == ball_x_ - 7) && (py_ == ball_y_ + 0)) new_ball_dx = 1;
      if ((px_ == ball_x_ + 7) && (py_ == ball_y_ + 0)) new_ball_dx = 0;
      if ((px_ == ball_x_ + 0) && (py_ == ball_y_ - 7)) new_ball_dy = 1;
      if ((px_ == ball_x_ + 0) && (py_ == ball_y_ + 7)) new_ball_dy = 0;
    }

    if (new_px == 0 && new_py == 0) {
      new_ball_x = ball_x_ + (new_ball_dx ? 1 : -1);
      new_ball_y = ball_y_ + (new_ball_dy ? 1 : -1);
    }

    //----------
    // Commit

    px_ = new_px;
    py_ = new_py;

    pad_x_ = new_pad_x;
    pad_y_ = new_pad_y;

    ball_x_ = new_ball_x;
    ball_y_ = new_ball_y;

    ball_dx_ = new_ball_dx;
    ball_dy_ = new_ball_dy;

    quad_a_ = quad_a_ << 1 | in_quad_a;
    quad_b_ = quad_b_ << 1 | in_quad_b;
  }

  //----------------------------------------

  logic<1> in_border() const {
    return (px_ <= 7) || (px_ >= 633) || (py_ <= 7) || (py_ >= 473);
  }

  logic<1> in_paddle() const {
    return (px_ >= pad_x_ - 63) && (px_ <= pad_x_ + 63) && (py_ >= pad_y_ - 3) &&
           (py_ <= pad_y_ + 3);
  }

  logic<1> in_ball() const {
    return (px_ >= ball_x_ - 7) && (px_ <= ball_x_ + 7) && (py_ >= ball_y_ - 7) &&
           (py_ <= ball_y_ + 7);
  }

  logic<1> in_checker() const { return px_[3] ^ py_[3]; }

  logic<10> px_;
  logic<10> py_;

  logic<10> pad_x_;
  logic<10> pad_y_;

  logic<10> ball_x_;
  logic<10> ball_y_;

  logic<1> ball_dx_;
  logic<1> ball_dy_;

  logic<2> quad_a_;
  logic<2> quad_b_;
};

//------------------------------------------------------------------------------

#endif
