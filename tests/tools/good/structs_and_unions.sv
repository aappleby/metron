/*
typedef struct packed {
  logic[ 6 :  0] op;
  logic[11 :  7] rd;
  logic[14 : 12] f3;
  logic[19 : 15] rs1;
  logic[24 : 20] rs2;
  logic[31 : 25] f7;
} rv32_rtype;
*/




typedef struct packed {
  logic[6:0] op;
  logic[4:0] rd;
  logic[2:0] f3;
  logic[4:0] rs1;
  logic[4:0] rs2;
  logic[6:0] f7;
} rv32_rtype;

typedef struct packed {
  logic[6:0]  op;
  logic[4:0]  rd;
  logic[2:0]  f3;
  logic[4:0]  rs1;
  logic[11:0] rs2;
} rv32_itype;







typedef union packed {
  struct packed {
    logic [3:0] c;
  } d;
  logic [3:0] a;
  logic [3:0] b;
} union_type;

module UnionTest(input logic clk);

  union_type x;
  always_ff @(posedge clk) begin
    x.a = 4'b1000;
    x.d.c = 4'b1000;
  end

endmodule
