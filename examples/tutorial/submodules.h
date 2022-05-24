#include "adder.h"
#include "counter.h"

class ModuleWithSubmodules {
public:
  int update() {
    // The field "my_counter.count" is a register, which means we can only read
    // it _before_ it is written. Uncomment this line to see the error if we
    // call update() on the counter before we send its value to the adder.
    // my_counter.update();

    int result = my_adder.add(my_counter.count, 7);

    // Our adder's "add" function can only be used _once_. Calling it a second
    // time is an error - uncomment this line to see the error.
    // int result2 = my_adder.add(3, 4);

    my_counter.update();
    return result;
  }

private:
  Adder my_adder;
  Counter my_counter;
};
