`include "metron_tools.sv"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

// X If statements that contain component calls must use {}

module Submod
(
);
 /*public:*/
  function tock(); endfunction
  always_comb tock();
endmodule

module Module
(
);
 /*public:*/
  function tock();
    if (1)
      endfunction
  always_comb tock();

  Submod submod(
  );

endmodule
