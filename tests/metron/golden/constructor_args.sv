`include "metron/metron_tools.sv"

// We should be able to handle constructor args.

module Module
#(
  parameter data_len = 1024,
  parameter blarp = 7,
  parameter /*const char**/ message_file = ""
)
(
  // global clock
  input logic clock,
  // tock() ports
  input logic[9:0] tock_addr_,
  // get_data() ports
  output logic[7:0] get_data_ret
);
/*public:*/

  initial begin
    if (message_file) $readmemh(message_file, data_);
  end

  always_comb begin : tock
    addr = tock_addr_;
  end

  always_ff @(posedge clock) begin : tick
    out_ <= data_[addr];
  end

  always_comb begin : get_data
    get_data_ret = out_;
  end

/*private:*/
  logic[9:0] addr;
  logic[7:0] data_[data_len];
  logic[7:0] out_;
endmodule
