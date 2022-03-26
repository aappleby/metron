
#include "example_rvsimple/rtl/adder.h"
#include "example_rvsimple/rtl/alu.h"
#include "example_rvsimple/rtl/alu_control.h"
#include "example_rvsimple/rtl/config.h"
#include "example_rvsimple/rtl/constants.h"
#include "example_rvsimple/rtl/control_transfer.h"
#include "example_rvsimple/rtl/data_memory_interface.h"
#include "example_rvsimple/rtl/example_data_memory.h"
#include "example_rvsimple/rtl/example_data_memory_bus.h"
#include "example_rvsimple/rtl/example_text_memory.h"
#include "example_rvsimple/rtl/example_text_memory_bus.h"
#include "example_rvsimple/rtl/immediate_generator.h"
#include "example_rvsimple/rtl/instruction_decoder.h"
#include "example_rvsimple/rtl/multiplexer.h"
#include "example_rvsimple/rtl/multiplexer2.h"
#include "example_rvsimple/rtl/multiplexer4.h"
#include "example_rvsimple/rtl/multiplexer8.h"
#include "example_rvsimple/rtl/regfile.h"
#include "example_rvsimple/rtl/register.h"
#include "example_rvsimple/rtl/riscv_core.h"
#include "example_rvsimple/rtl/singlecycle_control.h"
#include "example_rvsimple/rtl/singlecycle_ctlpath.h"
#include "example_rvsimple/rtl/singlecycle_datapath.h"
#include "example_rvsimple/rtl/toplevel.h"

#include "example_uart/rtl/uart_top.h"


void blah() {
  uart_top<3> top;
  top.update(0);
}

void blee() {
  toplevel top;
  top.init();
  top.tick(0);
  top.tock();
}