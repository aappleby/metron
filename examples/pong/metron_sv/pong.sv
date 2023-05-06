`include "metron/tools/metron_tools.sv"

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
 /*public*/

  //----------------------------------------

  initial begin

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
  end

  //----------------------------------------

  always_comb begin : pix_x
 pix_x_ret = px; end
  always_comb begin : pix_y
 pix_y_ret = py; end

  //----------------------------------------

  always_comb begin : tock_video

    vga_hsync = !((px >= 656) && (py <= 751));
    vga_vsync = !((py >= 490) && (py <= 491));

    if ((px < 640) && (py < 480)) begin

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

  always_comb begin : tock_game

    tick_in_quad_a = tock_game_in_quad_a;
    tick_in_quad_b = tock_game_in_quad_b;
  end

  //----------------------------------------

 /*private*/
  always_ff @(posedge clock) begin : tick

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
    new_px = px + 1;
    new_py = py;

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

    quad_dir = quad_a[1] ^ quad_b[0];
    quad_step = quad_a[1] ^ quad_a[0] ^ quad_b[1] ^ quad_b[0];

    new_pad_x = pad_x;
    new_pad_y = pad_y;

    if (quad_step) begin

      new_pad_x = pad_x + quad_dir ? 1 : 0;
      if (new_pad_x < 120) new_pad_x = 120;
      if (new_pad_x > 520) new_pad_x = 520;
    end

    //----------
    // Update in_ball

    new_ball_x = ball_x;
    new_ball_y = ball_y;
    new_ball_dx = ball_dx;
    new_ball_dy = ball_dy;

    if (in_border() | in_paddle()) begin

      if ((px == ball_x - 7) && (py == ball_y + 0)) new_ball_dx = 1;
      if ((px == ball_x + 7) && (py == ball_y + 0)) new_ball_dx = 0;
      if ((px == ball_x + 0) && (py == ball_y - 7)) new_ball_dy = 1;
      if ((px == ball_x + 0) && (py == ball_y + 7)) new_ball_dy = 0;
    end

    if (new_px == 0 && new_py == 0) begin

      new_ball_x = ball_x + (new_ball_dx ? 1 : -1);
      new_ball_y = ball_y + (new_ball_dy ? 1 : -1);
    end

    //----------
    // Commit

    px <= new_px;
    py <= new_py;

    pad_x <= new_pad_x;
    pad_y <= new_pad_y;

    ball_x <= new_ball_x;
    ball_y <= new_ball_y;

    ball_dx <= new_ball_dx;
    ball_dy <= new_ball_dy;

    quad_a <= quad_a << 1 | tick_in_quad_a;
    quad_b <= quad_b << 1 | tick_in_quad_b;
  end
logic tick_in_quad_a;
  logic tick_in_quad_b;
  //----------------------------------------

  function logic in_border();

    in_border = (px <= 7) || (px >= 633) || (py <= 7) || (py >= 473);
  endfunction

  function logic in_paddle();

    in_paddle = (px >= pad_x - 63) && (px <= pad_x + 63) && (py >= pad_y - 3) &&
           (py <= pad_y + 3);
  endfunction

  function logic in_ball();

    in_ball = (px >= ball_x - 7) && (px <= ball_x + 7) && (py >= ball_y - 7) &&
           (py <= ball_y + 7);
  endfunction

  function logic in_checker();
 in_checker = px[3] ^ py[3]; endfunction

  logic[9:0] px;
  logic[9:0] py;

  logic[9:0] pad_x;
  logic[9:0] pad_y;

  logic[9:0] ball_x;
  logic[9:0] ball_y;

  logic ball_dx;
  logic ball_dy;

  logic[1:0] quad_a;
  logic[1:0] quad_b;
endmodule

//------------------------------------------------------------------------------
