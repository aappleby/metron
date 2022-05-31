#include "metron_tools.h"

#define RAMS 3

class J1 {
public:

  logic<16> insn;
  logic<16> ramrd;
  logic<1> _ramWE;

  logic<5>  dsp;
  logic<5>  _dsp;  

  logic<16> st0;
  logic<16> _st0;

  logic<1>  _dstkW;
  logic<1>  _rstkW;
  logic<16> _rstkD;

  logic<13> pc;
  logic<13> _pc;
  logic<5> rsp;
  logic<5>  _rsp;

  logic<16> dpram[4096];


#if 0
module j1(
   input sys_clk_i, input sys_rst_i, input [15:0] io_din,
   output io_rd, output io_wr, output [15:0] io_addr, output [15:0] io_dout);
  insn  = dpram[_pc];
  ramrd = dpram[b15(_st0, 1)];

  porta_addr = _pc;

  portb_we   = _ramWE & (b2(_st0, 14) == 0);
  portb_en   = b2(_st0, 14) == 0;
  portb_addr = b15(_st0, 1);
  portb_din  = st1;
#endif

  void blerp() {
    int x = dstack[0];
    int y = rstack[0];
  }

  void tick_stack_writes() {
    logic<1> is_lit = insn[15];
    logic<1> is_alu = b3(insn, 13) == 0b011;
    logic<1> _dstkW = is_lit | (is_alu & insn[7]);

    if (_dstkW)
      dstack[_dsp] = st0;
    if (_rstkW)
      rstack[_rsp] = _rstkD;
  }

  logic<1> get_io_rd() const {
    logic<1> is_alu = b3(insn, 13) == 0b011;
    return (is_alu & (b4(insn,8) == 0xc));
  }

  logic<1> get_io_wr() const {
    logic<1> is_alu = b3(insn, 13) == 0b011;
    return is_alu & insn[5];
  }

  logic<16> get_io_addr() const {
    return st0;
  }

  logic<16> get_io_dout() const {
    logic<16> st1 = dstack[dsp];
    return st1;
  }

  // The D and R stacks
  logic<16> dstack[32];
  logic<16> rstack[32];

  // st0sel is the ALU operation.  For branch and call the operation
  // is T, for 0branch it is N.  For ALU ops it is loaded from the instruction
  // field.
  logic<4> st0sel;
  void tock_st0sel() {
    switch(b2(insn, 13)) {
      case 0:  st0sel = 0; break;
      case 1:  st0sel = 0; break;
      case 2:  st0sel = 1; break;
      case 3:  st0sel = b4(insn, 8); break;
      default: st0sel = b4(DONTCARE); break;
    }
  }

  // Compute the new value of T.
  void tock_alu(logic<16> io_din) {
    logic<16> st1 = dstack[dsp];
    logic<16> rst0 = rstack[rsp];
    logic<16> immediate = cat(b1(0), b15(insn));

    if (insn[15])
      _st0 = immediate;
    else
      switch (st0sel) {
        case 0b0000: _st0 = st0; break;
        case 0b0001: _st0 = st1; break;
        case 0b0010: _st0 = st0 + st1; break;
        case 0b0011: _st0 = st0 & st1; break;
        case 0b0100: _st0 = st0 | st1; break;
        case 0b0101: _st0 = st0 ^ st1; break;
        case 0b0110: _st0 = ~st0; break;
        case 0b0111: _st0 = dup<16>(b1(st1 == st0)); break;
        case 0b1000: _st0 = dup<16>(b1(signed(st1) < signed(st0))); break;
        case 0b1001: _st0 = st1 >> b4(st0); break;
        case 0b1010: _st0 = st0 - 1; break;
        case 0b1011: _st0 = rst0; break;
        case 0b1100: _st0 = b2(st0, 14) ? io_din : ramrd; break;
        case 0b1101: _st0 = st1 << b4(st0); break;
        case 0b1110: _st0 = cat(rsp, b3(0), dsp); break;
        case 0b1111: _st0 = dup<16>(b1(st1 < st0)); break;
        default:     _st0 = b16(DONTCARE); break;
      }
  }

  void tock_stacks() {
    logic<1> is_lit = insn[15];
    logic<1> is_alu = b3(insn, 13) == 0b011;
    logic<2> dd = b2(insn, 0);  // D stack delta
    logic<2> rd = b2(insn, 2);  // R stack delta

    if (is_lit) {
      _dsp = dsp + 1;
      _rsp = rsp;
      _rstkW = 0;
      _rstkD = _pc;
    } else if (is_alu) {
      _dsp = dsp + cat(dd[1], dd[1], dd[1], dd);
      _rsp = rsp + cat(rd[1], rd[1], rd[1], rd);
      _rstkW = insn[6];
      _rstkD = st0;
    } else { 
      // predicated jump is like DROP
      if (b3(insn, 13) == 0b001) {
        _dsp = dsp - 1;
      } else {
        _dsp = dsp;
      }
      if (b3(insn, 13) == 0b010) { // call
        _rsp = rsp + 1;
        _rstkW = 1;
        _rstkD = cat(b15(pc + 1), b1(0));
      } else {
        _rsp = rsp;
        _rstkW = 0;
        _rstkD = _pc;
      }
    }
  }

  logic<1> sys_rst_i;

  void tock_pc() {
    logic<1> is_alu = b3(insn, 13) == 0b011;
    logic<16> rst0 = rstack[rsp];

    if (sys_rst_i) {
      _pc = pc;
    } else {
      if ((b3(insn, 13) == 0b000) |
          ((b3(insn, 13) == 0b001) & (st0 == 0)) |
          (b3(insn, 13) == 0b010)) {
        _pc = b13(insn);
      } else if (is_alu & insn[12]) {
        _pc = b13(rst0, 1);
      } else {
        _pc = pc + 1;
      }
    }
  }

  void tick_regs() {
    if (sys_rst_i) {
      pc  = 0;
      st0 = 0;
      dsp = 0;
      rsp = 0;
    } else {
      pc  = _pc;
      st0 = _st0;
      dsp = _dsp;
      rsp = _rsp;
    }
  }


};
