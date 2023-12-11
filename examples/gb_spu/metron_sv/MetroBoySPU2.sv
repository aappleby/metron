`ifndef METRO_BOY_SPU2_H
`define METRO_BOY_SPU2_H
`include "metron/metron_tools.sv"

//------------------------------------------------------------------------------

module MetroBoySPU2 (
  // global clock
  input logic clock,
  // output signals
  output logic[8:0] out_r,
  output logic[8:0] out_l,
  // tick() ports
  input logic tick_reset,
  input logic[15:0] tick_addr,
  input logic[7:0] tick_data_in,
  input logic tick_read,
  input logic tick_write
);
/*public:*/

  //----------------------------------------

  always_comb begin : tock_out
    logic[8:0] l;
    logic[8:0] r;

    l = 0;
    r = 0;

    if (s1_running_ && s1_env_vol_) begin
      logic s1_out;
      s1_out = 0;
      case(s1_duty_)
        0:  begin s1_out = s1_phase_ < 1; /*break;*/ end
        1:  begin s1_out = s1_phase_ < 2; /*break;*/ end
        2:  begin s1_out = s1_phase_ < 4; /*break;*/ end
        3:  begin s1_out = s1_phase_ < 6; /*break;*/ end
      endcase
      if (mix_l1_ && s1_out) l = l + s1_env_vol_;
      if (mix_r1_ && s1_out) r = r + s1_env_vol_;
    end

    if (s2_running_ && s2_env_vol_) begin
      logic s2_out;
      s2_out = 0;
      case(s2_duty_)
        0:  begin s2_out = s2_phase_ < 1; /*break;*/ end
        1:  begin s2_out = s2_phase_ < 2; /*break;*/ end
        2:  begin s2_out = s2_phase_ < 4; /*break;*/ end
        3:  begin s2_out = s2_phase_ < 6; /*break;*/ end
      endcase
      if (mix_l2_ && s2_out) l = l + s2_env_vol_;
      if (mix_r2_ && s2_out) r = r + s2_env_vol_;
    end

    if (s3_running_ && s3_power_) begin
      logic[7:0] s3_sample;
      logic[3:0] s3_out;
      s3_sample = s3_wave_[s3_phase_ >> 1];
      s3_out = (s3_phase_ & 1) ? 4'(s3_sample) : s3_sample[7:4];
      s3_out = s3_out >> s3_volume_shift_;
      if (mix_l3_) l = l + s3_out;
      if (mix_r3_) r = r + s3_out;
    end

    if (s4_running_ && s4_env_vol_) begin
      logic s4_out;
      s4_out = s4_lfsr_[15];
      if (mix_l4_ && s4_out) l = l + s4_env_vol_;
      if (mix_r4_ && s4_out) r = r + s4_env_vol_;
    end

    l = l * volume_l_;
    r = r * volume_r_;

    out_l = l;
    out_r = r;
  end

  //----------------------------------------

  always_ff @(posedge clock) begin : tick
    if (tick_reset) begin
      int i;
      spu_clock_old_ <= 0;
      data_out_ <= 0;

      s1_sweep_shift_ <= 0;
      s1_sweep_dir_ <= 0;
      s1_sweep_timer_init_ <= 0;
      s1_len_timer_init_ <= 0;
      s1_duty_ <= 0;
      s1_env_timer_init_ <= 0;
      s1_env_add_ <= 0;
      s1_env_vol_init_ <= 0;
      s1_freq_timer_init_ <= 0;
      s1_len_en_ <= 0;
      s1_trig_ <= 0;
      s1_running_ <= 0;
      s1_sweep_timer_ <= 0;
      s1_sweep_freq_ <= 0;
      s1_len_timer_ <= 0;
      s1_env_vol_ <= 0;
      s1_env_timer_ <= 0;
      s1_freq_timer_ <= 0;
      s1_phase_ <= 0;

      s2_len_timer_init_ <= 0;
      s2_duty_ <= 0;
      s2_env_timer_init_ <= 0;
      s2_env_add_ <= 0;
      s2_env_vol_init_ <= 0;
      s2_freq_timer_init_ <= 0;
      s2_len_en_ <= 0;
      s2_trig_ <= 0;

      s2_len_timer_ <= 0;
      s2_running_ <= 0;
      s2_env_timer_ <= 0;
      s2_env_vol_ <= 0;
      s2_freq_timer_ <= 0;
      s2_phase_ <= 0;

      s3_power_ <= 0;
      s3_len_timer_init_ <= 0;
      s3_volume_shift_ <= 0;
      s3_freq_timer_init_ <= 0;
      s3_len_en_ <= 0;
      s3_trig_ <= 0;

      s3_running_ <= 0;
      s3_len_timer_ <= 0;
      s3_freq_timer_ <= 0;
      s3_phase_ <= 0;

      for (i = 0; i < 16; i = i + 1) begin
        s3_wave_[i] <= 0;
      end

      s4_len_timer_init_ <= 0;
      s4_env_timer_init_ <= 0;
      s4_env_add_ <= 0;
      s4_env_vol_init_ <= 0;
      s4_freq_timer_init_ <= 0;
      s4_mode_ <= 0;
      s4_shift_ <= 0;
      s4_len_en_ <= 0;
      s4_trig_ <= 0;

      s4_running_ <= 0;
      s4_len_timer_ <= 0;
      s4_env_timer_ <= 0;
      s4_env_vol_ <= 0;
      s4_freq_timer_ <= 0;
      s4_lfsr_ <= 0;

      volume_l_ <= 0;
      volume_r_ <= 0;

      mix_r1_ <= 0;
      mix_r2_ <= 0;
      mix_r3_ <= 0;
      mix_r4_ <= 0;
      mix_l1_ <= 0;
      mix_l2_ <= 0;
      mix_l3_ <= 0;
      mix_l4_ <= 0;

      spu_power_ <= 0;

    end
    else begin
      logic[15:0] spu_clock_new;
      logic[15:0] spu_tick;
      logic sweep_tick;
      logic length_tick;
      logic env_tick;
      logic lfsr_clock_old;
      logic lfsr_clock_new;
      spu_clock_new = spu_clock_old_ + 1;
      spu_tick = (~spu_clock_old_) & (spu_clock_new);

      sweep_tick  = spu_tick[12];
      length_tick = spu_tick[11];
      env_tick    = spu_tick[13];

      //----------------------------------------
      // Reg read

      if (tick_read) begin
        case (tick_addr)
          16'hFF10:  begin data_out_ <= {1'd1, s1_sweep_timer_init_, s1_sweep_dir_, s1_sweep_shift_}; /*break;*/ end
          16'hFF11:  begin data_out_ <= {s1_duty_, s1_len_timer_init_}; /*break;*/ end
          16'hFF12:  begin data_out_ <= {s1_env_vol_init_, s1_env_add_, s1_env_timer_init_}; /*break;*/ end
          16'hFF13:  begin data_out_ <= 8'(s1_freq_timer_init_); /*break;*/ end
          16'hFF14:  begin data_out_ <= {s1_trig_, s1_len_en_, 3'b111, s1_freq_timer_init_[10:8]}; /*break;*/ end

            //----------

          16'hFF16:  begin data_out_ <= {s2_duty_, s2_len_timer_init_}; /*break;*/ end
          16'hFF17:  begin data_out_ <= {s2_env_vol_init_, s2_env_add_, s2_env_timer_init_}; /*break;*/ end
          16'hFF18:  begin data_out_ <= 8'(s2_freq_timer_init_); /*break;*/ end
          16'hFF19:  begin data_out_ <= {s2_trig_, s2_len_en_, 3'b111, s2_freq_timer_init_[10:8]}; /*break;*/ end

            //----------

          16'hFF1A:  begin data_out_ <= {s3_power_, 7'b1111111}; /*break;*/ end
          16'hFF1B:  begin data_out_ <= s3_len_timer_init_; /*break;*/ end

          // metron didn't like the block without {}
          16'hFF1C: begin
            case (s3_volume_shift_)
              0:  begin data_out_ <= 8'b01000000; /*break;*/ end
              1:  begin data_out_ <= 8'b10000000; /*break;*/ end
              2:  begin data_out_ <= 8'b11000000; /*break;*/ end
              4:  begin data_out_ <= 8'b00000000; /*break;*/ end
            endcase
            /*break;*/
          end
          16'hFF1D:  begin data_out_ <= 8'(s3_freq_timer_init_); /*break;*/ end
          16'hFF1E:  begin data_out_ <= {s3_trig_, s3_len_en_, 3'b111, s3_freq_timer_init_[10:8]}; /*break;*/ end

            //----------

          16'hFF20:  begin data_out_ <= {2'b11, s4_len_timer_init_}; /*break;*/ end
          16'hFF21:  begin data_out_ <= {s4_env_vol_init_, s4_env_add_, s4_env_timer_init_}; /*break;*/ end
          16'hFF22:  begin data_out_ <= {s4_shift_, s4_mode_, s4_freq_timer_init_}; /*break;*/ end
          16'hFF23:  begin data_out_ <= {s4_trig_, s4_len_en_, 6'b111111}; /*break;*/ end

            //----------

          16'hFF24:  begin data_out_ <= {1'd0, volume_l_, 1'd0, volume_r_}; /*break;*/ end
          16'hFF25:  begin data_out_ <= {mix_l4_, mix_l3_, mix_l2_, mix_l1_, mix_r4_, mix_r3_, mix_r2_, mix_r1_}; /*break;*/ end
          16'hFF26:  begin data_out_ <= {spu_power_, 7'd0}; /*break;*/ end

          // "default: break didn't work?"
          default: begin /*break;*/ end
        endcase
      end

      //----------
      // s1 clock

      if (s1_freq_timer_ == 11'b11111111111) begin
        s1_phase_ <= s1_phase_ + 1;
        s1_freq_timer_ <= s1_sweep_timer_init_ ? s1_sweep_freq_ : s1_freq_timer_init_;
      end
      else begin
        s1_freq_timer_ <= s1_freq_timer_ + 1;
      end

      //----------
      // s1 length

      if (length_tick && s1_running_ && s1_len_en_) begin
        if (s1_len_timer_ == 6'b111111) begin
          s1_len_timer_ <= 0;
          s1_running_ <= 0;
        end
        else begin
          s1_len_timer_ <= s1_len_timer_ + 1;
        end
      end

      //----------
      // s1 sweep

      if (sweep_tick && s1_sweep_timer_init_ && s1_sweep_shift_) begin
        if (s1_sweep_timer_) begin
          s1_sweep_timer_ <= s1_sweep_timer_ - 1;
        end
        else begin
          logic[10:0] delta;
          logic[11:0] next_freq;
          delta = s1_sweep_freq_ >> s1_sweep_shift_;
          next_freq = s1_sweep_freq_ + (s1_sweep_dir_ ? -delta : +delta);
          if (next_freq > 2047) s1_running_ <= 0;
          s1_sweep_timer_ <= s1_sweep_timer_init_;
          s1_sweep_freq_ <= next_freq;
        end
      end

      //----------
      // s1 env

      if (env_tick && s1_env_timer_init_) begin
        if (s1_env_timer_) begin
          s1_env_timer_ <= s1_env_timer_ - 1;
        end
        else begin
          s1_env_timer_ <= s1_env_timer_init_;
          if (s1_env_add_) begin if (s1_env_vol_ < 15) s1_env_vol_ <= s1_env_vol_ + 1; end
          else            begin if (s1_env_vol_ >  0) s1_env_vol_ <= s1_env_vol_ - 1; end
        end
      end

      //----------
      // s2 clock

      if (s2_freq_timer_ == 12'h7FF) begin
        s2_phase_ <= s2_phase_ + 1;
        s2_freq_timer_ <= s2_freq_timer_init_;
      end
      else begin
        s2_freq_timer_ <= s2_freq_timer_ + 1;
      end

      //----------
      // s2 length

      if (length_tick && s2_running_ && s2_len_en_) begin
        if (s2_len_timer_ == 8'h3F) begin
          s2_len_timer_ <= 0;
          s2_running_ <= 0;
        end
        else begin
          s2_len_timer_ <= s2_len_timer_ + 1;
        end
      end

      //----------
      // s2 env

      if (env_tick && s2_env_timer_init_) begin
        if (s2_env_timer_) begin
          s2_env_timer_ <= s2_env_timer_ - 1;
        end
        else begin
          s2_env_timer_ <= s2_env_timer_init_;
          if (s2_env_add_) begin if (s2_env_vol_ < 15) s2_env_vol_ <= s2_env_vol_ + 1; end
          else            begin if (s2_env_vol_ >  0) s2_env_vol_ <= s2_env_vol_ - 1; end
        end
      end

      //----------
      // s3 clock - we run this twice because s3's timer ticks at 2 mhz

      begin
        logic[4:0] next_phase;
        logic[10:0] next_timer;
        next_phase = s3_phase_;
        next_timer = s3_freq_timer_;

        if (next_timer == 12'h7FF) begin
          next_phase = next_phase + 1;
          next_timer = s3_freq_timer_init_;
        end
        else begin
          next_timer = next_timer + 1;
        end

        if (next_timer == 12'h7FF) begin
          next_phase = next_phase + 1;
          next_timer = s3_freq_timer_init_;
        end
        else begin
          next_timer = next_timer + 1;
        end

        s3_phase_ <= next_phase;
        s3_freq_timer_ <= next_timer;
      end

      //----------
      // s3 length

      if (length_tick && s3_running_ && s3_len_en_) begin
        if (s3_len_timer_ == 8'hFF) begin
          s3_len_timer_ <= 0;
          s3_running_ <= 0;
        end
        else begin
          s3_len_timer_ <= s3_len_timer_ + 1;
        end
      end

      //----------
      // s4 lfsr

      lfsr_clock_old = spu_clock_old_[s4_shift_ + 1];
      lfsr_clock_new = spu_clock_new[s4_shift_ + 1];

      if ((lfsr_clock_old == 0) && (lfsr_clock_new == 1)) begin
        if (s4_freq_timer_) begin
          s4_freq_timer_ <= s4_freq_timer_ - 1;
        end
        else begin
          logic new_bit;
          new_bit = s4_lfsr_[15] ^ s4_lfsr_[14] ^ 1;
          s4_lfsr_ <= {
            s4_lfsr_[14:9],
            s4_mode_ ? new_bit : s4_lfsr_[8],
            8'(s4_lfsr_),
            new_bit};
          s4_freq_timer_ <= s4_freq_timer_init_;
        end
      end

      //----------
      // s4 length

      if (length_tick && s4_running_ && s4_len_en_) begin
        if (s4_len_timer_ == 8'h3F) begin
          s4_len_timer_ <= 0;
          s4_running_ <= 0;
        end
        else begin
          s4_len_timer_ <= s4_len_timer_ + 1;
        end
      end

      //----------
      // s4 env

      if (env_tick && s4_env_timer_init_) begin
        if (s4_env_timer_) begin
          s4_env_timer_ <= s4_env_timer_ - 1;
        end
        else begin
          s4_env_timer_ <= s4_env_timer_init_;
          if (s4_env_add_) begin if (s4_env_vol_ < 15) s4_env_vol_ <= s4_env_vol_ + 1; end
          else            begin if (s4_env_vol_ >  0) s4_env_vol_ <= s4_env_vol_ - 1; end
        end
      end

      //----------
      // Triggers

      if (s1_trig_ && (s1_env_vol_init_ || s1_env_add_)) begin
        s1_running_     <= 1;
        s1_len_timer_   <= s1_len_timer_init_;
        s1_sweep_timer_ <= s1_sweep_timer_init_;
        s1_sweep_freq_  <= s1_freq_timer_init_;
        s1_env_vol_     <= s1_env_vol_init_;
        s1_env_timer_   <= s1_env_timer_init_;
        s1_freq_timer_  <= s1_freq_timer_init_;
        s1_phase_       <= 0;
        s1_trig_        <= 0;
      end

      if (s2_trig_ && (s2_env_vol_init_ || s2_env_add_)) begin
        s2_running_    <= 1;
        s2_len_timer_  <= s2_len_timer_init_;
        s2_env_vol_    <= s2_env_vol_init_;
        s2_env_timer_  <= s2_env_timer_init_;
        s2_freq_timer_ <= s2_freq_timer_init_;
        s2_phase_      <= 0;
        s2_trig_       <= 0;
      end

      if (s3_trig_) begin
        s3_running_    <= 1;
        s3_len_timer_  <= s3_len_timer_init_;
        s3_freq_timer_ <= s3_freq_timer_init_;
        s3_phase_      <= 0;
        s3_trig_       <= 0;
      end

      if (s4_trig_ && (s4_env_vol_init_ || s4_env_add_)) begin
        s4_running_    <= 1;
        s4_len_timer_  <= s4_len_timer_init_;
        s4_env_vol_    <= s4_env_vol_init_;
        s4_env_timer_  <= s4_env_timer_init_;
        s4_freq_timer_ <= s4_freq_timer_init_;
        s4_lfsr_       <= 0;
        s4_trig_       <= 0;
      end

      //----------
      // Register writes

      if (tick_write && tick_addr >= 16'hFF10 && tick_addr <= 16'hFF26) begin
        case (tick_addr)
          16'hFF10: begin
            s1_sweep_shift_      <= 3'(tick_data_in);
            s1_sweep_dir_        <= tick_data_in[3];
            s1_sweep_timer_init_ <= tick_data_in[6:4];
            /*break;*/
          end
          16'hFF11: begin
            s1_len_timer_init_ <= 6'(tick_data_in);
            s1_duty_           <= tick_data_in[7:6];
            /*break;*/
          end
          16'hFF12: begin
            s1_env_timer_init_ <= 3'(tick_data_in);
            s1_env_add_        <= tick_data_in[3];
            s1_env_vol_init_   <= tick_data_in[7:4];
            /*break;*/
          end
          16'hFF13: begin
            s1_freq_timer_init_ <= {s1_freq_timer_init_[10:8], tick_data_in};
            /*break;*/
          end
          16'hFF14: begin
            s1_freq_timer_init_ <= {3'(tick_data_in), 8'(s1_freq_timer_init_)};
            s1_len_en_          <= tick_data_in[6];
            s1_trig_            <= tick_data_in[7];
            /*break;*/
          end

          //----------

          16'hFF16: begin
            s2_len_timer_init_ <= 6'(tick_data_in);
            s2_duty_           <= tick_data_in[7:6];
            /*break;*/
          end
          16'hFF17: begin
            s2_env_timer_init_ <= 3'(tick_data_in);
            s2_env_add_        <= tick_data_in[3];
            s2_env_vol_init_   <= tick_data_in[7:4];
            /*break;*/
          end
          16'hFF18: begin
            s2_freq_timer_init_ <= {s2_freq_timer_init_[10:8], tick_data_in};
            /*break;*/
          end
          16'hFF19: begin
            s2_freq_timer_init_ <= {3'(tick_data_in), 8'(s2_freq_timer_init_)};
            s2_len_en_          <= tick_data_in[6];
            s2_trig_            <= tick_data_in[7];
            /*break;*/
          end

          //----------

          16'hFF1A: begin
            s3_power_ <= tick_data_in[7];
            /*break;*/
          end
          16'hFF1B: begin
            s3_len_timer_init_ <= 8'(tick_data_in);
            /*break;*/
          end
          16'hFF1C: begin
            case (tick_data_in[6:5])
              0:  begin s3_volume_shift_ <= 4; /*break;*/ end
              1:  begin s3_volume_shift_ <= 0; /*break;*/ end
              2:  begin s3_volume_shift_ <= 1; /*break;*/ end
              3:  begin s3_volume_shift_ <= 2; /*break;*/ end
            endcase
            /*break;*/
          end
          16'hFF1D: begin
            s3_freq_timer_init_ <= {s3_freq_timer_init_[10:8], tick_data_in};
            /*break;*/
          end
          16'hFF1E: begin
            s3_freq_timer_init_ <= {3'(tick_data_in), 8'(s3_freq_timer_init_)};
            s3_len_en_          <= tick_data_in[6];
            s3_trig_            <= tick_data_in[7];
            /*break;*/
          end

          //----------

          16'hFF20: begin
            s4_len_timer_init_ <= 6'(tick_data_in);
            /*break;*/
          end
          16'hFF21: begin
            s4_env_timer_init_ <= 3'(tick_data_in);
            s4_env_add_        <= tick_data_in[3];
            s4_env_vol_init_   <= tick_data_in[7:4];
            /*break;*/
          end
          16'hFF22: begin
            s4_freq_timer_init_ <= 3'(tick_data_in);
            s4_mode_            <= tick_data_in[3];
            s4_shift_           <= tick_data_in[7:4];
            /*break;*/
          end
          16'hFF23: begin
            s4_len_en_ <= tick_data_in[6];
            s4_trig_   <= tick_data_in[7];
            /*break;*/
          end

          //----------

          16'hFF24: begin
            volume_r_ <= 3'(tick_data_in) + 1;
            volume_l_ <= tick_data_in[6:4] + 1;
            /*break;*/
          end
          16'hFF25: begin
            mix_r1_ <= 1'(tick_data_in);
            mix_r2_ <= tick_data_in[1];
            mix_r3_ <= tick_data_in[2];
            mix_r4_ <= tick_data_in[3];
            mix_l1_ <= tick_data_in[4];
            mix_l2_ <= tick_data_in[5];
            mix_l3_ <= tick_data_in[6];
            mix_l4_ <= tick_data_in[7];
            /*break;*/
          end
          16'hFF26: begin
            spu_power_ <= tick_data_in[7];
            /*break;*/
          end
          default: begin
            /*break;*/
          end
        endcase
      end

      //----------
      // Wavetable writes

      if (tick_write && tick_addr >= 16'hFF30 && tick_addr <= 16'hFF3F) begin
        s3_wave_[tick_addr & 4'hF] <= tick_data_in;
      end

      spu_clock_old_ <= spu_clock_new;
    end
  end

  //----------------------------------------

   // signals
   // signals

  //----------------------------------------

/*private:*/

  logic[15:0] spu_clock_old_;
  logic[7:0]  data_out_;

  // Channel 1

  /*NR10*/ logic[2:0]  s1_sweep_shift_;
  /*NR10*/ logic  s1_sweep_dir_;
  /*NR10*/ logic[2:0]  s1_sweep_timer_init_;
  /*NR11*/ logic[5:0]  s1_len_timer_init_;
  /*NR11*/ logic[1:0]  s1_duty_;
  /*NR12*/ logic[2:0]  s1_env_timer_init_;
  /*NR12*/ logic  s1_env_add_;
  /*NR12*/ logic[3:0]  s1_env_vol_init_;
  /*NR13*/ logic[10:0] s1_freq_timer_init_;
  /*NR14*/ logic  s1_len_en_;
  /*NR14*/ logic  s1_trig_;

  logic  s1_running_;
  logic[2:0]  s1_sweep_timer_;
  logic[10:0] s1_sweep_freq_;
  logic[5:0]  s1_len_timer_;
  logic[3:0]  s1_env_vol_;
  logic[2:0]  s1_env_timer_;
  logic[10:0] s1_freq_timer_;
  logic[2:0]  s1_phase_;

  // Channel 2

  /*NR21*/ logic[5:0]  s2_len_timer_init_;
  /*NR21*/ logic[1:0]  s2_duty_;
  /*NR22*/ logic[2:0]  s2_env_timer_init_;
  /*NR22*/ logic  s2_env_add_;
  /*NR22*/ logic[3:0]  s2_env_vol_init_;
  /*NR23*/ logic[10:0] s2_freq_timer_init_;
  /*NR24*/ logic  s2_len_en_;
  /*NR24*/ logic  s2_trig_;

  logic[5:0]  s2_len_timer_;
  logic  s2_running_;
  logic[2:0]  s2_env_timer_;
  logic[3:0]  s2_env_vol_;
  logic[10:0] s2_freq_timer_;
  logic[2:0]  s2_phase_;

  // Channel 3

  /*NR30*/ logic  s3_power_;
  /*NR31*/ logic[7:0]  s3_len_timer_init_;
  /*NR32*/ logic[2:0]  s3_volume_shift_;
  /*NR33*/ logic[10:0] s3_freq_timer_init_;
  /*NR34*/ logic  s3_len_en_;
  /*NR34*/ logic  s3_trig_;

  logic  s3_running_;
  logic[7:0]  s3_len_timer_;
  logic[10:0] s3_freq_timer_;
  logic[4:0]  s3_phase_;
  logic[7:0]  s3_wave_[16];

  // Channel 4

  /*NR41*/ logic[5:0]  s4_len_timer_init_;
  /*NR42*/ logic[2:0]  s4_env_timer_init_;
  /*NR42*/ logic  s4_env_add_;
  /*NR42*/ logic[3:0]  s4_env_vol_init_;
  /*NR43*/ logic[2:0]  s4_freq_timer_init_;
  /*NR43*/ logic  s4_mode_;
  /*NR43*/ logic[3:0]  s4_shift_;
  /*NR44*/ logic  s4_len_en_;
  /*NR44*/ logic  s4_trig_;

  logic  s4_running_;
  logic[5:0]  s4_len_timer_;
  logic[2:0]  s4_env_timer_;
  logic[3:0]  s4_env_vol_;
  logic[2:0]  s4_freq_timer_;
  logic[15:0] s4_lfsr_;

  // SPU Control Registers

  /*NR50*/ logic[3:0]  volume_l_;
  /*NR50*/ logic[3:0]  volume_r_;

  /*NR51*/ logic  mix_r1_;
  /*NR51*/ logic  mix_r2_;
  /*NR51*/ logic  mix_r3_;
  /*NR51*/ logic  mix_r4_;
  /*NR51*/ logic  mix_l1_;
  /*NR51*/ logic  mix_l2_;
  /*NR51*/ logic  mix_l3_;
  /*NR51*/ logic  mix_l4_;

  /*NR52*/ logic  spu_power_;

endmodule

`endif // METRO_BOY_SPU2_H
