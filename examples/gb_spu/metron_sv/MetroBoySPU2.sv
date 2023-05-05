`include "metron/tools/metron_tools.sv"

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
/*public*/

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
      logic lfsr_clock_old;
      logic lfsr_clock_new;
      spu_clock_new = spu_clock_old + 1;
      spu_tick = (~spu_clock_old) & (spu_clock_new);

      sweep_tick  = spu_tick[12];
      length_tick = spu_tick[11];
      env_tick    = spu_tick[13];

      //----------------------------------------
      // Reg read

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
          default: begin end
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

      //----------
      // s1 env

      if (env_tick && s1_env_timer_init) begin
        if (s1_env_timer) begin
          s1_env_timer <= s1_env_timer - 1;
        end
        else begin
          s1_env_timer <= s1_env_timer_init;
          if (s1_env_add) begin if (s1_env_vol < 15) s1_env_vol <= s1_env_vol + 1; end
          else            begin if (s1_env_vol >  0) s1_env_vol <= s1_env_vol - 1; end
        end
      end

      //----------
      // s2 clock

      if (s2_freq_timer == 12'h7FF) begin
        s2_phase <= s2_phase + 1;
        s2_freq_timer <= s2_freq_timer_init;
      end
      else begin
        s2_freq_timer <= s2_freq_timer + 1;
      end

      //----------
      // s2 length

      if (length_tick && s2_running && s2_len_en) begin
        if (s2_len_timer == 8'h3F) begin
          s2_len_timer <= 0;
          s2_running <= 0;
        end
        else begin
          s2_len_timer <= s2_len_timer + 1;
        end
      end

      //----------
      // s2 env

      if (env_tick && s2_env_timer_init) begin
        if (s2_env_timer) begin
          s2_env_timer <= s2_env_timer - 1;
        end
        else begin
          s2_env_timer <= s2_env_timer_init;
          if (s2_env_add) begin if (s2_env_vol < 15) s2_env_vol <= s2_env_vol + 1; end
          else            begin if (s2_env_vol >  0) s2_env_vol <= s2_env_vol - 1; end
        end
      end

      //----------
      // s3 clock - we run this twice because s3's timer ticks at 2 mhz

      begin
        logic[4:0] next_phase;
        logic[10:0] next_timer;
        next_phase = s3_phase;
        next_timer = s3_freq_timer;

        if (next_timer == 12'h7FF) begin
          next_phase = next_phase + 1;
          next_timer = s3_freq_timer_init;
        end
        else begin
          next_timer = next_timer + 1;
        end

        if (next_timer == 12'h7FF) begin
          next_phase = next_phase + 1;
          next_timer = s3_freq_timer_init;
        end
        else begin
          next_timer = next_timer + 1;
        end

        s3_phase <= next_phase;
        s3_freq_timer <= next_timer;
      end

      //----------
      // s3 length

      if (length_tick && s3_running && s3_len_en) begin
        if (s3_len_timer == 8'hFF) begin
          s3_len_timer <= 0;
          s3_running <= 0;
        end
        else begin
          s3_len_timer <= s3_len_timer + 1;
        end
      end

      //----------
      // s4 lfsr

      lfsr_clock_old = spu_clock_old[s4_shift + 1];
      lfsr_clock_new = spu_clock_new[s4_shift + 1];

      /*
      logic<1> lfsr_clock_old = 0;
      logic<1> lfsr_clock_new = 0;

      switch(s4_shift) {
        case 0 : { lfsr_clock_old = spu_clock_old[1 ]; lfsr_clock_new = spu_clock_new[1 ]; break; }
        case 1 : { lfsr_clock_old = spu_clock_old[2 ]; lfsr_clock_new = spu_clock_new[2 ]; break; }
        case 2 : { lfsr_clock_old = spu_clock_old[3 ]; lfsr_clock_new = spu_clock_new[3 ]; break; }
        case 3 : { lfsr_clock_old = spu_clock_old[4 ]; lfsr_clock_new = spu_clock_new[4 ]; break; }
        case 4 : { lfsr_clock_old = spu_clock_old[5 ]; lfsr_clock_new = spu_clock_new[5 ]; break; }
        case 5 : { lfsr_clock_old = spu_clock_old[6 ]; lfsr_clock_new = spu_clock_new[6 ]; break; }
        case 6 : { lfsr_clock_old = spu_clock_old[7 ]; lfsr_clock_new = spu_clock_new[7 ]; break; }
        case 7 : { lfsr_clock_old = spu_clock_old[8 ]; lfsr_clock_new = spu_clock_new[8 ]; break; }
        case 8 : { lfsr_clock_old = spu_clock_old[9 ]; lfsr_clock_new = spu_clock_new[9 ]; break; }
        case 9 : { lfsr_clock_old = spu_clock_old[10]; lfsr_clock_new = spu_clock_new[10]; break; }
        case 10: { lfsr_clock_old = spu_clock_old[11]; lfsr_clock_new = spu_clock_new[11]; break; }
        case 11: { lfsr_clock_old = spu_clock_old[12]; lfsr_clock_new = spu_clock_new[12]; break; }
        case 12: { lfsr_clock_old = spu_clock_old[13]; lfsr_clock_new = spu_clock_new[13]; break; }
        case 13: { lfsr_clock_old = spu_clock_old[14]; lfsr_clock_new = spu_clock_new[14]; break; }
        case 14: { lfsr_clock_old = spu_clock_old[15]; lfsr_clock_new = spu_clock_new[15]; break; }
        case 15: { lfsr_clock_old = 0;                 lfsr_clock_new = 0;                 break; }
      }
      */

      if ((lfsr_clock_old == 0) && (lfsr_clock_new == 1)) begin
        if (s4_freq_timer) begin
          s4_freq_timer <= s4_freq_timer - 1;
        end
        else begin
          logic new_bit;
          new_bit = s4_lfsr[15] ^ s4_lfsr[14] ^ 1;
          s4_lfsr <= {
            s4_lfsr[14:9],
            s4_mode ? new_bit : s4_lfsr[8],
            s4_lfsr[7:0],
            new_bit};
          s4_freq_timer <= s4_freq_timer_init;
        end
      end

      //----------
      // s4 length

      if (length_tick && s4_running && s4_len_en) begin
        if (s4_len_timer == 8'h3F) begin
          s4_len_timer <= 0;
          s4_running <= 0;
        end
        else begin
          s4_len_timer <= s4_len_timer + 1;
        end
      end

      //----------
      // s4 env

      if (env_tick && s4_env_timer_init) begin
        if (s4_env_timer) begin
          s4_env_timer <= s4_env_timer - 1;
        end
        else begin
          s4_env_timer <= s4_env_timer_init;
          if (s4_env_add) begin if (s4_env_vol < 15) s4_env_vol <= s4_env_vol + 1; end
          else            begin if (s4_env_vol >  0) s4_env_vol <= s4_env_vol - 1; end
        end
      end

      //----------
      // Triggers

      if (s1_trig && (s1_env_vol_init || s1_env_add)) begin
        s1_running     <= 1;
        s1_len_timer   <= s1_len_timer_init;
        s1_sweep_timer <= s1_sweep_timer_init;
        s1_sweep_freq  <= s1_freq_timer_init;
        s1_env_vol     <= s1_env_vol_init;
        s1_env_timer   <= s1_env_timer_init;
        s1_freq_timer  <= s1_freq_timer_init;
        s1_phase       <= 0;
        s1_trig        <= 0;
      end

      if (s2_trig && (s2_env_vol_init || s2_env_add)) begin
        s2_running    <= 1;
        s2_len_timer  <= s2_len_timer_init;
        s2_env_vol    <= s2_env_vol_init;
        s2_env_timer  <= s2_env_timer_init;
        s2_freq_timer <= s2_freq_timer_init;
        s2_phase      <= 0;
        s2_trig       <= 0;
      end

      if (s3_trig) begin
        s3_running    <= 1;
        s3_len_timer  <= s3_len_timer_init;
        s3_freq_timer <= s3_freq_timer_init;
        s3_phase      <= 0;
        s3_trig       <= 0;
      end

      if (s4_trig && (s4_env_vol_init || s4_env_add)) begin
        s4_running    <= 1;
        s4_len_timer  <= s4_len_timer_init;
        s4_env_vol    <= s4_env_vol_init;
        s4_env_timer  <= s4_env_timer_init;
        s4_freq_timer <= s4_freq_timer_init;
        s4_lfsr       <= 0;
        s4_trig       <= 0;
      end

      //----------
      // Register writes

      if (tick_write && tick_addr >= 16'hFF10 && tick_addr <= 16'hFF26) begin
        case (tick_addr)
          16'hFF10: begin
            s1_sweep_shift      <= tick_data_in[2:0];
            s1_sweep_dir        <= tick_data_in[3];
            s1_sweep_timer_init <= tick_data_in[6:4];
          end
          16'hFF11: begin
            s1_len_timer_init <= tick_data_in[5:0];
            s1_duty           <= tick_data_in[7:6];
          end
          16'hFF12: begin
            s1_env_timer_init <= tick_data_in[2:0];
            s1_env_add        <= tick_data_in[3];
            s1_env_vol_init   <= tick_data_in[7:4];
          end
          16'hFF13: begin
            s1_freq_timer_init <= {s1_freq_timer_init[10:8], tick_data_in};
          end
          16'hFF14: begin
            s1_freq_timer_init <= {tick_data_in[2:0], s1_freq_timer_init[7:0]};
            s1_len_en          <= tick_data_in[6];
            s1_trig            <= tick_data_in[7];
          end

          //----------

          16'hFF16: begin
            s2_len_timer_init <= tick_data_in[5:0];
            s2_duty           <= tick_data_in[7:6];
          end
          16'hFF17: begin
            s2_env_timer_init <= tick_data_in[2:0];
            s2_env_add        <= tick_data_in[3];
            s2_env_vol_init   <= tick_data_in[7:4];
          end
          16'hFF18: begin
            s2_freq_timer_init <= {s2_freq_timer_init[10:8], tick_data_in};
          end
          16'hFF19: begin
            s2_freq_timer_init <= {tick_data_in[2:0], s2_freq_timer_init[7:0]};
            s2_len_en          <= tick_data_in[6];
            s2_trig            <= tick_data_in[7];
          end

          //----------

          16'hFF1A: begin
            s3_power <= tick_data_in[7];
          end
          16'hFF1B: begin
            s3_len_timer_init <= tick_data_in[7:0];
          end
          16'hFF1C: begin
            case (tick_data_in[6:5])
              0: s3_volume_shift <= 4;
              1: s3_volume_shift <= 0;
              2: s3_volume_shift <= 1;
              3: s3_volume_shift <= 2;
            endcase
          end
          16'hFF1D: begin
            s3_freq_timer_init <= {s3_freq_timer_init[10:8], tick_data_in};
          end
          16'hFF1E: begin
            s3_freq_timer_init <= {tick_data_in[2:0], s3_freq_timer_init[7:0]};
            s3_len_en          <= tick_data_in[6];
            s3_trig            <= tick_data_in[7];
          end

          //----------

          16'hFF20: begin
            s4_len_timer_init <= tick_data_in[5:0];
          end
          16'hFF21: begin
            s4_env_timer_init <= tick_data_in[2:0];
            s4_env_add        <= tick_data_in[3];
            s4_env_vol_init   <= tick_data_in[7:4];
          end
          16'hFF22: begin
            s4_freq_timer_init <= tick_data_in[2:0];
            s4_mode            <= tick_data_in[3];
            s4_shift           <= tick_data_in[7:4];
          end
          16'hFF23: begin
            s4_len_en <= tick_data_in[6];
            s4_trig   <= tick_data_in[7];
          end

          //----------

          16'hFF24: begin
            volume_r <= tick_data_in[2:0] + 1;
            volume_l <= tick_data_in[6:4] + 1;
          end
          16'hFF25: begin
            mix_r1 <= tick_data_in[0];
            mix_r2 <= tick_data_in[1];
            mix_r3 <= tick_data_in[2];
            mix_r4 <= tick_data_in[3];
            mix_l1 <= tick_data_in[4];
            mix_l2 <= tick_data_in[5];
            mix_l3 <= tick_data_in[6];
            mix_l4 <= tick_data_in[7];
          end
          16'hFF26: begin
            spu_power <= tick_data_in[7];
          end
          default: begin
          end
        endcase
      end

      //----------
      // Wavetable writes

      if (tick_write && tick_addr >= 16'hFF30 && tick_addr <= 16'hFF3F) begin
        s3_wave[tick_addr & 4'hF] <= tick_data_in;
      end

      spu_clock_old <= spu_clock_new;
    end
  end

  //----------------------------------------

  /*
  void dump(Dumper& d) const {
    d("\002--------------SPU--------------\001\n");

    const char* bar = "===============";

    logic<4> s3_env_vol = 0;
    switch (s3_volume_shift) {
    case 0: s3_env_vol = 15; break;
    case 1: s3_env_vol = 7; break;
    case 2: s3_env_vol = 3; break;
    case 4: s3_env_vol = 0; break;
    }

    d("s1 running %d\n", s1_running);
    d("s2 running %d\n", s2_running);
    d("s3 running %d\n", s3_running);
    d("s4 running %d\n", s4_running);

    d("s1 len %d\n", s1_len_timer);
    d("s2 len %d\n", s2_len_timer);
    d("s3 len %d\n", s3_len_timer);
    d("s4 len %d\n", s4_len_timer);

    d("s1 env timer %d\n", s1_env_timer);
    d("s2 env timer %d\n", s2_env_timer);
    d("s4 env timer %d\n", s4_env_timer);

    d("s1 vol %2d %s\n", s1_env_vol, bar + 15 - s1_env_vol);
    d("s2 vol %2d %s\n", s2_env_vol, bar + 15 - s2_env_vol);
    d("s3 vol %2d %s\n", s3_env_vol, bar + 15 - s3_env_vol);
    d("s4 vol %2d %s\n", s4_env_vol, bar + 15 - s4_env_vol);

    d("s1 sweep timer %2d\n", s1_sweep_timer);
    d("s1 sweep freq  %2d\n", s1_sweep_freq);

    d("s1 freq timer %d\n", s1_freq_timer);
    d("s2 freq timer %d\n", s2_freq_timer);
    d("s3 freq timer %d\n", s3_freq_timer);
    d("s4 freq timer %d\n", s4_freq_timer);

    d("s1 phase %d\n", s1_phase);
    d("s2 phase %d\n", s2_phase);
    d("s3 phase %d\n", s3_phase);

    d("s4 lfsr 0x%04x\n", s4_lfsr);

    char buf[33];
    for (int i = 0; i < 16; i++) {
      logic<4> a = b4(s3_wave[i], 4);
      logic<4> b = b4(s3_wave[i], 0);

      buf[2 * i + 0] = a > 9 ? 'A' + a - 10 : '0' + a;
      buf[2 * i + 1] = b > 9 ? 'B' + b - 10 : '0' + b;
    }

    buf[32] = 0;
    d("[%s]\n", buf);
  }
  */

  // signals
  // signals

  //----------------------------------------

/*private*/

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
