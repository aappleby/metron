`include "metron/tools/metron_tools.sv"

// Structs can be used as input/output ports to submodules.
// ...but they have to be public member variables because Yosys...

package TL;
  /*const*/ int PutFullData = 0;
  /*const*/ int PutPartialData = 1;
  /*const*/ int Get = 4;
  /*const*/ int AccessAck = 0;
  /*const*/ int AccessAckData = 1;
endpackage;

typedef struct packed {
  logic[2:0]  a_opcode;
  logic[31:0] a_address;
  logic[3:0]  a_mask;
  logic[31:0] a_data;
  logic  a_valid;
} tilelink_a;

typedef struct packed {
  logic[2:0]  d_opcode;
  logic[31:0] d_data;
  logic  d_valid;
} tilelink_d;

//------------------------------------------------------------------------------

module TilelinkDevice (
  // global clock
  input logic clock
);
/*public:*/

  {{10CNodeField}};
  {{10CNodeField}};

  always_comb begin : tock
    {{7CNodeIf}}
  end

  always_ff @(posedge clock) begin : tick
    {{7CNodeIf}}
  end

/*private:*/
  logic[31:0] test_reg;
  logic  oe;
endmodule

//------------------------------------------------------------------------------

module TilelinkCPU (
  // global clock
  input logic clock
);
/*public:*/

  {{10CNodeField}};
  {{10CNodeField}};

  always_comb begin : tock
    {{7CNodeIf}}
  end

  always_ff @(posedge clock) begin : tick
    {{7CNodeIf}}
  end

/*private:*/
  logic[31:0] addr;
  logic[31:0] data;
endmodule

//------------------------------------------------------------------------------

module Top (
  // global clock
  input logic clock
);
/*public:*/
  always_comb begin : tock
    /*cpu.tock()*/;
    /*dev.tock()*/;

    cpu_tld  = dev_tld;
    dev_tla  = cpu_tla;

    /*cpu.tick()*/;
    /*dev.tick()*/;
  end

  TilelinkCPU cpu;
  TilelinkDevice dev;
endmodule