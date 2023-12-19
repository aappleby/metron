#include "metron/metron_tools.h"

// Modules can use constructors to initialize state

class Module {
public:

  Module() {
    my_reg_ = 7;
  }

  logic<8> get_reg() {
    return my_reg_;
  }

private:

  logic<8> my_reg_;
};

/*#
`ifdef IVERILOG
module Test;
  logic clock;
  logic[7:0] tock;
  Module mod(.clock(clock), .tock(tock));
  initial begin
    if (tock != 7) $display("FAIL");
  end
endmodule
`endif
#*/
