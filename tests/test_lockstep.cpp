#include <stdio.h>

#include "metron_lockstep/basic_lockstep.h"
#include "metron_vl/Vbasic_lockstep.h"

int main(int argc, char** argv) {

  Vbasic_lockstep vtop;

  vtop.clock = 0;
  vtop.eval();
  vtop.clock = 1;
  vtop.eval();


  printf("Hello Lockstep World\n");
  return 0;
}
