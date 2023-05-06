// Preprocessor macros generally won't work, but include guards should be OK.

`ifndef INCLUDE_GUARDS_H
`define INCLUDE_GUARDS_H

module Module (
  // blah() ports
  output int blah_ret
);
/*public*/
  always_comb begin : blah
    blah_ret = 7;
  end
endmodule

`endif
