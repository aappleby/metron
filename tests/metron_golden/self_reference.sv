`include "metron/metron_tools.sv"

// A module should be able to have a noconvert-tagged method that returns a
// pointer to its type

module Module (
);
/*public:*/
  /* metron_noconvert */ /*Module* get_this();*/
endmodule
