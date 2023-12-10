`include "metron/metron_tools.sv"

// Structs exist in both C++ and Verilog

typedef struct packed {
  logic[7:0] a;
} MyStruct1;

module Module (
  // input signals
  input MyStruct1 my_struct1,
  // output signals
  output logic[7:0] a,
  // get_something() ports
  output MyStruct1 get_something_ret,
  // tock() ports
  output logic[7:0] tock_ret
);
/*public:*/


  always_comb begin : get_something
    get_something_ret = my_struct1;
  end

  always_comb begin : tock
    my_struct1.a = 1;

    // does not work in yosys
    //return extract_field(my_struct1);
    tock_ret = 16;
  end

/*private:*/

  // does not work in yosys
  /*
  logic<8> extract_field(MyStruct1 m) {
    return m.a;
  }
  */

endmodule
