#include "metron/metron_tools.h"
#include "dpram.h"

class J1 {
public:

  logic<1>  io_rd_;
  logic<1>  io_wr_;
  logic<16> io_addr_;
  logic<16> io_dout_;
  logic<16> io_din;

  enum {
    OP_JUMP = 0b000,
    OP_CJMP = 0b001,
    OP_CALL = 0b010,
    OP_ALU  = 0b011,
  };

  void tock(logic<1> sys_rst_i) {
    logic<5>  _dsp;
    logic<16> _st0;
    logic<1>  _dstkW;
    logic<1>  _rstkW;
    logic<16> _rstkD;
    logic<13> _pc;
    logic<5>  _rsp;

    logic<16> insn;
    logic<16> ramrd;

    insn = dpram.porta_dout_;
    ramrd = dpram.portb_dout_;

    logic<16> dd = sign_extend<16>(b2(insn, 0));
    logic<16> rd = sign_extend<16>(b2(insn, 2));
    logic<1>  op_store = insn[5];

    logic<3> opcode = b3(insn, 13);
    logic<1> is_lit = insn[15];
    logic<16> st1 = dstack_[dsp_];

    logic<4> st0sel;
    switch(b2(insn, 13)) {
      case 0:  st0sel = 0; break;
      case 1:  st0sel = 0; break;
      case 2:  st0sel = 1; break;
      case 3:  st0sel = b4(insn, 8); break;
    }

    // Compute the new value of T.
    logic<16> rst0 = rstack_[rsp_];

    if (is_lit) {
      _st0 = cat(b1(0), b15(insn));
    }
    else {
      switch (st0sel) {
        case 0b0000: _st0 = st0_; break;
        case 0b0001: _st0 = st1; break;
        case 0b0010: _st0 = st0_ + st1; break;
        case 0b0011: _st0 = st0_ & st1; break;
        case 0b0100: _st0 = st0_ | st1; break;
        case 0b0101: _st0 = st0_ ^ st1; break;
        case 0b0110: _st0 = ~st0_; break;
        case 0b0111: _st0 = dup<16>(b1(st1 == st0_)); break;
        case 0b1000: _st0 = dup<16>(b1(signed(st1) < signed(st0_))); break;
        case 0b1001: _st0 = st1 >> b4(st0_); break;
        case 0b1010: _st0 = st0_ - 1; break;
        case 0b1011: _st0 = rst0; break;
        case 0b1100: _st0 = b2(st0_, 14) ? io_din : ramrd; break;
        case 0b1101: _st0 = st1 << b4(st0_); break;
        case 0b1110: _st0 = cat(rsp_, b3(0), dsp_); break;
        case 0b1111: _st0 = dup<16>(b1(st1 < st0_)); break;
        default:     _st0 = DONTCARE; break;
      }
    }

    if (is_lit) {
      _dsp = dsp_ + 1;
      _rsp = rsp_;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = pc_ + 1;
    }
    else if (opcode == OP_JUMP) {
      _dsp = dsp_;
      _rsp = rsp_;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = b13(insn);
    }
    else if (opcode == OP_CJMP) {
      // predicated jump is like DROP
      _dsp = dsp_ - 1;
      _rsp = rsp_;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = st0_ ? b13(pc_ + 1) : b13(insn);
    }
    else if (opcode == OP_CALL) {
      _dsp = dsp_;
      _rsp = rsp_ + 1;
      _rstkW = 1;
      _rstkD = cat(b15(pc_ + 1), b1(0));
      _pc = b13(insn);
    }
    else if (opcode == OP_ALU) {
      _dsp = dsp_ + sign_extend<5>(b2(insn, 0));
      _rsp = rsp_ + sign_extend<5>(b2(insn, 2));
      _rstkW = insn[6];
      _rstkD = st0_;
      _pc = insn[12] ? b13(rst0, 1) : b13(pc_ + 1);
    }


    logic<1> _ramWE = (opcode == OP_ALU) & op_store;

    dpram.tick(
      b16(pc_),                      // porta_addr
      b16(_st0, 1),                 // portb_addr
      _ramWE & (b2(_st0, 14) == 0), // portb_write
      b16(st1));                       // portb_data

    _dstkW = is_lit | ((opcode == OP_ALU) & insn[7]);

    tick(sys_rst_i, _pc, _st0, _dsp, _rsp, _dstkW, _rstkW, _rstkD, insn);
  }

  void tick(
    logic<1> sys_rst_i,
    logic<13> _pc,
    logic<16> _st0,
    logic<5>  _dsp,
    logic<5>  _rsp,
    logic<1> _dstkW,
    logic<1> _rstkW,
    logic<16> _rstkD,
    logic<16> insn
  ) {


    if (sys_rst_i) {
      pc_      = 0;
      st0_     = 0;
      dsp_     = 0;
      rsp_     = 0;
      io_rd_   = 0;
      io_wr_   = 0;
      io_addr_ = 0;
      io_dout_ = 0;
    } else {
      logic<3> opcode = b3(insn, 13);

      io_rd_   = (opcode == OP_ALU && (b4(insn,8) == 0xc));
      io_wr_   = opcode == OP_ALU && insn[5];
      io_addr_ = b16(st0_);
      io_dout_ = dstack_[dsp_];

      if (_dstkW) dstack_[_dsp] = st0_;
      if (_rstkW) rstack_[_rsp] = _rstkD;

      pc_  = _pc;
      st0_ = _st0;
      dsp_ = _dsp;
      rsp_ = _rsp;
    }
  }

private:

  // The RAM
  DPRam dpram;

  // The D and R stacks
  logic<16> dstack_[32];
  logic<16> rstack_[32];

  logic<13> pc_;
  logic<16> st0_;
  logic<5>  dsp_;
  logic<5>  rsp_;

};
