#include <stdio.h>

#include "MetroBoySPU2.h"
#include "gb_spu/VMetroBoySPU2.h"

// Doesn't do anything yet, just here to validate that the metron + verilate +
// compile + link steps work OK.

int main(int argc, char** argv) {
  VMetroBoySPU2 top;
  top.eval();
  printf("Hello World, VMetroBoySPU2 is %ld bytes\n", sizeof(top));
}
