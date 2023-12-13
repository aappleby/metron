`include "metron/metron_tools.sv"

// Structs exist in both C++ and Verilog

typedef struct packed {
  logic[7:0] a;
} MyStruct1;

module Module (
  // output signals
  output MyStruct1 my_struct1,
  output logic[7:0] tock_ret,
  // get_something() ports
  output MyStruct1 get_something_ret
);
/*public:*/


  always_comb begin : get_something
    get_something_ret = my_struct1;
  end

  always_comb begin : tock
    my_struct1.a = 1;

    // FIXME does not work in yosys
    //return extract_field(my_struct1);
    tock_ret = 16;
  end

/*private:*/

  function logic[7:0] extract_field(MyStruct1 m);
    extract_field = m.a;
  endfunction

endmodule
