`include "metron/tools/metron_tools.sv"

//------------------------------------------------------------------------------

typedef struct packed {
  {{9CNodeType}} x;
  {{9CNodeType}} y;
} S;

module Adder (
{{port list}}
);
  {{template parameter list}}
{{9CNodeType}} a;
  {{9CNodeType}} b;
endmodule;

module Top (
{{port list}}
);
{{template parameter list}}
/*public:*/

  localparam string some_string = "foobarbaz";

  {{13CNodeFunction}}
  {{13CNodeFunction}}
  {{9CNodeType}} blarp;
  {{10CNodeField}};
endmodule;

//------------------------------------------------------------------------------
