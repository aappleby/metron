`include "metron/tools/metron_tools.sv"

module Submod1 (
);
/*public:*/

endmodule

module Module (
);
/*public:*/

  always_comb begin : tock_foo1
    /*tock_foo2();*/
    /*tock_foo2();*/
    /*tock_foo2();*/
  end

/*private:*/

  always_comb begin : tock_foo2
    /*tock_foo3();*/
  end

  always_comb begin : tock_foo3
    /*tock_foo4();*/
  end

  always_comb begin : tock_foo4
  end


  Submod1 x(
  );
  Submod1 y(
  );
  Submod1 z(
  );
endmodule
