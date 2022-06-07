`include "metron_tools.sv"

typedef logic[15:0] sample_t;

/*

#pragma warning(push)
#pragma warning(disable:4201)

#pragma pack(push, 1)
struct Req {
  uint16_t addr = 0;
  union {
    struct {
      uint8_t data_lo;
      uint8_t data_hi;
    };
    uint16_t data = 0;
  };
  uint8_t  read = 0;
  uint8_t  write = 0;
  uint16_t pad1 = 0;

  operator bool() const {
    return read || write;
  }
};
#pragma pack(pop)

static_assert(sizeof(Req) == 8, "Req size != 8");

struct Ack {
  uint16_t addr = 0;
  union {
    struct {
      uint8_t data_lo;
      uint8_t data_hi;
    };
    uint16_t data = 0;
  };
  uint8_t  read = 0;
  uint8_t  pad1 = 0;
  uint16_t pad2 = 0;
};

static_assert(sizeof(Ack) == 8, "Ack size != 8");

#pragma warning(pop)

struct Req {
  logic<16> addr;
  logic<8> data_lo;
  logic<8> data_hi;
  logic<8> read;
  logic<8> write;
};

struct Ack {
  logic<16> addr;
  //logic<1> read;
  //logic<1> write;
  logic<8> read;
  logic<8> write;

  logic<8> data_lo;
  logic<8> data_hi;
};
*/

//-----------------------------------------------------------------------------

module MetroBoySPU (
  // global clock
  input logic clock,
  // output signals
  output logic[15:0] out_r,
  output logic[15:0] out_l,
  output logic[7:0] ack_data,
  output logic ack_valid,
  // tock_ack() ports
  input logic[15:0] tock_ack_addr,
  // tick() ports
  input logic tick_reset,
  input logic[15:0] tick_addr,
  input logic[7:0] tick_data,
  input logic tick_we
);
/*public:*/



  always_comb begin : tock_ack
    ack_data = 0;
    ack_valid = 0;

    if (tock_ack_addr >= 16'hFF30 && tock_ack_addr <= 16'hFF3F) begin
      // wavetable
      ack_data = s3_wave[tock_ack_addr & 4'hF];
    end
    else if (tock_ack_addr >= 16'hFF10 && tock_ack_addr <= 16'hFF26) begin
      case (tock_ack_addr)
        16'hFF10: ack_data = nr10 | 8'h80;
        16'hFF11: ack_data = nr11 | 8'h3F;
        16'hFF12: ack_data = nr12 | 8'h00;
        16'hFF13: ack_data = nr13 | 8'hFF;
        16'hFF14: ack_data = nr14 | 8'hBF;

        16'hFF15: ack_data = nr20 | 8'hFF;
        16'hFF16: ack_data = nr21 | 8'h3F;
        16'hFF17: ack_data = nr22 | 8'h00;
        16'hFF18: ack_data = nr23 | 8'hFF;
        16'hFF19: ack_data = nr24 | 8'hBF;

        16'hFF1A: ack_data = nr30 | 8'h7F;
        16'hFF1B: ack_data = nr31 | 8'hFF;
        16'hFF1C: ack_data = nr32 | 8'h9F;
        16'hFF1D: ack_data = nr33 | 8'hFF;
        16'hFF1E: ack_data = nr34 | 8'hBF;

        16'hFF1F: ack_data = nr40 | 8'hFF;
        16'hFF20: ack_data = nr41 | 8'hFF;
        16'hFF21: ack_data = nr42 | 8'h00;
        16'hFF22: ack_data = nr43 | 8'h00;
        16'hFF23: ack_data = nr44 | 8'hBF;

        16'hFF24: ack_data = nr50 | 8'h00;
        16'hFF25: ack_data = nr51 | 8'h00;

        16'hFF26: begin
          logic[7:0] bus_out_;
          bus_out_ = (nr52 & 8'h80) | 8'h70;
          if (s1_enable) bus_out_ = bus_out_ | 8'b00000001;
          if (s2_enable) bus_out_ = bus_out_ | 8'b00000010;
          if (s3_enable) bus_out_ = bus_out_ | 8'b00000100;
          if (s4_enable) bus_out_ = bus_out_ | 8'b00001000;
          ack_data = bus_out_;
        end
      endcase
    end

  end

/*public:*/

  always_comb begin : tock_out
    logic[15:0] out_r_;
    logic[15:0] out_l_;
    logic[3:0] volume_r;
    logic[3:0] volume_l;
    out_l = 0;
    out_r = 0;

    //----------
    // mixer & master volume

    out_r_ = 0;
    out_l_ = 0;

    if (nr51 & 8'b00000001) out_r_ = out_r_ + s1_out;
    if (nr51 & 8'b00000010) out_r_ = out_r_ + s2_out;
    if (nr51 & 8'b00000100) out_r_ = out_r_ + s3_out;
    if (nr51 & 8'b00001000) out_r_ = out_r_ + s4_out;
    if (nr51 & 8'b00010000) out_l_ = out_l_ + s1_out;
    if (nr51 & 8'b00100000) out_l_ = out_l_ + s2_out;
    if (nr51 & 8'b01000000) out_l_ = out_l_ + s3_out;
    if (nr51 & 8'b10000000) out_l_ = out_l_ + s4_out;

    volume_r = ((nr50 & 8'b00000111) >> 0) + 1;
    volume_l = ((nr50 & 8'b01110000) >> 4) + 1;

    out_r = out_r_ * volume_r;
    out_l = out_l_ * volume_l;
  end

  always_ff @(posedge clock) begin : tick
    if (tick_reset) begin
      int i;
      nr10 <= 8'h80;
      nr11 <= 8'hBF;
      nr12 <= 8'hF3;
      nr13 <= 8'hFF;
      nr14 <= 8'hBF;

      nr20 <= 8'h00;
      nr21 <= 8'h3F;
      nr22 <= 8'h00;
      nr23 <= 8'hFF;
      nr24 <= 8'hBF;

      nr30 <= 8'h7F;
      nr31 <= 8'hFF;
      nr32 <= 8'h9F;
      nr33 <= 8'hFF;
      nr34 <= 8'hBF;

      nr40 <= 8'h80;
      nr41 <= 8'hFF;
      nr42 <= 8'h00;
      nr43 <= 8'h00;
      nr44 <= 8'hBF;

      nr50 <= 8'h77;
      nr51 <= 8'hF3;
      nr52 <= 8'hF1;

      for (i = 0; i < 16; i = i + 1) begin
        s3_wave[i] <= 0;
      end

      s4_lfsr <= 16'h7FFF;

      spu_clock <= 0;

      s1_enable <= 1;
      s2_enable <= 0;
      s3_enable <= 0;
      s4_enable <= 0;

      s1_sweep_clock <= 0;
      s1_sweep_freq <= 0;


      s1_duration <= 0;
      s2_duration <= 0;
      s3_duration <= 0;

      s4_duration <= 0;
      s1_env_clock <= 0;
      s2_env_clock <= 0;
      s4_env_clock <= 0;

      s1_env_volume <= 0;
      s2_env_volume <= 0;
      s4_env_volume <= 0;

      s1_phase_clock <= 0;
      s2_phase_clock <= 0;
      s3_phase_clock <= 0;
      s4_phase_clock <= 0;

      s1_phase <= 0;
      s2_phase <= 0;
      s3_phase <= 0;

      s1_out <= 0;
      s2_out <= 0;
      s3_out <= 0;
      s4_out <= 0;
    end
    else begin
      logic sound_on;
      logic[15:0] spu_clock_;
      logic[15:0] clock_flip;
      logic sweep_tick;
      logic length_tick;
      logic env_tick;
      logic s3_power;
      logic[15:0] s3_phase_clock_;
      logic[7:0] s3_phase_;
      logic s1_trigger_;
      logic s2_trigger_;
      logic s3_trigger_;
      logic s4_trigger_;
      sound_on = (nr52 & 8'h80);
      spu_clock_ = (spu_clock + 1) & 16'h3FFF;
      clock_flip = (~spu_clock) & spu_clock_;

      sweep_tick =  (spu_clock_ & 14'b01111111111111) == 14'b01000000000000;
      length_tick = (spu_clock_ & 14'b00111111111111) == 14'b00000000000000;
      env_tick =    (spu_clock_ & 14'b11111111111111) == 14'b11100000000000;

      //----------
      // output

      s1_out <= 0;
      s2_out <= 0;
      s3_out <= 0;
      s4_out <= 0;

      if (s1_enable) begin
        logic[7:0] s1_volume;
        logic[7:0] s1_duty;
        logic[7:0] s1_sample;
        s1_volume = (nr12 & 8'h08) ? 8'(s1_env_volume) : 8'(15 ^ s1_env_volume);
        s1_duty = (nr11 & 8'b11000000) >> 6;
        s1_duty = s1_duty ? s1_duty * 2 : 1;
        s1_sample = (s1_phase < s1_duty) ? 8'(s1_volume) : 8'd0;
        s1_out <= s1_sample;
      end

      if (s2_enable) begin
        logic[7:0] s2_volume;
        logic[7:0] s2_duty;
        logic[7:0] s2_sample;
        s2_volume = (nr22 & 8'h08) ? 8'(s2_env_volume) : 8'(15 ^ s2_env_volume);
        s2_duty = (nr21 & 8'b11000000) >> 6;
        s2_duty = s2_duty ? s2_duty * 2 : 1;
        s2_sample = (s2_phase < s2_duty) ? 8'(s2_volume) : 8'd0;
        s2_out <= s2_sample;
      end

      s3_power = (nr30 & 8'b10000000);
      if (s3_enable && s3_power) begin
        logic[7:0] s3_volume_shift;
        logic[7:0] s3_sample;
        s3_volume_shift = 0;
        case ((nr32 & 8'b01100000) >> 5)
          0: s3_volume_shift = 4;
          1: s3_volume_shift = 0;
          2: s3_volume_shift = 1;
          3: s3_volume_shift = 2;
        endcase
        s3_sample = s3_wave[s3_phase >> 1];
        s3_sample = (s3_phase & 1) ? 8'(s3_sample & 4'hF) : 8'(s3_sample >> 4);
        s3_sample = s3_sample >> s3_volume_shift;
        s3_out <= s3_sample;
      end

      if (s4_enable) begin
        logic[7:0] s4_volume;
        logic[7:0] s4_sample;
        s4_volume = (nr42 & 8'h08) ? 8'(s4_env_volume) : 8'(15 ^ s4_env_volume);
        s4_sample = (s4_lfsr & 1) ? 8'(s4_volume) : 8'd0;
        s4_out <= s4_sample;
      end

      //----------
      // s1 clock

      if (!s1_phase_clock) begin
        logic[7:0] s1_sweep_period;
        logic[15:0] s1_freq;
        s1_sweep_period = (nr10 & 8'b01110000) >> 4;
        s1_freq = ((nr14 << 8) | nr13) & 16'h07FF;

        s1_phase_clock <= 2047 ^ (s1_sweep_period ? s1_sweep_freq : s1_freq);
        s1_phase <= (s1_phase + 1) & 7;
      end
      else begin
        s1_phase_clock <= s1_phase_clock - 1;
      end

      //----------
      // s2 clock

      if (!s2_phase_clock) begin
        logic[15:0] s2_freq;
        s2_freq = ((nr24 << 8) | nr23) & 16'h07FF;

        s2_phase_clock <= 2047 ^ s2_freq;
        s2_phase <= (s2_phase + 1) & 7;
      end
      else begin
        s2_phase_clock <= s2_phase_clock - 1;
      end

      //----------
      // s3 clock - we run this twice because this is ticking at 1 mhz

      s3_phase_clock_ = s3_phase_clock;
      s3_phase_ = s3_phase;

      if (!s3_phase_clock_) begin
        logic[15:0] s3_freq;
        s3_freq = ((nr34 << 8) | nr33) & 16'h07FF;

        s3_phase_clock_ = 2047 ^ s3_freq;
        s3_phase_ = (s3_phase_ + 1) & 31;
      end
      else begin
        s3_phase_clock_ = s3_phase_clock_ - 1;
      end

      if (!s3_phase_clock_) begin
        logic[15:0] s3_freq;
        s3_freq = ((nr34 << 8) | nr33) & 16'h07FF;

        s3_phase_clock_ = 2047 ^ s3_freq;
        s3_phase_ = (s3_phase_ + 1) & 31;
      end
      else begin
        s3_phase_clock_ = s3_phase_clock_ - 1;
      end

      s3_phase_clock <= s3_phase_clock_;
      s3_phase <= s3_phase_;

      //----------
      // s4 clock

      if (!s4_phase_clock) begin
        logic s4_lfsr_mode;
        logic[7:0] s4_phase_period;
        logic lfsr_bit;
        s4_lfsr_mode = (nr43 & 8'b00001000) >> 3;
        s4_phase_period = (nr43 & 8'b00000111) ? (nr43 & 8'b00000111) * 2 : 1;

        s4_phase_clock <= s4_phase_period;
        lfsr_bit = (s4_lfsr ^ (s4_lfsr >> 1)) & 1;
        s4_phase_clock <= s4_phase_period;
        if (s4_lfsr_mode) begin
          s4_lfsr <= ((s4_lfsr >> 1) & 16'b0111111111011111) | (lfsr_bit << 5);
        end
        else begin
          s4_lfsr <= ((s4_lfsr >> 1) & 16'b0011111111111111) | (lfsr_bit << 14);
        end
      end
      else begin
        logic[7:0] s4_clock_shift;
        s4_clock_shift = (nr43 & 8'b11110000) >> 4;
        s4_phase_clock <= s4_phase_clock - ((clock_flip >> s4_clock_shift) & 1);
      end

      //----------
      // sweep

      if (sweep_tick) begin
        logic[7:0] s1_sweep_period;
        logic[7:0] s1_sweep_shift;
        logic s1_sweep_dir;
        s1_sweep_period = (nr10 & 8'b01110000) >> 4;
        s1_sweep_shift = (nr10 & 8'b00000111) >> 0;
        s1_sweep_dir = (nr10 & 8'b00001000) >> 3;

        if (s1_sweep_period && s1_sweep_shift) begin
          if (s1_sweep_clock) begin
            s1_sweep_clock <= s1_sweep_clock - 1;
          end
          else begin
            logic[15:0] delta;
            logic[15:0] new_freq;
            delta = s1_sweep_freq >> s1_sweep_shift;
            new_freq = s1_sweep_freq + (s1_sweep_dir ? -delta : +delta);
            s1_sweep_clock <= s1_sweep_period;
            if (new_freq > 2047) begin
              s1_enable <= 0;
            end
            else begin
              s1_sweep_freq <= new_freq;
            end
          end
        end
      end

      //----------
      // length

      if (length_tick) begin
        logic s1_length_enable;
        logic s2_length_enable;
        logic s3_length_enable;
        logic s4_length_enable;
        logic[7:0] s1_duration_;
        logic[7:0] s2_duration_;
        logic[15:0] s3_duration_;
        logic[7:0] s4_duration_;
        s1_length_enable = (nr14 & 8'b01000000) >> 6;
        s2_length_enable = (nr24 & 8'b01000000) >> 6;
        s3_length_enable = (nr34 & 8'b01000000) >> 6;
        s4_length_enable = (nr44 & 8'b01000000) >> 6;
        s1_duration_ = s1_duration;
        s2_duration_ = s2_duration;
        s3_duration_ = s3_duration;
        s4_duration_ = s4_duration;

        if (s1_length_enable && s1_duration_) s1_duration_ = s1_duration_ - 1;
        if (s2_length_enable && s2_duration_) s2_duration_ = s2_duration_ - 1;
        if (s3_length_enable && s3_duration_) s3_duration_ = s3_duration_ - 1;
        if (s4_length_enable && s4_duration_) s4_duration_ = s4_duration_ - 1;

        if (s1_length_enable && s1_duration_ == 0) s1_enable <= 0;
        if (s2_length_enable && s2_duration_ == 0) s2_enable <= 0;
        if (s3_length_enable && s3_duration_ == 0) s3_enable <= 0;
        if (s4_length_enable && s4_duration_ == 0) s4_enable <= 0;

        s1_duration <= s1_duration_;
        s2_duration <= s2_duration_;
        s3_duration <= s3_duration_;
        s4_duration <= s4_duration_;
      end

      //----------
      // env

      if (env_tick) begin
        logic s1_env_dir;
        logic s2_env_dir;
        logic s4_env_dir;
        logic[7:0] s1_env_period;
        logic[7:0] s2_env_period;
        logic[7:0] s4_env_period;
        s1_env_dir = (nr12 & 8'b00001000) >> 3;
        s2_env_dir = (nr22 & 8'b00001000) >> 3;
        s4_env_dir = (nr42 & 8'b00001000) >> 3;

        s1_env_period = (nr12 & 8'b00000111) >> 0;
        s2_env_period = (nr22 & 8'b00000111) >> 0;
        s4_env_period = (nr42 & 8'b00000111) >> 0;

        if (s1_env_period) begin
          if (s1_env_clock) begin
            s1_env_clock <= s1_env_clock - 1;
          end
          else begin
            s1_env_clock <= s1_env_period;
            if (s1_env_volume < 15) s1_env_volume <= s1_env_volume + 1;
          end
        end

        if (s2_env_period) begin
          if (s2_env_clock) begin
            s2_env_clock <= s2_env_clock - 1;
          end
          else begin
            s2_env_clock <= s2_env_period;
            if (s2_env_volume < 15) s2_env_volume <= s2_env_volume + 1;
          end
        end

        if (s4_env_period) begin
          if (s4_env_clock) begin
            s4_env_clock <= s4_env_clock - 1;
          end
          else begin
            s4_env_clock <= s4_env_period;
            if (s4_env_volume < 15) s4_env_volume <= s4_env_volume + 1;
          end
        end
      end

      spu_clock <= spu_clock_;

      //----------
      // Wavetable writes

      if (tick_we && tick_addr >= 16'hFF30 && tick_addr <= 16'hFF3F) begin
        s3_wave[tick_addr & 4'hF] <= tick_data;
      end

      //----------
      // Triggers

      s1_trigger_ = tick_we && tick_addr == 16'hFF14 && (tick_data & 8'h80);
      s2_trigger_ = tick_we && tick_addr == 16'hFF19 && (tick_data & 8'h80);
      s3_trigger_ = tick_we && tick_addr == 16'hFF1E && (tick_data & 8'h80);
      s4_trigger_ = tick_we && tick_addr == 16'hFF23 && (tick_data & 8'h80);

      if (s1_trigger_) begin
        logic[7:0] s1_sweep_period;
        logic[7:0] s1_length;
        logic[7:0] s1_start_volume;
        logic[7:0] s1_env_period;
        logic[15:0] s1_freq;
        s1_sweep_period = (nr10 & 8'b01110000) >> 4;
        s1_length = 64 - (nr11 & 8'b00111111);
        s1_start_volume = (nr12 & 8'b11110000) >> 4;
        s1_env_period = (nr12 & 8'b00000111) >> 0;
        s1_freq = ((nr14 << 8) | nr13) & 16'h07FF;

        s1_enable <= (nr12 & 8'hF8) != 0;
        s1_duration <= s1_length;
        s1_sweep_clock <= s1_sweep_period;
        s1_sweep_freq <= s1_freq;

        s1_env_volume <= (nr12 & 8'h08) ? 8'(s1_start_volume) : 8'(15 ^ s1_start_volume);
        s1_env_clock <= s1_env_period;

        s1_phase_clock <= 2047 ^ s1_freq;
        s1_phase <= 0;
      end

      if (s2_trigger_) begin
        logic[7:0] s2_length;
        logic[7:0] s2_start_volume;
        logic[7:0] s2_env_period;
        logic[15:0] s2_freq;
        s2_length = 64 - (nr21 & 8'b00111111);
        s2_start_volume = (nr22 & 8'b11110000) >> 4;
        s2_env_period = (nr22 & 8'b00000111) >> 0;
        s2_freq = ((nr24 << 8) | nr23) & 16'h07FF;

        s2_enable <= (nr22 & 8'hF8) != 0;
        s2_duration <= s2_length;

        s2_env_volume <= (nr22 & 8'h08) ? 8'(s2_start_volume) : 8'(15 ^ s2_start_volume);
        s2_env_clock <= s2_env_period;

        s2_phase_clock <= 2047 ^ s2_freq;
        s2_phase <= 0;
      end

      if (s3_trigger_) begin
        logic[15:0] s3_length;
        logic[15:0] s3_freq;
        s3_length = 256 - nr31;
        s3_freq = ((nr34 << 8) | nr33) & 16'h07FF;

        s3_enable <= (nr32 != 0);
        s3_duration <= s3_length;
        s3_phase_clock <= 2047 ^ s3_freq;
        s3_phase <= 0;
      end

      if (s4_trigger_) begin
        logic[7:0] s4_length;
        logic[7:0] s4_start_volume;
        logic[7:0] s4_env_period;
        logic[7:0] s4_phase_period;
        s4_length = 64 - (nr41 & 8'b00111111);
        s4_start_volume = (nr42 & 8'b11110000) >> 4;
        s4_env_period = (nr42 & 8'b00000111) >> 0;
        s4_phase_period = (nr43 & 8'b00000111) ? (nr43 & 8'b00000111) * 2 : 1;

        s4_enable <= (nr42 & 8'hF8) != 0;
        s4_duration <= s4_length;

        s4_env_volume <= (nr42 & 8'h08) ? 8'(s4_start_volume) : 8'(15 ^ s4_start_volume);
        s4_env_clock <= s4_env_period;

        s4_phase_clock <= s4_phase_period;
        s4_lfsr <= 16'h7FFF;
      end

      //----------
      // Register writes

      if (tick_we && tick_addr >= 16'hFF10 && tick_addr <= 16'hFF26) begin
        case (tick_addr)
          16'hFF10: nr10 <= tick_data | 8'b10000000;
          16'hFF11: nr11 <= tick_data | 8'b00000000;
          16'hFF12: nr12 <= tick_data | 8'b00000000;
          16'hFF13: nr13 <= tick_data | 8'b00000000;
          16'hFF14: nr14 <= tick_data | 8'b00111000;
          16'hFF16: nr21 <= tick_data | 8'b00000000;
          16'hFF17: nr22 <= tick_data | 8'b00000000;
          16'hFF18: nr23 <= tick_data | 8'b00000000;
          16'hFF19: nr24 <= tick_data | 8'b00111000;
          16'hFF1A: nr30 <= tick_data | 8'b01111111;
          16'hFF1B: nr31 <= tick_data | 8'b00000000;
          16'hFF1C: nr32 <= tick_data | 8'b10011111;
          16'hFF1D: nr33 <= tick_data | 8'b00000000;
          16'hFF1E: nr34 <= tick_data | 8'b00111000;
          16'hFF20: nr41 <= tick_data | 8'b11000000;
          16'hFF21: nr42 <= tick_data | 8'b00000000;
          16'hFF22: nr43 <= tick_data | 8'b00000000;
          16'hFF23: nr44 <= tick_data | 8'b00111111;
          16'hFF24: nr50 <= tick_data | 8'b00000000;
          16'hFF25: nr51 <= tick_data | 8'b00000000;
          16'hFF26: nr52 <= tick_data | 8'b01110000;
        endcase
      end


    end
  end

  //-----------------------------------------------------------------------------

/*private:*/

  logic[7:0] nr10;
  logic[7:0] nr11;
  logic[7:0] nr12;
  logic[7:0] nr13;

  logic[7:0] nr14;
  logic[7:0] nr20;
  logic[7:0] nr21;
  logic[7:0] nr22;

  logic[7:0] nr23;
  logic[7:0] nr24;
  logic[7:0] nr30;
  logic[7:0] nr31;

  logic[7:0] nr32;
  logic[7:0] nr33;
  logic[7:0] nr34;
  logic[7:0] nr40;

  logic[7:0] nr41;
  logic[7:0] nr42;
  logic[7:0] nr43;
  logic[7:0] nr44;

  logic[7:0] nr50;
  logic[7:0] nr51;
  logic[7:0] nr52;

  logic[7:0] s3_wave[16];

  logic[15:0] s4_lfsr;

  logic[15:0] spu_clock;

  logic s1_enable;
  logic s2_enable;
  logic s3_enable;
  logic s4_enable;

  logic[7:0] s1_sweep_clock;
  logic[15:0] s1_sweep_freq;

  logic[7:0] s1_duration;
  logic[7:0] s2_duration;
  logic[15:0] s3_duration;

  logic[7:0] s4_duration;
  logic[7:0] s1_env_clock;
  logic[7:0] s2_env_clock;
  logic[7:0] s4_env_clock;

  logic[7:0] s1_env_volume;
  logic[7:0] s2_env_volume;
  logic[7:0] s4_env_volume;

  logic[15:0] s1_phase_clock;
  logic[15:0] s2_phase_clock;
  logic[15:0] s3_phase_clock;
  logic[15:0] s4_phase_clock;

  logic[7:0] s1_phase;
  logic[7:0] s2_phase;
  logic[7:0] s3_phase;

  logic[7:0] s1_out;
  logic[7:0] s2_out;
  logic[7:0] s3_out;
  logic[7:0] s4_out;
endmodule

/*

  void tick(int phase_total, const Req& req, Ack& ack) {

  }

  //----------
  // glitches n stuff

  if (we && addr == 0xFF12) {
    if ((nr12 & 0x08) && s1_enable) s1_env_volume = (s1_env_volume + 1) & 15;
    if ((data & 0xF8) == 0) s1_enable = 0;
  }

  if (we && addr == 0xFF17) {
    if ((nr22 & 0x08) && s2_enable) s2_env_volume = (s2_env_volume + 1) & 15;
    if ((data & 0xF8) == 0) s2_enable = 0;
  }

  if (we && addr == 0xFF21) {
    if ((nr42 & 0x08) && s4_enable) s4_env_volume = (s4_env_volume + 1) & 15;
    if ((data & 0xF8) == 0) s4_enable = 0;
  }

//-----------------------------------------------------------------------------

void MetroBoySPU::dump(Dumper& d) const {
  d("\002--------------SPU--------------\001\n");

  d("NR10 %s\n", byte_to_bits(nr10));
  d("NR11 %s\n", byte_to_bits(nr11));
  d("NR12 %s\n", byte_to_bits(nr12));
  d("NR13 %s\n", byte_to_bits(nr13));
  d("NR14 %s\n", byte_to_bits(nr14));
  d("NR20 %s\n", byte_to_bits(nr20));
  d("NR21 %s\n", byte_to_bits(nr21));
  d("NR22 %s\n", byte_to_bits(nr22));
  d("NR23 %s\n", byte_to_bits(nr23));
  d("NR24 %s\n", byte_to_bits(nr24));
  d("NR30 %s\n", byte_to_bits(nr30));
  d("NR31 %s\n", byte_to_bits(nr31));
  d("NR32 %s\n", byte_to_bits(nr32));
  d("NR33 %s\n", byte_to_bits(nr33));
  d("NR34 %s\n", byte_to_bits(nr34));
  d("NR40 %s\n", byte_to_bits(nr40));
  d("NR41 %s\n", byte_to_bits(nr41));
  d("NR42 %s\n", byte_to_bits(nr42));
  d("NR43 %s\n", byte_to_bits(nr43));
  d("NR44 %s\n", byte_to_bits(nr44));
  d("NR50 %s\n", byte_to_bits(nr50));
  d("NR51 %s\n", byte_to_bits(nr51));
  d("NR52 %s\n", byte_to_bits(nr52));

  const char* bar = "===============";

  logic<8> s1_volume = (nr12 & 0x08) ? s1_env_volume : 15 ^ s1_env_volume;
  d("s1 vol %s\n", bar + (15 - s1_volume));

  logic<8> s2_volume = (nr22 & 0x08) ? s2_env_volume : 15 ^ s2_env_volume;
  d("s2 vol %s\n", bar + (15 - s2_volume));

  logic<8> s3_volume = 0;
  switch ((nr32 & 0b01100000) >> 5) {
  case 0: s3_volume = 0; break;
  case 1: s3_volume = 15; break;
  case 2: s3_volume = 7; break;
  case 3: s3_volume = 3; break;
  }
  d("s3 vol %s\n", bar + (15 - s3_volume));

  logic<8> s4_volume = (nr42 & 0x08) ? s4_env_volume : 15 ^ s4_env_volume;
  d("s4 vol %s\n", bar + (15 - s4_volume));

  if (0) {
    char buf[33];
    for (int i = 0; i < 16; i++) {
      logic<8> a = (s3_wave[i] & 0x0F) >> 0;
      logic<8> b = (s3_wave[i] & 0xF0) >> 4;

      buf[2 * i + 0] = 'A' + a;
      buf[2 * i + 1] = 'A' + b;
    }

    buf[32] = 0;
    d("[%s]\n", buf);
  }
}

//-----------------------------------------------------------------------------
*/
