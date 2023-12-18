`include "metron/metron_tools.sv"
`include "dpram.sv"

module J1
(
  // global clock
  input logic clock,
  // input signals
  input logic[15:0] io_din,
  // output registers
  output logic io_rd_,
  output logic io_wr_,
  output logic[15:0] io_addr_,
  output logic[15:0] io_dout_,
  // tock() ports
  input logic tock_sys_rst_i
);
/*public:*/


  typedef enum {
    OP_JUMP = 32'b000,
    OP_CJMP = 32'b001,
    OP_CALL = 32'b010,
    OP_ALU  = 32'b011,
  };

  always_comb begin : tock
    logic[4:0]  _dsp;
    logic[15:0] _st0;
    logic  _dstkW;
    logic  _rstkW;
    logic[15:0] _rstkD;
    logic[12:0] _pc;
    logic[4:0]  _rsp;
    logic[15:0] insn;
    logic[15:0] ramrd;
    logic[15:0] dd;
    logic[15:0] rd;
    logic  op_store;
    logic[2:0] opcode;
    logic is_lit;
    logic[15:0] st1;
    logic[3:0] st0sel;
    logic[15:0] rst0;
    logic _ramWE;


    insn = dpram_porta_dout_;
    ramrd = dpram_portb_dout_;

    dd = $signed(2'(insn));
    rd = $signed(insn[3:2]);
    op_store = insn[5];

    opcode = insn[15:13];
    is_lit = insn[15];
    st1 = dstack_[dsp_];

    case(insn[14:13])
      0:  st0sel = 0; /*break;*/
      1:  st0sel = 0; /*break;*/
      2:  st0sel = 1; /*break;*/
      3:  st0sel = insn[11:8]; /*break;*/
    endcase

    // Compute the new value of T.
    rst0 = rstack_[rsp_];

    if (is_lit) begin
      _st0 = {1'd0, 15'(insn)};
    end
    else begin
      case (st0sel)
        4'b0000: _st0 = st0_; /*break;*/
        4'b0001: _st0 = st1; /*break;*/
        4'b0010: _st0 = st0_ + st1; /*break;*/
        4'b0011: _st0 = st0_ & st1; /*break;*/
        4'b0100: _st0 = st0_ | st1; /*break;*/
        4'b0101: _st0 = st0_ ^ st1; /*break;*/
        4'b0110: _st0 = ~st0_; /*break;*/
        4'b0111: _st0 = {16 {1'(st1 == st0_)}}; /*break;*/
        4'b1000: _st0 = {16 {1'($signed(st1) < $signed(st0_))}}; /*break;*/
        4'b1001: _st0 = st1 >> 4'(st0_); /*break;*/
        4'b1010: _st0 = st0_ - 1; /*break;*/
        4'b1011: _st0 = rst0; /*break;*/
        4'b1100: _st0 = st0_[15:14] ? io_din : ramrd; /*break;*/
        4'b1101: _st0 = st1 << 4'(st0_); /*break;*/
        4'b1110: _st0 = {rsp_, 3'd0, dsp_}; /*break;*/
        4'b1111: _st0 = {16 {1'(st1 < st0_)}}; /*break;*/
        default:     _st0 = 'x; /*break;*/
      endcase
    end

    if (is_lit) begin
      _dsp = dsp_ + 1;
      _rsp = rsp_;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = pc_ + 1;
    end
    else if (opcode == OP_JUMP) begin
      _dsp = dsp_;
      _rsp = rsp_;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = 13'(insn);
    end
    else if (opcode == OP_CJMP) begin
      // predicated jump is like DROP
      _dsp = dsp_ - 1;
      _rsp = rsp_;
      _rstkW = 0;
      _rstkD = _pc;
      _pc = st0_ ? 13'(pc_ + 1) : 13'(insn);
    end
    else if (opcode == OP_CALL) begin
      _dsp = dsp_;
      _rsp = rsp_ + 1;
      _rstkW = 1;
      _rstkD = {15'(pc_ + 1), 1'd0};
      _pc = 13'(insn);
    end
    else if (opcode == OP_ALU) begin
      _dsp = dsp_ + $signed(2'(insn));
      _rsp = rsp_ + $signed(insn[3:2]);
      _rstkW = insn[6];
      _rstkD = st0_;
      _pc = insn[12] ? rst0[13:1] : 13'(pc_ + 1);
    end


    _ramWE = (opcode == OP_ALU) & op_store;

    dpram_tick_porta_addr = 16'(pc_);
    dpram_tick_portb_addr = _st0[16:1];
    dpram_tick_portb_write = _ramWE & (_st0[15:14] == 0);
    dpram_tick_portb_data = 16'(st1);
    /*dpram.tick(
      b16(pc_),                      // porta_addr
      b16(_st0, 1),                 // portb_addr
      _ramWE & (b2(_st0, 14) == 0), // portb_write
      b16(st1));*/                       // portb_data

    _dstkW = is_lit | ((opcode == OP_ALU) & insn[7]);

    tick_sys_rst_i = tock_sys_rst_i;
    tick__pc = _pc;
    tick__st0 = _st0;
    tick__dsp = _dsp;
    tick__rsp = _rsp;
    tick__dstkW = _dstkW;
    tick__rstkW = _rstkW;
    tick__rstkD = _rstkD;
    tick_insn = insn;
    /*tick(sys_rst_i, _pc, _st0, _dsp, _rsp, _dstkW, _rstkW, _rstkD, insn);*/
  end

  always_ff @(posedge clock) begin : tick


    if (tick_sys_rst_i) begin
      pc_      <= 0;
      st0_     <= 0;
      dsp_     <= 0;
      rsp_     <= 0;
      io_rd_   <= 0;
      io_wr_   <= 0;
      io_addr_ <= 0;
      io_dout_ <= 0;
    end else begin
      logic[2:0] opcode;
      opcode = tick_insn[15:13];

      io_rd_   <= (opcode == OP_ALU && (tick_insn[11:8] == 4'hc));
      io_wr_   <= opcode == OP_ALU && tick_insn[5];
      io_addr_ <= 16'(st0_);
      io_dout_ <= dstack_[dsp_];

      if (tick__dstkW) dstack_[tick__dsp] <= st0_;
      if (tick__rstkW) rstack_[tick__rsp] <= tick__rstkD;

      pc_  <= tick__pc;
      st0_ <= tick__st0;
      dsp_ <= tick__dsp;
      rsp_ <= tick__rsp;
    end
  end
  logic tick_sys_rst_i;
  logic[12:0] tick__pc;
  logic[15:0] tick__st0;
  logic[4:0] tick__dsp;
  logic[4:0] tick__rsp;
  logic tick__dstkW;
  logic tick__rstkW;
  logic[15:0] tick__rstkD;
  logic[15:0] tick_insn;

/*private:*/

  // The RAM
  DPRam dpram(
    // Global clock
    .clock(clock),
    // Output registers
    .porta_dout_(dpram_porta_dout_),
    .portb_dout_(dpram_portb_dout_),
    // tick() ports
    .tick_porta_addr(dpram_tick_porta_addr),
    .tick_portb_addr(dpram_tick_portb_addr),
    .tick_portb_write(dpram_tick_portb_write),
    .tick_portb_data(dpram_tick_portb_data)
  );
  logic[15:0] dpram_tick_porta_addr;
  logic[15:0] dpram_tick_portb_addr;
  logic dpram_tick_portb_write;
  logic[15:0] dpram_tick_portb_data;
  logic[15:0] dpram_porta_dout_;
  logic[15:0] dpram_portb_dout_;

  // The D and R stacks
  logic[15:0] dstack_[32];
  logic[15:0] rstack_[32];

  logic[12:0] pc_;
  logic[15:0] st0_;
  logic[4:0]  dsp_;
  logic[4:0]  rsp_;

endmodule
