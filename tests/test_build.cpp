
#include "examples/rvsimple/metron/adder.h"
#include "examples/rvsimple/metron/alu.h"
#include "examples/rvsimple/metron/alu_control.h"
#include "examples/rvsimple/metron/config.h"
#include "examples/rvsimple/metron/constants.h"
#include "examples/rvsimple/metron/control_transfer.h"
#include "examples/rvsimple/metron/data_memory_interface.h"
#include "examples/rvsimple/metron/example_data_memory.h"
#include "examples/rvsimple/metron/example_data_memory_bus.h"
#include "examples/rvsimple/metron/example_text_memory.h"
#include "examples/rvsimple/metron/example_text_memory_bus.h"
#include "examples/rvsimple/metron/immediate_generator.h"
#include "examples/rvsimple/metron/instruction_decoder.h"
#include "examples/rvsimple/metron/multiplexer.h"
#include "examples/rvsimple/metron/multiplexer2.h"
#include "examples/rvsimple/metron/multiplexer4.h"
#include "examples/rvsimple/metron/multiplexer8.h"
#include "examples/rvsimple/metron/regfile.h"
#include "examples/rvsimple/metron/register.h"
#include "examples/rvsimple/metron/riscv_core.h"
#include "examples/rvsimple/metron/singlecycle_control.h"
#include "examples/rvsimple/metron/singlecycle_ctlpath.h"
#include "examples/rvsimple/metron/singlecycle_datapath.h"
#include "examples/rvsimple/metron/toplevel.h"

#include "examples/uart/metron/uart_top.h"


void blah() {
  uart_top<3> top;
  top.tock(0);
}

void blee() {
  toplevel top;
  top.init();
  //top.tick(0);
  top.tock(0);
}