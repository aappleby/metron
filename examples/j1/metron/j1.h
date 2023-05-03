#include "metron/tools/metron_tools.h"
#include "dpram.h"

class J1 {
public:

  logic<1>  io_rd;
  logic<1>  io_wr;
  logic<16> io_addr;
  logic<16> io_dout;
  logic<16> io_din;

  void tock(logic<1> sys_rst_i) {
    tick(sys_rst_i);
  }

private:

  // The RAM
  DPRam dpram;

  // The D and R stacks
  logic<16> dstack[32];
  logic<16> rstack[32];

  logic<16> insn;
  logic<16> ramrd;

  logic<13> pc;
  logic<16> st0;
  logic<5>  dsp;
  logic<5>  rsp;

  //----------------------------------------

  void tick(logic<1> sys_rst_i) {
    logic<5>  _dsp;
    logic<16> _st0;
    logic<1>  _dstkW;
    logic<1>  _rstkW;
    logic<16> _rstkD;
    logic<13> _pc;
    logic<5>  _rsp;

    enum {
      OP_JUMP = 0b000,
      OP_CJMP = 0b001,
      OP_CALL = 0b010,
      OP_ALU  = 0b011,
    };

    insn = dpram.porta_dout;
    ramrd = dpram.portb_dout;

    logic<16> dd = b2(insn, 0).as_signed();
    logic<16> rd = b2(insn, 2).as_signed();
    logic<1>  op_store = insn[5];

    logic<3> opcode = b3(insn, 13);
    logic<1> is_lit = insn[15];
    logic<16> st1 = dstack[dsp];

    logic<4> st0sel;
    switch(b2(insn, 13)) {
      case 0:  st0sel = 0; break;
      case 1:  st0sel = 0; break;
      case 2:  st0sel = 1; break;
      case 3:  st0sel = b4(insn, 8); break;
    }

    // Compute the new value of T.
    logic<16> st1 = dstack[dsp];
    logic<16> rst0 = rstack[rsp];


    if (is_lit) {
      _st0 = cat(b1(0), b15(insn));
    }
    else {
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
        default:     _st0 = DONTCARE; break;
      }
    }

    if (is_lit) {
      _dsp = dsp + 1;
      _rsp = rsp;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = pc + 1;
    }
    else if (opcode == OP_JUMP) {
      _dsp = dsp;
      _rsp = rsp;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = b13(insn);
    }
    else if (opcode == OP_CJMP) {
      // predicated jump is like DROP
      _dsp = dsp - 1;
      _rsp = rsp;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = st0 ? b13(pc + 1) : b13(insn);
    }
    else if (opcode == OP_CALL) {
      _dsp = dsp;
      _rsp = rsp + 1;
      _rstkW = 1;
      _rstkD = cat(b15(pc + 1), b1(0));
      _pc = b13(insn);
    }
    else if (opcode == OP_ALU) {
      _dsp = dsp + b2(insn, 0).as_signed();
      _rsp = rsp + b2(insn, 2).as_signed();
      _rstkW = insn[6];
      _rstkD = st0;
      _pc = insn[12] ? b13(rst0, 1) : b13(pc + 1);
    }


    logic<1> _ramWE = (opcode == OP_ALU) & op_store;

    dpram.tick(
      b16(pc),                      // porta_addr
      b16(_st0, 1),                 // portb_addr
      _ramWE & (b2(_st0, 14) == 0), // portb_write
      b16(st1));                       // portb_data

    logic<1> _dstkW = is_lit | (opcode == OP_ALU & insn[7]);

    if (_dstkW) dstack[_dsp] = st0;
    if (_rstkW) rstack[_rsp] = _rstkD;

    if (sys_rst_i) {
      pc      = 0;
      st0     = 0;
      dsp     = 0;
      rsp     = 0;
      io_rd   = 0;
      io_wr   = 0;
      io_addr = 0;
      io_dout = 0;
    } else {
      pc  = _pc;
      st0 = _st0;
      dsp = _dsp;
      rsp = _rsp;
      io_rd   = (opcode == OP_ALU && (b4(insn,8) == 0xc));
      io_wr   = opcode == OP_ALU && insn[5];
      io_addr = b16(st0);
      io_dout = dstack[dsp];
    }
  }
};
