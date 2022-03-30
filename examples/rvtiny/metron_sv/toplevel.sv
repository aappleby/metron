`ifndef RVSIMPLE_TOPLEVEL_H
`define RVSIMPLE_TOPLEVEL_H

`include "metron_tools.sv"

module toplevel
(
  input logic clock,
  input logic reset,
  output logic[31:0] o_bus_read_data,
  output logic[31:0] o_bus_address,
  output logic[31:0] o_bus_write_data,
  output logic[3:0]  o_bus_byte_enable,
  output logic  o_bus_read_enable,
  output logic  o_bus_write_enable,
  output logic[31:0] o_inst,
  output logic[31:0] o_pc
);
 /*public:*/

  /*logic<32> o_bus_read_data;*/
  /*logic<32> o_bus_address;*/
  /*logic<32> o_bus_write_data;*/
  /*logic<4>  o_bus_byte_enable;*/
  /*logic<1>  o_bus_read_enable;*/
  /*logic<1>  o_bus_write_enable;*/
  /*logic<32> o_inst;*/
  /*logic<32> o_pc;*/

  localparam /*const*/ int OP_ALU    = 8'h33;
  localparam /*const*/ int OP_ALUI   = 8'h13;
  localparam /*const*/ int OP_LOAD   = 8'h03;
  localparam /*const*/ int OP_STORE  = 8'h23;
  localparam /*const*/ int OP_BRANCH = 8'h63;
  localparam /*const*/ int OP_JAL    = 8'h6F;
  localparam /*const*/ int OP_JALR   = 8'h67;
  localparam /*const*/ int OP_LUI    = 8'h37;
  localparam /*const*/ int OP_AUIPC  = 8'h17;

  initial begin : init
    string s;
    pc = 0;
    regs[0] = 32'd0;

    /*std::string s;*/
    $value$plusargs("text_file=%s", s);
    $readmemh(s, text_mem);

    $value$plusargs("data_file=%s", s);
    $readmemh(s, data_mem);
  end

  always_ff @(posedge clock) begin : tick
    if (reset) begin
      pc <= 0;
      regs[0] = 32'd0;
      o_bus_read_data <= 0;
      o_bus_address <= 0;
      o_bus_write_data <= 0;
      o_bus_byte_enable <= 0;
      o_bus_read_enable <= 0;
      o_bus_write_enable <= 0;
      o_inst <= 0;
      o_pc <= 0;
    end
    else begin
      logic[31:0] inst;
      logic[6:0] op;
      logic[4:0] rd;
      logic[2:0] f3;
      logic[4:0] r1;
      logic[4:0] r2;
      logic[6:0] f7;
      inst = text_mem[pc[15:2]];

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
      // Metron simulates this a few percent faster if we don't have ALU and ALUI in the same branch,
      // but then we duplicate the big ALU switch...

      if (op == OP_ALU || op == OP_ALUI) begin
        logic[31:0] op_a;
        logic[31:0] op_b;
        logic[31:0] alu_result;
        op_a = regs[r1];
        op_b = op == OP_ALUI ? {{21 {inst[31]}}, inst[30:25], inst[24:20]} : regs[r2];
        /*logic<32> alu_result;*/

        case(f3) 
        /*case*/ 0: alu_result = (op == OP_ALU) && f7[5] ? op_a - op_b : op_a + op_b; /*break;*/
        /*case*/ 1: alu_result = op_a << 5'(op_b); /*break;*/
        /*case*/ 2: alu_result = $signed(op_a) < $signed(op_b); /*break;*/
        /*case*/ 3: alu_result = op_a < op_b; /*break;*/
        /*case*/ 4: alu_result = op_a ^ op_b; /*break;*/
        /*case*/ 5: begin
          // FIXME BUG Verilator isn't handling this ternary expression correctly.
          //alu_result = f7[5] ? sra(op_a, b5(op_b)) : b32(op_a >> b5(op_b)); break;
          if (f7[5]) begin
            alu_result = ($signed(op_a) >>> 5'(op_b));
          end
          else begin
            alu_result = op_a >> 5'(op_b);
          end
          /*break;*/
        end
        /*case*/ 6: alu_result = op_a | op_b; /*break;*/
        /*case*/ 7: alu_result = op_a & op_b; /*break;*/
        endcase

        if (rd) regs[rd] = alu_result;
        pc <= pc + 4;
      end

      //----------

      else if (op == OP_LOAD) begin
        logic[31:0] imm;
        logic[31:0] addr;
        logic[31:0] data;
        imm = {{21 {inst[31]}}, inst[30:25], inst[24:20]};
        addr = regs[r1] + imm;
        data = data_mem[addr[16:2]] >> (8 * 2'(addr));

        case (f3) 
        /*case*/ 0:  data = $signed(8'(data));   /*break;*/
        /*case*/ 1:  data = $signed(16'(data));  /*break;*/
        /*case*/ 4:  data = 8'(data); /*break;*/
        /*case*/ 5:  data = 16'(data); /*break;*/
        endcase

        if (rd) regs[rd] = data;
        pc <= pc + 4;
      end

      //----------

      else if (op == OP_STORE) begin
        logic[31:0] imm;
        logic[31:0] addr;
        logic[31:0] data;
        logic[31:0] mask;
        logic[14:0] phys_addr;
        imm = {{21 {inst[31]}}, inst[30:25], inst[11:7]};
        addr = regs[r1] + imm;
        data = regs[r2] << (8 * 2'(addr));

        mask = 0;
        if (f3 == 0) mask = 32'h000000FF << (8 * 2'(addr));
        if (f3 == 1) mask = 32'h0000FFFF << (8 * 2'(addr));
        if (f3 == 2) mask = 32'hFFFFFFFF;

        phys_addr = addr[16:2];
        data_mem[phys_addr] = (data_mem[phys_addr] & ~mask) | (data & mask);

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

        /*logic<1> take_branch;*/
        case (f3) 
        /*case*/ 0:  take_branch = op_a == op_b; /*break;*/
        /*case*/ 1:  take_branch = op_a != op_b; /*break;*/
        /*case*/ 4:  take_branch = $signed(op_a) <  $signed(op_b); /*break;*/
        /*case*/ 5:  take_branch = $signed(op_a) >= $signed(op_b); /*break;*/
        /*case*/ 6:  take_branch = op_a < op_b; /*break;*/
        /*case*/ 7:  take_branch = op_a >= op_b; /*break;*/
        default: take_branch = 1'bx; /*break;*/
        endcase

        if (take_branch) begin
          logic[31:0] imm;
          imm = {{20 {inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'd0};
          pc <= pc + imm;
        end else begin
          pc <= pc + 4;
        end
      end

      //----------

      else if (op == OP_JAL) begin
        logic[31:0] imm;
        imm = {{12 {inst[31]}}, inst[19:12], inst[20], inst[30:25], inst[24:21], 1'd0};
        if (rd) regs[rd] = pc + 4;
        pc <= pc + imm;
      end

      //----------

      else if (op == OP_JALR) begin
        logic[31:0] imm;
        imm = {{21 {inst[31]}}, inst[30:25], inst[24:20]};
        if (rd) regs[rd] = pc + 4;
        pc <= regs[r1] + imm;
      end

      //----------

      else if (op == OP_LUI) begin
        logic[31:0] imm;
        imm = {inst[31], inst[30:20], inst[19:12], 12'd0};
        if (rd) regs[rd] = imm;
        pc <= pc + 4;
      end

      //----------

      else if (op == OP_AUIPC) begin
        logic[31:0] imm;
        imm = {inst[31], inst[30:20], inst[19:12], 12'd0};
        if (rd) regs[rd] = pc + imm;
        pc <= pc + 4;
      end
    end
  end

  //----------------------------------------

/*private:*/
  logic[31:0] text_mem[32*1024];
  logic[31:0] data_mem[32*1024];
  logic[31:0] pc;
  logic[31:0] regs[32];
endmodule;


`endif  // RVSIMPLE_TOPLEVEL_H

