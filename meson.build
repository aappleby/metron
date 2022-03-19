project('metron_project', 'c', 'cpp')

incdir = include_directories(
  'tree-sitter/lib/include'
)

ts_srcs = [
  'tree-sitter/lib/src/lib.c',
  'tree-sitter-cpp/src/parser.c',
  'tree-sitter-cpp/src/scanner.cc',
]

mt_srcs = [
  'src/MetronApp.cpp',
  'src/MtCursor.cpp',
  'src/MtMethod.cpp',
  'src/MtModLibrary.cpp',
  'src/MtModule.cpp',
  'src/MtNode.cpp',
  'src/MtSourceFile.cpp',
  'src/Platform.cpp',
]

vl_srcs = [
  '/usr/share/verilator/include/verilated.cpp',
]

metron = executable(
  'metron',
  ts_srcs + mt_srcs,
  override_options: ['cpp_std=c++2a'],
  include_directories : incdir
)

executable(
  'main_rtl',
  ['example_rvsimple/main_rtl.cpp'],
  override_options: ['cpp_std=c++2a'],
  include_directories : include_directories(
    'example_rvsimple/rtl',
    'src'
  ),
)


base_rtl = [
  'adder.h',
  'alu.h',
  'alu_control.h',
  'config.h',
  'constants.h',
  'control_transfer.h',
  'data_memory_interface.h',
  'example_data_memory.h',
  'example_data_memory_bus.h',
  'example_text_memory.h',
  'example_text_memory_bus.h',
  'immediate_generator.h',
  'instruction_decoder.h',
  'multiplexer.h',
  'multiplexer2.h',
  'multiplexer4.h',
  'multiplexer8.h',
  'regfile.h',
  'register.h',
  'riscv_core.h',
  'singlecycle_control.h',
  'singlecycle_datapath.h',
]


#builddir/metron -Rexample_rvsimple/rtl -Oblarnap adder.h alu.h alu_control.h config.h constants.h control_transfer.h data_memory_interface.h example_data_memory.h example_data_memory_bus.h example_text_memory.h example_text_memory_bus.h immediate_generator.h instruction_decoder.h multiplexer.h multiplexer2.h multiplexer4.h multiplexer8.h regfile.h register.h riscv_core.h singlecycle_control.h singlecycle_datapath.h
