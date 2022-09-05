`include "metron_tools.sv"

//------------------------------------------------------------------------------

module MetroBoySPU2 (
  // global clock
  input logic clock,
  // output signals
  output logic[8:0]  out_r,
  output logic[8:0]  out_l,
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

    if (s1_running && s1_env_vol) begin
      logic s1_out;
      s1_out = 0;
      case(s1_duty)
        0: s1_out = s1_phase < 1;
        1: s1_out = s1_phase < 2;
        2: s1_out = s1_phase < 4;
        3: s1_out = s1_phase < 6;
      endcase
      if (mix_l1 && s1_out) l = l + s1_env_vol;
      if (mix_r1 && s1_out) r = r + s1_env_vol;
    end

    if (s2_running && s2_env_vol) begin
      logic s2_out;
      s2_out = 0;
      case(s2_duty)
        0: s2_out = s2_phase < 1;
        1: s2_out = s2_phase < 2;
        2: s2_out = s2_phase < 4;
        3: s2_out = s2_phase < 6;
      endcase
      if (mix_l2 && s2_out) l = l + s2_env_vol;
      if (mix_r2 && s2_out) r = r + s2_env_vol;
    end

    if (s3_running && s3_power) begin
      logic[7:0] s3_sample;
      logic[3:0] s3_out;
      s3_sample = s3_wave[s3_phase >> 1];
      s3_out = (s3_phase & 1) ? s3_sample[3:0] : s3_sample[7:4];
      s3_out = s3_out >> s3_volume_shift;
      if (mix_l3) l = l + s3_out;
      if (mix_r3) r = r + s3_out;
    end

    if (s4_running && s4_env_vol) begin
      logic s4_out;
      s4_out = s4_lfsr[15];
      if (mix_l4 && s4_out) l = l + s4_env_vol;
      if (mix_r4 && s4_out) r = r + s4_env_vol;
    end

    l = l * volume_l;
    r = r * volume_r;

    out_l = l;
    out_r = r;
  end

  //----------------------------------------

  always_ff @(posedge clock) begin : tick
    if (tick_reset) begin
      int i;
      spu_clock_old <= 0;
      data_out <= 0;

      s1_sweep_shift <= 0;
      s1_sweep_dir <= 0;
      s1_sweep_timer_init <= 0;
      s1_len_timer_init <= 0;
      s1_duty <= 0;
      s1_env_timer_init <= 0;
      s1_env_add <= 0;
      s1_env_vol_init <= 0;
      s1_freq_timer_init <= 0;
      s1_len_en <= 0;
      s1_trig <= 0;
      s1_running <= 0;
      s1_sweep_timer <= 0;
      s1_sweep_freq <= 0;
      s1_len_timer <= 0;
      s1_env_vol <= 0;
      s1_env_timer <= 0;
      s1_freq_timer <= 0;
      s1_phase <= 0;

      s2_len_timer_init <= 0;
      s2_duty <= 0;
      s2_env_timer_init <= 0;
      s2_env_add <= 0;
      s2_env_vol_init <= 0;
      s2_freq_timer_init <= 0;
      s2_len_en <= 0;
      s2_trig <= 0;

      s2_len_timer <= 0;
      s2_running <= 0;
      s2_env_timer <= 0;
      s2_env_vol <= 0;
      s2_freq_timer <= 0;
      s2_phase <= 0;

      s3_power <= 0;
      s3_len_timer_init <= 0;
      s3_volume_shift <= 0;
      s3_freq_timer_init <= 0;
      s3_len_en <= 0;
      s3_trig <= 0;

      s3_running <= 0;
      s3_len_timer <= 0;
      s3_freq_timer <= 0;
      s3_phase <= 0;

      for (i = 0; i < 16; i = i + 1) begin
        s3_wave[i] <= 0;
      end

      s4_len_timer_init <= 0;
      s4_env_timer_init <= 0;
      s4_env_add <= 0;
      s4_env_vol_init <= 0;
      s4_freq_timer_init <= 0;
      s4_mode <= 0;
      s4_shift <= 0;
      s4_len_en <= 0;
      s4_trig <= 0;

      s4_running <= 0;
      s4_len_timer <= 0;
      s4_env_timer <= 0;
      s4_env_vol <= 0;
      s4_freq_timer <= 0;
      s4_lfsr <= 0;

      volume_l <= 0;
      volume_r <= 0;

      mix_r1 <= 0;
      mix_r2 <= 0;
      mix_r3 <= 0;
      mix_r4 <= 0;
      mix_l1 <= 0;
      mix_l2 <= 0;
      mix_l3 <= 0;
      mix_l4 <= 0;

      spu_power <= 0;

    end
    else begin
      logic[15:0] spu_clock_new;
      logic[15:0] spu_tick;
      logic sweep_tick;
      logic length_tick;
      logic env_tick;
      spu_clock_new = spu_clock_old + 1;
      spu_tick = (~spu_clock_old) & (spu_clock_new);

      sweep_tick  = spu_tick[12];
      length_tick = spu_tick[11];
      env_tick    = spu_tick[13];
      spu_clock_old <= spu_clock_new;

      if (tick_read) begin
        case (tick_addr)
          16'hFF10: data_out <= {1'd1, s1_sweep_timer_init, s1_sweep_dir, s1_sweep_shift};
          16'hFF11: data_out <= {s1_duty, s1_len_timer_init};
          16'hFF12: data_out <= {s1_env_vol_init, s1_env_add, s1_env_timer_init};
          16'hFF13: data_out <= s1_freq_timer_init[7:0];
          16'hFF14: data_out <= {s1_trig, s1_len_en, 3'b111, s1_freq_timer_init[10:8]};

            //----------

          16'hFF16: data_out <= {s2_duty, s2_len_timer_init};
          16'hFF17: data_out <= {s2_env_vol_init, s2_env_add, s2_env_timer_init};
          16'hFF18: data_out <= s2_freq_timer_init[7:0];
          16'hFF19: data_out <= {s2_trig, s2_len_en, 3'b111, s2_freq_timer_init[10:8]};

            //----------

          16'hFF1A: data_out <= {s3_power, 7'b1111111};
          16'hFF1B: data_out <= s3_len_timer_init;

          // metron didn't like the block without {}
          16'hFF1C: begin
            case (s3_volume_shift)
              0: data_out <= 8'b01000000;
              1: data_out <= 8'b10000000;
              2: data_out <= 8'b11000000;
              4: data_out <= 8'b00000000;
            endcase
          end
          16'hFF1D: data_out <= s3_freq_timer_init[7:0];
          16'hFF1E: data_out <= {s3_trig, s3_len_en, 3'b111, s3_freq_timer_init[10:8]};

            //----------

          16'hFF20: data_out <= {2'b11, s4_len_timer_init};
          16'hFF21: data_out <= {s4_env_vol_init, s4_env_add, s4_env_timer_init};
          16'hFF22: data_out <= {s4_shift, s4_mode, s4_freq_timer_init};
          16'hFF23: data_out <= {s4_trig, s4_len_en, 6'b111111};

            //----------

          16'hFF24: data_out <= {1'd0, volume_l, 1'd0, volume_r};
          16'hFF25: data_out <= {mix_l4, mix_l3, mix_l2, mix_l1, mix_r4, mix_r3, mix_r2, mix_r1};
          16'hFF26: data_out <= {spu_power, 7'd0};

          // "default: break didn't work?"
          default: begin  end
        endcase
      end

      //----------
      // s1 clock

      if (s1_freq_timer == 11'b11111111111) begin
        s1_phase <= s1_phase + 1;
        s1_freq_timer <= s1_sweep_timer_init ? s1_sweep_freq : s1_freq_timer_init;
      end
      else begin
        s1_freq_timer <= s1_freq_timer + 1;
      end

      //----------
      // s1 length

      if (length_tick && s1_running && s1_len_en) begin
        if (s1_len_timer == 6'b111111) begin
          s1_len_timer <= 0;
          s1_running <= 0;
        end
        else begin
          s1_len_timer <= s1_len_timer + 1;
        end
      end

      //----------
      // s1 sweep

      if (sweep_tick && s1_sweep_timer_init && s1_sweep_shift) begin
        if (s1_sweep_timer) begin
          s1_sweep_timer <= s1_sweep_timer - 1;
        end
        else begin
          logic[10:0] delta;
          logic[11:0] next_freq;
          delta = s1_sweep_freq >> s1_sweep_shift;
          next_freq = s1_sweep_freq + (s1_sweep_dir ? -delta : +delta);
          if (next_freq > 2047) s1_running <= 0;
          s1_sweep_timer <= s1_sweep_timer_init;
          s1_sweep_freq <= next_freq;
        end
      end



    end
  end

   // signals
   // signals

  //----------------------------------------

/*private:*/

  logic[15:0] spu_clock_old;
  logic[7:0]  data_out;

  // Channel 1

  /*NR10*/ logic[2:0]  s1_sweep_shift;
  /*NR10*/ logic  s1_sweep_dir;
  /*NR10*/ logic[2:0]  s1_sweep_timer_init;
  /*NR11*/ logic[5:0]  s1_len_timer_init;
  /*NR11*/ logic[1:0]  s1_duty;
  /*NR12*/ logic[2:0]  s1_env_timer_init;
  /*NR12*/ logic  s1_env_add;
  /*NR12*/ logic[3:0]  s1_env_vol_init;
  /*NR13*/ logic[10:0] s1_freq_timer_init;
  /*NR14*/ logic  s1_len_en;
  /*NR14*/ logic  s1_trig;

  logic  s1_running;
  logic[2:0]  s1_sweep_timer;
  logic[10:0] s1_sweep_freq;
  logic[5:0]  s1_len_timer;
  logic[3:0]  s1_env_vol;
  logic[2:0]  s1_env_timer;
  logic[10:0] s1_freq_timer;
  logic[2:0]  s1_phase;

  // Channel 2

  /*NR21*/ logic[5:0]  s2_len_timer_init;
  /*NR21*/ logic[1:0]  s2_duty;
  /*NR22*/ logic[2:0]  s2_env_timer_init;
  /*NR22*/ logic  s2_env_add;
  /*NR22*/ logic[3:0]  s2_env_vol_init;
  /*NR23*/ logic[10:0] s2_freq_timer_init;
  /*NR24*/ logic  s2_len_en;
  /*NR24*/ logic  s2_trig;

  logic[5:0]  s2_len_timer;
  logic  s2_running;
  logic[2:0]  s2_env_timer;
  logic[3:0]  s2_env_vol;
  logic[10:0] s2_freq_timer;
  logic[2:0]  s2_phase;

  // Channel 3

  /*NR30*/ logic  s3_power;
  /*NR31*/ logic[7:0]  s3_len_timer_init;
  /*NR32*/ logic[2:0]  s3_volume_shift;
  /*NR33*/ logic[10:0] s3_freq_timer_init;
  /*NR34*/ logic  s3_len_en;
  /*NR34*/ logic  s3_trig;

  logic  s3_running;
  logic[7:0]  s3_len_timer;
  logic[10:0] s3_freq_timer;
  logic[4:0]  s3_phase;
  logic[7:0]  s3_wave[16];

  // Channel 4

  /*NR41*/ logic[5:0]  s4_len_timer_init;
  /*NR42*/ logic[2:0]  s4_env_timer_init;
  /*NR42*/ logic  s4_env_add;
  /*NR42*/ logic[3:0]  s4_env_vol_init;
  /*NR43*/ logic[2:0]  s4_freq_timer_init;
  /*NR43*/ logic  s4_mode;
  /*NR43*/ logic[3:0]  s4_shift;
  /*NR44*/ logic  s4_len_en;
  /*NR44*/ logic  s4_trig;

  logic  s4_running;
  logic[5:0]  s4_len_timer;
  logic[2:0]  s4_env_timer;
  logic[3:0]  s4_env_vol;
  logic[2:0]  s4_freq_timer;
  logic[15:0] s4_lfsr;

  // SPU Control Registers

  /*NR50*/ logic[3:0]  volume_l;
  /*NR50*/ logic[3:0]  volume_r;

  /*NR51*/ logic  mix_r1;
  /*NR51*/ logic  mix_r2;
  /*NR51*/ logic  mix_r3;
  /*NR51*/ logic  mix_r4;
  /*NR51*/ logic  mix_l1;
  /*NR51*/ logic  mix_l2;
  /*NR51*/ logic  mix_l3;
  /*NR51*/ logic  mix_l4;

  /*NR52*/ logic  spu_power;

endmodule
