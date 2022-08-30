`include "metron_tools.sv"

//------------------------------------------------------------------------------

module Blockram (
  // global clock
  input logic clock,
  // get_data() ports
  output logic[7:0] get_data_ret,
  // tick() ports
  input logic[7:0] tick_address,
  input logic tick_write,
  input logic[7:0] tick_data_in
);
/*public:*/
  initial begin
    $readmemh("blockram.hex", memory, 0, 255);
  end

  always_comb begin : get_data
    get_data_ret = data_out;
  end

  always_ff @(posedge clock) begin : tick
    if (tick_write) begin
      memory[tick_address] <= tick_data_in;
      data_out <= tick_data_in;
    end
    else begin
      data_out <= memory[tick_address];
    end
  end

/*private:*/
  logic[7:0] memory[256];
  logic[7:0] data_out;
endmodule

//------------------------------------------------------------------------------

module Checksum (
  // global clock
  input logic clock,
  // get_checksum() ports
  output int get_checksum_ret,
  // get_done() ports
  output int get_done_ret,
  // tock() ports
  input int tock_reset
);
/*public:*/

  always_comb begin : get_checksum
    get_checksum_ret = sum;
  end

  always_comb begin : get_done
    get_done_ret = cursor == 256;
  end

  always_comb begin : tock
    logic[7:0] data;
    data = ram_get_data_ret;
    ram_tick_address = cursor;
    ram_tick_write = 0;
    ram_tick_data_in = 0;

    tick_reset = tock_reset;
    tick_data = data;
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    if (tick_reset) begin
      cursor <= 0;
      sum <= 0;
    end
    else begin
      if (cursor < 256) begin
        cursor <= cursor + 1;
        sum <= sum + tick_data;
      end
    end
  end
  int tick_reset;
  logic[7:0] tick_data;

  int cursor;
  int sum;
  Blockram ram(
    // global clock
    .clock(clock),
    // get_data() ports
    .get_data_ret(ram_get_data_ret),
    // tick() ports
    .tick_address(ram_tick_address),
    .tick_write(ram_tick_write),
    .tick_data_in(ram_tick_data_in)
  );
  logic[7:0] ram_tick_address;
  logic ram_tick_write;
  logic[7:0] ram_tick_data_in;
  logic[7:0] ram_get_data_ret;

endmodule

//------------------------------------------------------------------------------
