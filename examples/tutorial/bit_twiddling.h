#include "metron_tools.h"

class Module {
  // Private only so we don't spam our port list for this example
private:

  // Logic types can be indexed like arrays that return individual bits.
  logic<1> extract_bit_four(logic<16> a) {
    return a[4];
  }

  // bN(x, offset) is shorthand for bx<N>(x, offset) which extracts a slice
  // of the bits in a field. Slices that overflow the source will be padded
  // with 0.
  logic<3> extract_three_bits_starting_at_bit_four(logic<16> a) {
    return b3(a, 4);
  }

  // Logic types can be concatenated together, and the result's bit width will
  // be the sum of the bit widths of the arguments.
  logic<3> concatenate(logic<1> a, logic<1> b, logic<1> c) {
    return cat(a, b, c);
  }

  // Logic types can be duplicated, which is equivalent to cat(x, x, x...). The
  // result's bit width will be the product of the argument width and the dup
  // count.
  logic<9> duplicate_three_times(logic<3> a) {
    return dup<3>(a);
  }
};
