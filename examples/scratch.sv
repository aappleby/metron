`include "metron/tools/metron_tools.sv"

// Empty module should pass.

module Module (
{{port list}}
);
  /*
  Module() {
    blep();
  }

  void blep() {
    x = 1;
    y = 2;
  }
  */

  {{10CNodeField}};
  {{10CNodeField}};

  {{13CNodeFunction}}
endmodule;