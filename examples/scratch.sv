`ifndef PINHWEEL_SOC_REGFILE_H
`define PINHWEEL_SOC_REGFILE_H

`include "metron/metron_tools.sv"
//#include "pinwheel/tools/regfile_if.h"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal

module ram4k
#(
  parameter /*const char**/ filename = ""
)
(
  // global clock
  input logic clock,
  // output registers
  output logic[15:0] rdata_,
  // tick() ports
  input logic[10:0] tick_raddr,
  input logic[10:0] tick_waddr,
  input logic[15:0] tick_wdata,
  input logic tick_wren
);
/*public:*/

  initial begin
    if (filename) begin
      $readmemh(filename, ram_);
    end
  end

  // metron_noemit
  /*void tick(logic<11> raddr, logic<11> waddr, logic<16> wdata, logic<1> wren) {
    rdata_ = raddr == waddr ? wdata : ram_[raddr];
    if (wren) ram_[waddr] = wdata;
  }*/


  SB_RAM40_4K my_ram(
    .RCLK(clock), .RADDR(tick_raddr), .RDATA(rdata_),
    .WCLK(clock), .WADDR(tick_waddr), .WDATA(tick_wdata), .WE(tick_wren),
  );


  logic[15:0] ram_[255];
endmodule

module regfile
(
  // global clock
  input logic clock,
  // tick() ports
  input logic[10:0] tick_raddr1,
  input logic[10:0] tick_raddr2,
  input logic[10:0] tick_waddr,
  input logic[15:0] tick_wdata,
  input logic tick_wren,
  // get_rdata1() ports
  output logic[15:0] get_rdata1_ret,
  // get_rdata2() ports
  output logic[15:0] get_rdata2_ret
);
/*public:*/

  always_comb begin : tick
    ram1__tick_raddr = tick_raddr1;
    ram1__tick_waddr = tick_waddr;
    ram1__tick_wdata = tick_wdata;
    ram1__tick_wren = tick_wren;
    /*ram1_.tick(raddr1, waddr, wdata, wren);*/
    ram2__tick_raddr = tick_raddr1;
    ram2__tick_waddr = tick_waddr;
    ram2__tick_wdata = tick_wdata;
    ram2__tick_wren = tick_wren;
    /*ram2_.tick(raddr1, waddr, wdata, wren);*/
  end

  always_comb begin : get_rdata1
    get_rdata1_ret = ram1__rdata_;
  end

  always_comb begin : get_rdata2
    get_rdata2_ret = ram2__rdata_;
  end

/*private:*/

  ram4k ram1_(
    // Global clock
    .clock(clock),
    // Output registers
    .rdata_(ram1__rdata_),
    // tick() ports
    .tick_raddr(ram1__tick_raddr),
    .tick_waddr(ram1__tick_waddr),
    .tick_wdata(ram1__tick_wdata),
    .tick_wren(ram1__tick_wren)
  );
  logic[10:0] ram1__tick_raddr;
  logic[10:0] ram1__tick_waddr;
  logic[15:0] ram1__tick_wdata;
  logic ram1__tick_wren;
  logic[15:0] ram1__rdata_;
  ram4k ram2_(
    // Global clock
    .clock(clock),
    // Output registers
    .rdata_(ram2__rdata_),
    // tick() ports
    .tick_raddr(ram2__tick_raddr),
    .tick_waddr(ram2__tick_waddr),
    .tick_wdata(ram2__tick_wdata),
    .tick_wren(ram2__tick_wren)
  );
  logic[10:0] ram2__tick_raddr;
  logic[10:0] ram2__tick_waddr;
  logic[15:0] ram2__tick_wdata;
  logic ram2__tick_wren;
  logic[15:0] ram2__rdata_;
endmodule


//------------------------------------------------------------------------------
// verilator lint_on unusedsignal

`endif // PINHWEEL_SOC_REGFILE_H
