`include "metron/tools/metron_tools.sv"

// Structs can be used as input/output ports to submodules.
// ...but they have to be public member variables because Yosys...

package TL;
  int PutFullData = 0;
  int PutPartialData = 1;
  int Get = 4;
  int AccessAck = 0;
  int AccessAckData = 1;
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
  input logic clock,
  // output signals
  output tilelink_d tld,
  output tilelink_a tla
);
/*public:*/

  initial begin
    test_reg = 0;
    oe = 0;
  end


  always_comb begin : tock
    if (oe) begin
      tld_d_opcode = TL::AccessAckData;
      tld_d_data   = test_reg;
      tld_d_valid  = 1;
    end
    else begin
      tld_d_opcode = TL::AccessAckData;
      tld_d_data   = 'x;
      tld_d_valid  = 0;
    end
  end

  always_ff @(posedge clock) begin : tick
    if (tla_a_address == 16'h1234) begin
      if (tla_a_opcode == TL::PutFullData && tla_a_valid) begin
        logic[31:0] mask;
        mask = {{8 {tla_a_mask[0]}},
          {8 {tla_a_mask[1]}},
          {8 {tla_a_mask[2]}},
          {8 {tla_a_mask[3]}}};
        test_reg <= (test_reg & ~mask) | (tla_a_data & mask);
      end else if (tla_a_opcode == TL::Get) begin
        oe <= 1;
      end
    end
  end

/*private:*/
  logic[31:0] test_reg;
  logic  oe;
endmodule

//------------------------------------------------------------------------------

module TilelinkCPU (
  // global clock
  input logic clock,
  // output signals
  output tilelink_a tla,
  output tilelink_d tld
);
/*public:*/


  initial begin
    addr = 16'h1234;
    data = 16'h4321;
  end

  always_comb begin : tock
    if (data & 1) begin
      tla_a_opcode  = TL::Get;
      tla_a_address = addr;
      tla_a_mask    = 4'b1111;
      tla_a_data    = 'x;
      tla_a_valid   = 1;
    end else begin
      tla_a_opcode  = TL::PutFullData;
      tla_a_address = addr;
      tla_a_mask    = 4'b1111;
      tla_a_data    = 32'hDEADBEEF;
      tla_a_valid   = 1;
    end
  end

  always_ff @(posedge clock) begin : tick
    if (tld_d_opcode == TL::AccessAckData && tld_d_valid) begin
      data <= tld_d_data;
    end
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
    /*cpu.tock();*/
    /*dev.tock();*/

    cpu_tld = dev_tld;
    dev_tla = cpu_tla;

    /*cpu.tick();*/
    /*dev.tick();*/
  end

  TilelinkCPU cpu;
  TilelinkDevice dev;
endmodule

//------------------------------------------------------------------------------
