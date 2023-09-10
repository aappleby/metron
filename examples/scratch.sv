`include "metron/tools/metron_tools.sv"

module Module (
);
/*public:*/

  parameter q = 7;
  initial begin
    x = 1;
    y = 2;
    z = 3;
  end

/*private:*/

  logic x;
  logic y;
  logic z;
  logic[1:0] out;
endmodule