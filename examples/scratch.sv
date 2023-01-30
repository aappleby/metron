`include "metron_tools.sv"

// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

typedef struct packed {
  logic[7:0] a;
  logic[31:0] d[7];
} InnerStruct;

typedef struct packed {
  InnerStruct x;
  InnerStruct y;
} OuterStruct;

module Module (
  // output registers
  output OuterStruct s,
  // func2() ports
  input OuterStruct func2_clarnk,
  output InnerStruct func2_ret
);
/*public:*/

  always_comb begin : func2
    logic[7:0] blep;
    clarnk.x.a = clarnk.x.a + 1;

    clarnk.y.a = 7;
    blep = clarnk.y.a;

    s.y.a = clarnk.y.d[2];
    s.y.d[6] = 5;
    s.y.a = 6;
    func2_ret = s.x;
  end
endmodule
