`include "metron/tools/metron_tools.sv"

// Structs exist in both C++ and Verilog

typedef struct packed {
  logic[7:0] a;
} MyStruct1;

module Module (
{{port list}}
);
/*public:*/

  {{10CNodeField}};

  {{13CNodeFunction}}
endmodule;