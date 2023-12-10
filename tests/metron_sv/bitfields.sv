`include "metron/metron_tools.sv"

// C structs with bit-size annotations become arrays of sized logic<>s.

typedef struct packed {
  logic[6:0] op;
  logic[4:0] rd;
  logic[2:0] f3;
  logic[4:0] rs1;
  logic[4:0] rs2;
  logic[6:0] f7;
} rv32_rtype;

typedef struct packed {
  logic[6:0] op;
  logic[4:0] rd;
  logic[2:0] f3;
  logic[4:0] rs1;
  logic[11:0] imm_11_0;
} rv32_itype;

typedef struct packed {
  logic[6:0] op;
  logic[4:0] imm_4_0;
  logic[2:0] f3;
  logic[4:0] rs1;
  logic[4:0] rs2;
  logic[6:0] imm_11_5;
} rv32_stype;

typedef struct packed {
  logic[6:0] op;
  logic[0:0] imm_11;
  logic[3:0] imm_4_1;
  logic[2:0] f3;
  logic[4:0] rs1;
  logic[4:0] rs2;
  logic[5:0] imm_10_5;
  logic[0:0] imm_12;
} rv32_btype;

typedef struct packed {
  logic[6:0] op;
  logic[4:0] rd;
  logic[19:0] imm_31_12;
} rv32_utype;

typedef struct packed {
  logic[6:0] op;
  logic[4:0] rd;
  logic[7:0] imm_19_12;
  logic[0:0] imm_11;
  logic[9:0] imm_10_1;
  logic[0:0] imm_20;
} rv32_jtype;

typedef union packed {
  logic[31:0] raw;
  rv32_rtype r;
  rv32_itype i;
  rv32_stype s;
  rv32_btype b;
  rv32_utype u;
  rv32_jtype j;
} rv32_insn;

module Module (
  // output signals
  output logic[31:0] raw,
  // tock() ports
  input logic[31:0] tock_x
);
  /*public:*/

  always_comb begin : tock
    blah.raw = tock_x;
  end

  rv32_insn blah;
endmodule
