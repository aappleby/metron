`include "metron/tools/metron_tools.sv"

module Submod1 (
  // get_y() ports
  output int get_y_ret,
  // get_z() ports
  output int get_z_ret
);
/*public:*/

  always_comb begin : get_y  get_y_ret = y; end
  always_comb begin : get_z  get_z_ret = z; end

/*private:*/
  int y;
  int z;
endmodule

module Module (
);
/*public:*/

  always_comb begin : tock_foo1
  end

/*private:*/

  Submod1 x(
    // get_y() ports
    .get_y_ret(x_get_y_ret),
    // get_z() ports
    .get_z_ret(x_get_z_ret)
  );
  int x_get_y_ret;
  int x_get_z_ret;
  Submod1 y(
    // get_y() ports
    .get_y_ret(y_get_y_ret),
    // get_z() ports
    .get_z_ret(y_get_z_ret)
  );
  int y_get_y_ret;
  int y_get_z_ret;
  Submod1 z(
    // get_y() ports
    .get_y_ret(z_get_y_ret),
    // get_z() ports
    .get_z_ret(z_get_z_ret)
  );
  int z_get_y_ret;
  int z_get_z_ret;
endmodule
