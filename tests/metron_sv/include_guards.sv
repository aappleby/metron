// Preprocessor macros generally won't work, but include guards should be OK.

`ifndef INCLUDE_GUARDS_H
`define INCLUDE_GUARDS_H

module Module
(
  // blah() bindings
  output int blah_ret
);
/*public:*/
  function int blah();
    blah = 7;
  endfunction
  always_comb blah_ret = blah();
endmodule

`endif
