`include "metron/metron_tools.sv"

// Structs can be used as input/output ports to submodules.
// ...but they have to be public member variables because Yosys...

package TL;
  parameter /*static*/ /*const*/ int PutFullData = 0;
  parameter /*static*/ /*const*/ int PutPartialData = 1;
  parameter /*static*/ /*const*/ int Get = 4;
  parameter /*static*/ /*const*/ int AccessAck = 0;
  parameter /*static*/ /*const*/ int AccessAckData = 1;
endpackage

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
  // input signals
  input logic[2:0] a_opcode,
  input logic[31:0] a_address,
  input logic[31:0] a_data,
  input logic a_valid,
  // output signals
  output logic[2:0] d_opcode,
  output logic[31:0] d_data,
  output logic d_valid
);
/*public:*/

  initial begin
    test_reg_ = 0;
    oe_ = 0;
  end


  always_comb begin : tock
    if (oe_) begin
      tld.d_opcode = TL::AccessAckData;
      tld.d_data   = test_reg_;
      tld.d_valid  = 1;
    end
    else begin
      tld.d_opcode = TL::AccessAckData;
      tld.d_data   = 'x;
      tld.d_valid  = 0;
    end
  end

  always_ff @(posedge clock) begin : tick
    if (tla.a_address == 16'h1234) begin
      if (tla.a_opcode == TL::PutFullData && tla.a_valid) begin
        logic[31:0] mask;
        mask = {
          {8 {tla.a_mask[0]}},
          {8 {tla.a_mask[1]}},
          {8 {tla.a_mask[2]}},
          {8 {tla.a_mask[3]}}};
        test_reg_ <= (test_reg_ & ~mask) | (tla.a_data & mask);
        oe_ <= 0;
      end else if (tla.a_opcode == TL::Get) begin
        oe_ <= 1;
      end
    end
  end

/*private:*/
  logic[31:0] test_reg_;
  logic  oe_;
endmodule

//------------------------------------------------------------------------------

module TilelinkCPU (
  // global clock
  input logic clock,
  // input signals
  input logic[2:0] d_opcode,
  input logic[31:0] d_data,
  input logic d_valid,
  // output signals
  output logic[2:0] a_opcode,
  output logic[31:0] a_address,
  output logic[3:0] a_mask,
  output logic[31:0] a_data,
  output logic a_valid
);
/*public:*/


  initial begin
    addr_ = 16'h1234;
    data_ = 16'h4321;
  end

  always_comb begin : tock
    if (data_ & 1) begin
      tla.a_opcode  = TL::Get;
      tla.a_address = addr_;
      tla.a_mask    = 4'b1111;
      tla.a_data    = 'x;
      tla.a_valid   = 1;
    end else begin
      tla.a_opcode  = TL::PutFullData;
      tla.a_address = addr_;
      tla.a_mask    = 4'b1111;
      tla.a_data    = 32'hDEADBEEF;
      tla.a_valid   = 1;
    end
  end

  always_ff @(posedge clock) begin : tick
    if (tld.d_opcode == TL::AccessAckData && tld.d_valid) begin
      data_ <= tld.d_data;
    end
  end

/*private:*/
  logic[31:0] addr_;
  logic[31:0] data_;
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

  TilelinkCPU cpu(
    // Global clock
    .clock(clock),
    // Input signals
    .d_opcode(cpu_d_opcode),
    .d_data(cpu_d_data),
    .d_valid(cpu_d_valid),
    // Output signals
    .a_opcode(cpu_a_opcode),
    .a_address(cpu_a_address),
    .a_mask(cpu_a_mask),
    .a_data(cpu_a_data),
    .a_valid(cpu_a_valid)
  );
  logic[2:0] cpu_d_opcode;
  logic[31:0] cpu_d_data;
  logic cpu_d_valid;
  logic[2:0] cpu_a_opcode;
  logic[31:0] cpu_a_address;
  logic[3:0] cpu_a_mask;
  logic[31:0] cpu_a_data;
  logic cpu_a_valid;
  TilelinkDevice dev(
    // Global clock
    .clock(clock),
    // Input signals
    .a_opcode(dev_a_opcode),
    .a_address(dev_a_address),
    .a_data(dev_a_data),
    .a_valid(dev_a_valid),
    // Output signals
    .d_opcode(dev_d_opcode),
    .d_data(dev_d_data),
    .d_valid(dev_d_valid)
  );
  logic[2:0] dev_a_opcode;
  logic[31:0] dev_a_address;
  logic[31:0] dev_a_data;
  logic dev_a_valid;
  logic[2:0] dev_d_opcode;
  logic[31:0] dev_d_data;
  logic dev_d_valid;
endmodule

//------------------------------------------------------------------------------
