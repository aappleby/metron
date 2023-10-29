// RISC-V SiMPLE SV -- common configuration for testbench
// BSD 3-Clause License
// (c) 2017-2021, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef CONFIG_H
`define CONFIG_H

`include "metron/metron_tools.sv"

package rv_config;

// Select ISA extensions
// `define M_MODULE    // multiplication and division

//////////////////////////////////////////
//              Memory config           //
//////////////////////////////////////////

// Program counter initial value
parameter /*static*/ /*const*/ int unsigned INITIAL_PC = 32'h00400000;

// Instruction memory
parameter /*static*/ /*const*/ int unsigned TEXT_BEGIN = INITIAL_PC;
parameter /*static*/ /*const*/ int unsigned TEXT_BITS = 16;
parameter /*static*/ /*const*/ int unsigned TEXT_WIDTH = (1 << TEXT_BITS);
parameter /*static*/ /*const*/ int unsigned TEXT_END = (TEXT_BEGIN + TEXT_WIDTH - 1);

// Data memory
parameter /*static*/ /*const*/ int unsigned DATA_BEGIN = 32'h80000000;
parameter /*static*/ /*const*/ int unsigned DATA_BITS = 17;
parameter /*static*/ /*const*/ int unsigned DATA_WIDTH = (1 << DATA_BITS);
parameter /*static*/ /*const*/ int unsigned DATA_END = (DATA_BEGIN + DATA_WIDTH - 1);

localparam string TEXT_HEX = "add.code.vh";
localparam string DATA_HEX = "add.data.vh";

endpackage  // namespace rv_config

`endif // CONFIG_H
