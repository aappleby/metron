
typedef struct packed {
  logic[31:0]  raw;
} rv32_insn;

module pinwheel_core (
);

  always_comb begin : tadsfadock
    rv32_insn AB_insn;
    AB_insn.raw = 32'd0;
  end


endmodule
