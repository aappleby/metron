// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

typedef struct packed {
  logic[7:0] a;
} MyStruct1;

module Foo();

  function MyStruct1 some_func();
    MyStruct1 result;
    result.a = 7;
    some_func = result;
  endfunction

  function MyStruct1 another_func(MyStruct1 my_arg);
    MyStruct1 result;
    result.a = my_arg.a + 1;
    another_func = result;
  endfunction

endmodule
