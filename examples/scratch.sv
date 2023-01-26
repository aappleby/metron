`include "metron_tools.sv"

/* verilator lint_off UNUSEDSIGNAL */
// verilator lint_off undriven

typedef struct packed {
  logic[2:0]  d_opcode;
} tilelink_d;


module pinwheel_core (
  // tock() ports
  input tilelink_d tock_bus_tld
);
/*public:*/
  always_comb begin : tock
  end
endmodule

module pinwheel (
  // global clock
  input logic clock,
  // input signals
  input logic cond_1,
  // output signals
  output tilelink_d bus_tld,
  // output registers
  output tilelink_d bus_tld2
);
/*public:*/

  pinwheel_core core(
    // tock() ports
    .tock_bus_tld(core_tock_bus_tld)
  );
  tilelink_d core_tock_bus_tld;


  always_comb begin : tock
    bus_tld.d_opcode = 3'bx;
    if (cond_1 == 1)  bus_tld = bus_tld2;
    core_tock_bus_tld = bus_tld;
  end

  always_ff @(posedge clock) begin : tick
    bus_tld2.d_opcode <= bus_tld2.d_opcode + 1;
  end
endmodule
