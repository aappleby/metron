`include "metron/metron_tools.sv"

module Module (
  // foo() ports
  output logic foo_ret
);
  /*public:*/

  always_comb begin : foo
    /* metron_noconvert */ /*int x;*/
    foo_ret = 0;
  end

  /* metron_noconvert */ /*void akdslfkajsf(const char* skdfjlskdjf);*/
  /* metron_noconvert */ /*size_t size_bytes();*/
  /* metron_noconvert */ /*void debug_dump() { printf("Hello World %d\n", 1234); }*/
endmodule
