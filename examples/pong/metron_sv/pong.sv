`ifndef PONG_H
`define PONG_H
`include "metron/metron_tools.sv"

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

module Pong (
  // global clock
  input logic clock,
  // output signals
  output logic vga_hsync,
  output logic vga_vsync,
  output logic vga_R,
  output logic vga_G,
  output logic vga_B,
  // pix_x() ports
  output logic[9:0] pix_x_ret,
  // pix_y() ports
  output logic[9:0] pix_y_ret,
  // tock_game() ports
  input logic tock_game_in_quad_a,
  input logic tock_game_in_quad_b
);
 /*public:*/

  //----------------------------------------

  initial begin
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
  end

  //----------------------------------------

  always_comb begin : pix_x  pix_x_ret = px_; end
  always_comb begin : pix_y  pix_y_ret = py_; end

  //----------------------------------------

  always_comb begin : tock_video
    vga_hsync = !((px_ >= 656) && (py_ <= 751));
    vga_vsync = !((py_ >= 490) && (py_ <= 491));

    if ((px_ < 640) && (py_ < 480)) begin
      vga_R = in_border() | in_paddle() | in_ball() | in_checker();
      vga_G = in_border() | in_paddle() | in_ball();
      vga_B = in_border() | in_paddle() | in_ball();
    end else begin
      vga_R = 0;
      vga_G = 0;
      vga_B = 0;
    end
  end

  //----------------------------------------

  always_ff @(posedge clock) begin : tock_game
    tick(tock_game_in_quad_a, tock_game_in_quad_b);
  end

  //----------------------------------------

 /*private:*/
  task automatic tick(logic in_quad_a, logic in_quad_b);
    logic[9:0] new_px;
    logic[9:0] new_py;
    logic quad_dir;
    logic quad_step;
    logic[9:0] new_pad_x;
    logic[9:0] new_pad_y;
    logic[9:0] new_ball_x;
    logic[9:0] new_ball_y;
    logic new_ball_dx;
    logic new_ball_dy;
    new_px = px_ + 1;
    new_py = py_;

    //----------
    // Update screen coord

    if (new_px == 800) begin
      new_px = 0;
      new_py = new_py + 1;
    end

    if (new_py == 525) begin
      new_py = 0;
    end

    //----------
    // Update quadrature encoder

    quad_dir = quad_a_[1] ^ quad_b_[0];
    quad_step = quad_a_[1] ^ quad_a_[0] ^ quad_b_[1] ^ quad_b_[0];

    new_pad_x = pad_x_;
    new_pad_y = pad_y_;

    if (quad_step) begin
      new_pad_x = pad_x_ + quad_dir ? 1 : 0;
      if (new_pad_x < 120) new_pad_x = 120;
      if (new_pad_x > 520) new_pad_x = 520;
    end

    //----------
    // Update in_ball

    new_ball_x = ball_x_;
    new_ball_y = ball_y_;
    new_ball_dx = ball_dx_;
    new_ball_dy = ball_dy_;

    if (in_border() | in_paddle()) begin
      if ((px_ == ball_x_ - 7) && (py_ == ball_y_ + 0)) new_ball_dx = 1;
      if ((px_ == ball_x_ + 7) && (py_ == ball_y_ + 0)) new_ball_dx = 0;
      if ((px_ == ball_x_ + 0) && (py_ == ball_y_ - 7)) new_ball_dy = 1;
      if ((px_ == ball_x_ + 0) && (py_ == ball_y_ + 7)) new_ball_dy = 0;
    end

    if (new_px == 0 && new_py == 0) begin
      new_ball_x = ball_x_ + (new_ball_dx ? 1 : -1);
      new_ball_y = ball_y_ + (new_ball_dy ? 1 : -1);
    end

    //----------
    // Commit

    px_ <= new_px;
    py_ <= new_py;

    pad_x_ <= new_pad_x;
    pad_y_ <= new_pad_y;

    ball_x_ <= new_ball_x;
    ball_y_ <= new_ball_y;

    ball_dx_ <= new_ball_dx;
    ball_dy_ <= new_ball_dy;

    quad_a_ <= quad_a_ << 1 | in_quad_a;
    quad_b_ <= quad_b_ << 1 | in_quad_b;
  endtask

  //----------------------------------------

  function logic in_border() /*const*/;
    in_border = (px_ <= 7) || (px_ >= 633) || (py_ <= 7) || (py_ >= 473);
  endfunction

  function logic in_paddle() /*const*/;
    in_paddle = (px_ >= pad_x_ - 63) && (px_ <= pad_x_ + 63) && (py_ >= pad_y_ - 3) &&
           (py_ <= pad_y_ + 3);
  endfunction

  function logic in_ball() /*const*/;
    in_ball = (px_ >= ball_x_ - 7) && (px_ <= ball_x_ + 7) && (py_ >= ball_y_ - 7) &&
           (py_ <= ball_y_ + 7);
  endfunction

  function logic in_checker() /*const*/;  in_checker = px_[3] ^ py_[3]; endfunction

  logic[9:0] px_;
  logic[9:0] py_;

  logic[9:0] pad_x_;
  logic[9:0] pad_y_;

  logic[9:0] ball_x_;
  logic[9:0] ball_y_;

  logic ball_dx_;
  logic ball_dy_;

  logic[1:0] quad_a_;
  logic[1:0] quad_b_;
endmodule

//------------------------------------------------------------------------------

`endif
