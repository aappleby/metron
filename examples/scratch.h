#include "metron/metron_tools.h"

// We need to support very basic preprocessor macros or else dealing with mixed
// languages will be a huge pain in the butt.

#ifdef METRON_SV

module Module2(input logic clock);
  logic[7:0] counter;

  always @(posedge clock) begin
    counter <= counter + 1;
  end
endmodule

#else

class Module {
};

#endif
