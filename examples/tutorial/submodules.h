#include "adder.h"
#include "counter.h"

class ModuleWithSubmodules {
public:
  int update() {
    // The field "my_counter.count" is a register, which means we can only read
    // it _before_ it is written. Uncomment this line to see the error if we
    // call update() on the counter before we send its value to the adder.
    // my_counter.update();

    // Each branch of our if() can call my_adder.add, but we can only have one
    // call to it total per code path.
    int result;
    if (1) {
      result = my_adder.add(my_counter.count, 7);
    }
    else {
      result = my_adder.add(23, my_counter.count);
    }

    // If we try to call it a second time, Metron will generate an error.
    // int result2 = my_adder.add(3, 4);

    // The counter's update() method is a tick, but the method we're currently
    // in is a tock. This cross-module, cross-tick/tock call is OK.
    my_counter.update();
    return result;
  }

private:
  // Metron will generate and bind variables to our submodules during
  // conversion so that we can "call" methods in them.
  Adder my_adder;
  Counter my_counter;
};
