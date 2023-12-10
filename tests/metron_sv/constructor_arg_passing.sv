`include "metron/metron_tools.sv"

// We should be able to pass constructor arguments to submodules.

//----------------------------------------

module Module (
  // global clock
  input logic clock,
  // tock() ports
  input logic[9:0] tock_new_addr,
  // get_data() ports
  output logic[7:0] get_data_ret
);
  parameter data_len = 1024;
  parameter blarp = 0;
/*public:*/

  parameter /*const char**/ filename = "";
  initial begin
    if (filename) $readmemh(filename, data_);
  end

  always_comb begin : tock
    addr = tock_new_addr;
    /*tick();*/
  end

  always_comb begin : get_data
    get_data_ret = out_;
  end

/*private:*/
  always_ff @(posedge clock) begin : tick
    out_ <= data_[addr];
  end

  logic[9:0] addr;
  logic[7:0] data_[data_len];
  logic[7:0] out_;
endmodule

//----------------------------------------

module Top (
  // global clock
  input logic clock,
  // tock() ports
  input logic[9:0] tock_addr
);
/*public:*/
  initial begin
  end

  always_comb begin : tock
    mod__tock_new_addr = tock_addr;
    /*mod_.tock(addr);*/
  end

  always_comb begin : tick
    //mod.tick();
  end

  Module #(
    // Template Parameters
    .data_len(7777),
    .blarp(8383),
    // Constructor Parameters
    .filename("examples/uart/message.hex")
  ) mod_(
    // Global clock
    .clock(clock),
    // tock() ports
    .tock_new_addr(mod__tock_new_addr),
    // get_data() ports
    .get_data_ret(mod__get_data_ret)
  );
  logic[9:0] mod__tock_new_addr;
  logic[7:0] mod__get_data_ret;
  logic[9:0] derp;
endmodule

//----------------------------------------
