#include "metron/metron_tools.h"

/*
640x480x60
Pixel clock 25.175 MHz

Screen X    [000-639]
Front porch [640-655]
HSync-      [656-751]
Back porch  [752-799]

Screen Y    [000-479]
Front porch [480-489]
VSync-      [490-491]
Back porch  [492-524]
*/

class VGAOut {
 public:
  logic<1> vga_hsync;
  logic<1> vga_vsync;
  logic<1> vga_R;
  logic<1> vga_G;
  logic<1> vga_B;

  logic<1> in_border() const {
    return (px_ <= 7) || (px_ >= 633) || (py_ <= 7) || (py_ >= 473);
  }

  logic<1> in_checker() const {
    return px_[3] ^ py_[3];
  }

  void update_video() {
    vga_hsync = !((px_ >= 656) && (px_ <= 751));
    vga_vsync = !((py_ >= 490) && (py_ <= 491));

    if ((px_ < 640) && (py_ < 480)) {
      vga_R = in_border() | in_checker();
      vga_G = in_border();
      vga_B = in_border();
    } else {
      vga_R = 0;
      vga_G = 0;
      vga_B = 0;
    }
  }

  void update_counters() {
    logic<10> new_px = px_ + 1;
    logic<10> new_py = py_;

    if (new_px == 800) {
      new_px = 0;
      new_py = new_py + 1;
    }

    if (new_py == 525) {
      new_py = 0;
    }

    px_ = new_px;
    py_ = new_py;
  }

private:
  logic<10> px_;
  logic<10> py_;
};
