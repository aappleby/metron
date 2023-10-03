`include "metron/metron_tools.sv"

// We should be able to handle constructor args.

module Module (
  // global clock
  input logic clock,
  // tock() ports
  input logic[9:0] tock_addr_,
  // get_data() ports
  output logic[7:0] get_data_ret
);
  parameter data_len = 1024;
  parameter blarp = 7;
/*public:*/

  parameter /*const*/ filename = "examples/uart/message.hex";
  initial begin
    $readmemh(filename, data);
  end

  always_comb begin : tock
    addr = tock_addr_;
  end

  always_ff @(posedge clock) begin : tick
    out <= data[addr];
  end

  always_comb begin : get_data
    get_data_ret = out;
  end

/*private:*/
  logic[9:0] addr;
  logic[7:0] data[data_len];
  logic[7:0] out;
endmodule
