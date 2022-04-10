`ifndef RVSIMPLE_TOPLEVEL_H
`define RVSIMPLE_TOPLEVEL_H

`include "metron_tools.sv"

module toplevel
(
  input logic clock,
  output logic[31:0] o_bus_read_data,
  output logic[31:0] o_bus_address,
  output logic[31:0] o_bus_write_data,
  output logic[3:0] o_bus_byte_enable,
  output logic o_bus_read_enable,
  output logic o_bus_write_enable,
  output logic[31:0] o_inst,
  output logic[31:0] o_pc,
  input logic reset
);
  initial begin /*toplevel*/
    string s;

    pc = 0;
    phase = 0;
    inst = 0;
    regs[0] = 32'd0;

    $value$plusargs("text_file=%s", s);
    $readmemh(s, text_mem);

    $value$plusargs("data_file=%s", s);
    $readmemh(s, data_mem);
  end

  always_comb begin /*tock*/ /*tick(reset)*/; end


  //----------------------------------------

  localparam /*const*/ int OP_ALU = 8'h33;
  localparam /*const*/ int OP_ALUI = 8'h13;
  localparam /*const*/ int OP_LOAD = 8'h03;
  localparam /*const*/ int OP_STORE = 8'h23;
  localparam /*const*/ int OP_BRANCH = 8'h63;
  localparam /*const*/ int OP_JAL = 8'h6F;
  localparam /*const*/ int OP_JALR = 8'h67;
  localparam /*const*/ int OP_LUI = 8'h37;
  localparam /*const*/ int OP_AUIPC = 8'h17;

  task tick();
    if (reset) begin
      pc <= 0;
      phase <= 0;
      inst <= 0;
      regs[0] <= 32'd0;
      o_bus_read_data <= 0;
      o_bus_address <= 0;
      o_bus_write_data <= 0;
      o_bus_byte_enable <= 0;
      o_bus_read_enable <= 0;
      o_bus_write_enable <= 0;
      o_inst <= 0;
      o_pc <= 0;
    end else begin
      if (phase == 0) begin
        phase <= 1;
        inst <= text_mem[pc[15:2]];
      end else if (phase == 1) begin
        logic[6:0] op;
        logic[4:0] rd;
        logic[2:0] f3;
        logic[4:0] r1;
        logic[4:0] r2;
        logic[6:0] f7;

        phase <= 0;

        op = inst[6:0];
        rd = inst[11:7];
        f3 = inst[14:12];
        r1 = inst[19:15];
        r2 = inst[24:20];
        f7 = inst[31:25];

        o_bus_address <= 0;
        o_bus_write_enable <= 0;
        o_bus_write_data <= 0;

        //----------
        // Metron simulates this a few percent faster if we don't have ALU and
        // ALUI in the same branch, but then we duplicate the big ALU switch...

        if (op == OP_ALU || op == OP_ALUI) begin
          logic[31:0] op_a;
          logic[31:0] op_b;
          logic[31:0] alu_result;

          op_a = regs[r1];
          op_b =
              op == OP_ALUI ? {{21 {inst[31]}}, inst[30:25], inst[24:20]}
                            : regs[r2];

          case (f3)
            0:
              alu_result = (op == OP_ALU) && f7[5] ? op_a - op_b : op_a + op_b;
            1:
              alu_result = op_a << 5'(op_b);
            2:
              alu_result = $signed(op_a) < $signed(op_b);
            3:
              alu_result = op_a < op_b;
            4:
              alu_result = op_a ^ op_b;
            5: begin
              // FIXME BUG Verilator isn't handling this ternary expression
              // correctly.
              // alu_result = f7[5] ? sra(op_a, b5(op_b)) : b32(op_a >>
              // b5(op_b)); break;
              if (f7[5]) begin
                alu_result = ($signed(op_a) >>> 5'(op_b));
              end else begin
                alu_result = op_a >> 5'(op_b);
              end
            end
            6:
              alu_result = op_a | op_b;
            7:
              alu_result = op_a & op_b;
          endcase

          if (rd) regs[rd] <= alu_result;
          pc <= pc + 4;
        end

        //----------

        else if (op == OP_LOAD) begin
          logic[31:0] imm;
          logic[31:0] addr;
          logic[31:0] rdata;

          imm = {{21 {inst[31]}}, inst[30:25], inst[24:20]};
          addr = regs[r1] + imm;
          rdata = data_mem[addr[16:2]] >> (8 * 2'(addr));

          case (f3)
            0:
              rdata = $signed(8'(rdata));
            1:
              rdata = $signed(16'(rdata));
            4:
              rdata = 8'(rdata);
            5:
              rdata = 16'(rdata);
          endcase

          if (rd) regs[rd] <= rdata;
          pc <= pc + 4;
        end

        //----------

        else if (op == OP_STORE) begin
          logic[31:0] imm;
          logic[31:0] addr;
          logic[31:0] wdata;
          logic[31:0] mask;
          logic[14:0] phys_addr;

          imm = {{21 {inst[31]}}, inst[30:25], inst[11:7]};
          addr = regs[r1] + imm;
          wdata = regs[r2] << (8 * 2'(addr));

          mask = 0;
          if (f3 == 0) mask = 32'h000000FF << (8 * 2'(addr));
          if (f3 == 1) mask = 32'h0000FFFF << (8 * 2'(addr));
          if (f3 == 2) mask = 32'hFFFFFFFF;

          phys_addr = addr[16:2];
          data_mem[phys_addr] <= (data_mem[phys_addr] & ~mask) | (wdata & mask);

          pc <= pc + 4;

          o_bus_address <= addr;
          o_bus_write_enable <= 1;
          o_bus_write_data <= regs[r2];
        end

        //----------

        else if (op == OP_BRANCH) begin
          logic[31:0] op_a;
          logic[31:0] op_b;
          logic take_branch;

          op_a = regs[r1];
          op_b = regs[r2];

          case (f3)
            0:
              take_branch = op_a == op_b;
            1:
              take_branch = op_a != op_b;
            4:
              take_branch = $signed(op_a) < $signed(op_b);
            5:
              take_branch = $signed(op_a) >= $signed(op_b);
            6:
              take_branch = op_a < op_b;
            7:
              take_branch = op_a >= op_b;
            default:
              take_branch = 1'bx;
          endcase

          if (take_branch) begin
            logic[31:0] imm;

            imm = {{20 {inst[31]}}, inst[7], inst[30:25],
                                inst[11:8], 1'd0};
            pc <= pc + imm;
          end else begin
            pc <= pc + 4;
          end
        end

        //----------

        else if (op == OP_JAL) begin
          logic[31:0] imm;

          imm = {{12 {inst[31]}}, inst[19:12], inst[20],
                              inst[30:25], inst[24:21], 1'd0};
          if (rd) regs[rd] <= pc + 4;
          pc <= pc + imm;
        end

        //----------

        else if (op == OP_JALR) begin
          logic[31:0] rr1;
          logic[31:0] imm;

          rr1 = regs[r1]; // Lol, Metron actually found a bug - gotta read r1 before writing
          imm = {{21 {inst[31]}}, inst[30:25], inst[24:20]};
          if (rd) regs[rd] <= pc + 4;
          pc <= rr1 + imm;
        end

        //----------

        else if (op == OP_LUI) begin
          logic[31:0] imm;

          imm = {inst[31], inst[30:20], inst[19:12], 12'd0};
          if (rd) regs[rd] <= imm;
          pc <= pc + 4;
        end

        //----------

        else if (op == OP_AUIPC) begin
          logic[31:0] imm;

          imm = {inst[31], inst[30:20], inst[19:12], 12'd0};
          if (rd) regs[rd] <= pc + imm;
          pc <= pc + 4;
        end
      end
    end
  endtask
  always_ff @(posedge clock) tick();

  logic[31:0] pc;
  logic[1:0] phase;
  logic[31:0] inst;
  logic[31:0] text_mem[32 * 1024];
  logic[31:0] data_mem[32 * 1024];
  logic[31:0] regs[32];
endmodule;

`endif  // RVSIMPLE_TOPLEVEL_H

