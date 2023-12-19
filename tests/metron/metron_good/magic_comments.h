#include "metron/metron_tools.h"

// Comments surrounded by / * # <something # * / get unwrapped and dropped
// directly in the output file.

class Module {
public:

  void tick() {
    my_reg_ = my_reg_ + 1;
  }

/*#
  always @(posedge clock) begin
    //$display("Hello World!\n");
  end
#*/

  int my_reg_;
};
