`include "metron/tools/metron_tools.sv"

// Concatenating logics should produce logics with correct <N>

module Module (
  // output signals
  output logic[5:0] sig1,
  output logic[5:0] sig2
);
/*public:*/

  initial begin
    $write("Hello World?\n");
  end


  always_comb begin : tock1
    a=1;
    b=2;
    c=3;

    sig1 = {a,b,c};
  end

  always_comb begin : tock2
    a=1;
    b=2;
    c=3;

    sig2 = {1'(a),2'(b),3'(c)};
  end
endmodule