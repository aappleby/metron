`ifndef PINWHEEL_RTL_BLOCK_RAM_H
`define PINWHEEL_RTL_BLOCK_RAM_H

`include "metron/metron_tools.sv"
`include "pinwheel/rtl/tilelink.sv"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal
// verilator lint_off unusedparam

module block_ram (
  // global clock
  input logic clock,
  // output signals
  output tilelink_d bus_tld,
  // tick() ports
  input tilelink_a tick_tla
);
  parameter addr_mask = 32'hF0000000;
  parameter addr_tag = 32'h00000000;
/*public:*/

  parameter /*const char**/ filename = "";
  initial begin
    if (filename) $readmemh(filename, data);
  end

  // metron_noconvert
  /*logic<32> get() const { return bus_tld.d_data; }*/

  always_ff @(posedge clock) begin : tick
    bus_tld.d_opcode <= 3'bx;
    bus_tld.d_param  <= 2'bx;
    bus_tld.d_size   <= 3'bx;
    bus_tld.d_source <= 1'bx;
    bus_tld.d_sink   <= 3'bx;
    bus_tld.d_data   <= 32'bx;
    bus_tld.d_error  <= 0;
    bus_tld.d_valid  <= 0;
    bus_tld.d_ready  <= 1;

    if (tick_tla.a_valid && ((tick_tla.a_address & addr_mask) == addr_tag)) begin
      if (tick_tla.a_opcode == TL::PutPartialData) begin
        logic[31:0] old_data;
        logic[31:0] new_data;
        old_data = data[tick_tla.a_address[11:2]];
        new_data = tick_tla.a_data;
        if (tick_tla.a_address[0]) new_data = new_data << 8;
        if (tick_tla.a_address[1]) new_data = new_data << 16;
        new_data = ((tick_tla.a_mask[0] ? new_data : old_data) & 32'h000000FF) |
                  ((tick_tla.a_mask[1] ? new_data : old_data) & 32'h0000FF00) |
                  ((tick_tla.a_mask[2] ? new_data : old_data) & 32'h00FF0000) |
                  ((tick_tla.a_mask[3] ? new_data : old_data) & 32'hFF000000);

        data[tick_tla.a_address[11:2]] <= new_data;
        bus_tld.d_opcode <= TL::AccessAckData;
        bus_tld.d_data <= new_data;
        bus_tld.d_valid <= 1;
      end
      else begin
        bus_tld.d_opcode <= TL::AccessAckData;
        bus_tld.d_data <= data[tick_tla.a_address[11:2]];
        bus_tld.d_valid <= 1;
      end
    end
  end

  // metron_noconvert
  /*uint32_t* get_data() { return (uint32_t*)data; }*/

  // metron_noconvert
  /*size_t data_size() const { return sizeof(data); }*/

  // metron_noconvert
  /*const uint32_t* get_data() const { return (uint32_t*)data; }*/


/*private:*/

  logic[31:0] data[16384];
endmodule

// verilator lint_on unusedsignal
// verilator lint_off unusedparam
//------------------------------------------------------------------------------

`endif // PINWHEEL_RTL_BLOCK_RAM_H
