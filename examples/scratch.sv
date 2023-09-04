`include "metron/tools/metron_tools.sv"

// Empty module should pass.

module Module (
{{port list}}
);
  {{10CNodeField}};
  {{10CNodeField}};

  {{13CNodeFunction}}
endmodule;