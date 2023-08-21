`include "metron/tools/metron_tools.sv"

//------------------------------------------------------------------------------

typedef struct packed {
  {{CNodeField}};
  {{CNodeField}};
} S;

module Top (
{{port list}}
);
{{template parameter list}}
/*public:*/
  {{CNodeFunction}}
  {{CNodeFunction}}
  {{CNodeField}};
endmodule;

//------------------------------------------------------------------------------
