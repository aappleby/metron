`include "metron_tools.sv"

// Zero-initializing structs should work for convenience.

typedef struct packed {
  logic[7:0] field;
} MyStruct1;

module Module (input logic clock);

  function derp();
    MyStruct1 asdf;
    asdf.field = 1;
    derp = asdf;
  endfunction

  MyStruct1 my_struct1;
  task automatic derp2(logic[7:0] arg);
    MyStruct1 asdf;
    asdf.field = arg;
    my_struct1 = asdf;
    my_struct1.field = 2;
  endtask

  MyStruct1 my_struct2;
  always_comb begin
    MyStruct1 asdf;
    //asdf = my_struct2;
    //my_struct2 = asdf;

    // Yosys breaks if you refer to fields of a local struct...
    //my_struct2.field = asdf.field;
    //asdf.field = my_struct2.field;
  end

endmodule
