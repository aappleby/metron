`include "metron_tools.sv"

// bad
// enum { FOO, BAR, BAZ };
// typedef enum logic[1:0] { FOO=70, BAR=71, BAZ=72 } blem;
// typedef enum { FOO, BAR=0, BAZ=1 } blem;

// good
// enum { FOO, BAR, BAZ } blem;
// enum { FOO=0, BAR=1, BAZ=2 } blem;
// typedef enum { FOO, BAR, BAZ } blem;
// typedef enum { FOO=0, BAR=1, BAZ=2 } blem;
// typedef enum logic[1:0] { FOO, BAR, BAZ } blem;
// typedef enum logic[1:0] { FOO=0, BAR=1, BAZ=2 } blem;


// Simple anonymous enums should work.
// FIXME DIS DOESNT WORK

module Module
(
  input logic clock
);
/*public:*/

  typedef enum { FOO, BAR, BAZ } blem;

  always_comb begin /*tock*/
    logic[1:0] x;
    blem y;

    x = FOO;
    y = BAR;

    if (x == FOO) begin
      y = BAZ;
    end
  end

endmodule

