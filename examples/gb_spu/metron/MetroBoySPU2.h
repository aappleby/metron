#ifndef METRO_BOY_SPU2_H
#define METRO_BOY_SPU2_H
#include "metron/metron_tools.h"

//------------------------------------------------------------------------------

class MetroBoySPU2 {
public:

  //----------------------------------------

  void tock_out() {
    logic<9> l;
    logic<9> r;

    l = 0;
    r = 0;

    if (s1_running_ && s1_env_vol_) {
      logic<1> s1_out = 0;
      switch(s1_duty_) {
        case 0: s1_out = s1_phase_ < 1; break;
        case 1: s1_out = s1_phase_ < 2; break;
        case 2: s1_out = s1_phase_ < 4; break;
        case 3: s1_out = s1_phase_ < 6; break;
      }
      if (mix_l1_ && s1_out) l = l + s1_env_vol_;
      if (mix_r1_ && s1_out) r = r + s1_env_vol_;
    }

    if (s2_running_ && s2_env_vol_) {
      logic<1> s2_out = 0;
      switch(s2_duty_) {
        case 0: s2_out = s2_phase_ < 1; break;
        case 1: s2_out = s2_phase_ < 2; break;
        case 2: s2_out = s2_phase_ < 4; break;
        case 3: s2_out = s2_phase_ < 6; break;
      }
      if (mix_l2_ && s2_out) l = l + s2_env_vol_;
      if (mix_r2_ && s2_out) r = r + s2_env_vol_;
    }

    if (s3_running_ && s3_power_) {
      logic<8> s3_sample = s3_wave_[s3_phase_ >> 1];
      logic<4> s3_out = (s3_phase_ & 1) ? b4(s3_sample, 0) : b4(s3_sample, 4);
      s3_out = s3_out >> s3_volume_shift_;
      if (mix_l3_) l = l + s3_out;
      if (mix_r3_) r = r + s3_out;
    }

    if (s4_running_ && s4_env_vol_) {
      logic<1> s4_out = b1(s4_lfsr_, 15);
      if (mix_l4_ && s4_out) l = l + s4_env_vol_;
      if (mix_r4_ && s4_out) r = r + s4_env_vol_;
    }

    l = l * volume_l_;
    r = r * volume_r_;

    out_l = l;
    out_r = r;
  }

  //----------------------------------------

  void tick(logic<1> reset, logic<16> addr, logic<8> data_in, logic<1> read, logic<1> write) {
    if (reset) {
      spu_clock_old_ = 0;
      data_out_ = 0;

      s1_sweep_shift_ = 0;
      s1_sweep_dir_ = 0;
      s1_sweep_timer_init_ = 0;
      s1_len_timer_init_ = 0;
      s1_duty_ = 0;
      s1_env_timer_init_ = 0;
      s1_env_add_ = 0;
      s1_env_vol_init_ = 0;
      s1_freq_timer_init_ = 0;
      s1_len_en_ = 0;
      s1_trig_ = 0;
      s1_running_ = 0;
      s1_sweep_timer_ = 0;
      s1_sweep_freq_ = 0;
      s1_len_timer_ = 0;
      s1_env_vol_ = 0;
      s1_env_timer_ = 0;
      s1_freq_timer_ = 0;
      s1_phase_ = 0;

      s2_len_timer_init_ = 0;
      s2_duty_ = 0;
      s2_env_timer_init_ = 0;
      s2_env_add_ = 0;
      s2_env_vol_init_ = 0;
      s2_freq_timer_init_ = 0;
      s2_len_en_ = 0;
      s2_trig_ = 0;

      s2_len_timer_ = 0;
      s2_running_ = 0;
      s2_env_timer_ = 0;
      s2_env_vol_ = 0;
      s2_freq_timer_ = 0;
      s2_phase_ = 0;

      s3_power_ = 0;
      s3_len_timer_init_ = 0;
      s3_volume_shift_ = 0;
      s3_freq_timer_init_ = 0;
      s3_len_en_ = 0;
      s3_trig_ = 0;

      s3_running_ = 0;
      s3_len_timer_ = 0;
      s3_freq_timer_ = 0;
      s3_phase_ = 0;

      for (int i = 0; i < 16; i++) {
        s3_wave_[i] = 0;
      }

      s4_len_timer_init_ = 0;
      s4_env_timer_init_ = 0;
      s4_env_add_ = 0;
      s4_env_vol_init_ = 0;
      s4_freq_timer_init_ = 0;
      s4_mode_ = 0;
      s4_shift_ = 0;
      s4_len_en_ = 0;
      s4_trig_ = 0;

      s4_running_ = 0;
      s4_len_timer_ = 0;
      s4_env_timer_ = 0;
      s4_env_vol_ = 0;
      s4_freq_timer_ = 0;
      s4_lfsr_ = 0;

      volume_l_ = 0;
      volume_r_ = 0;

      mix_r1_ = 0;
      mix_r2_ = 0;
      mix_r3_ = 0;
      mix_r4_ = 0;
      mix_l1_ = 0;
      mix_l2_ = 0;
      mix_l3_ = 0;
      mix_l4_ = 0;

      spu_power_ = 0;

    }
    else {
      logic<16> spu_clock_new = spu_clock_old_ + 1;
      logic<16> spu_tick = (~spu_clock_old_) & (spu_clock_new);

      logic<1> sweep_tick  = b1(spu_tick, 12);
      logic<1> length_tick = b1(spu_tick, 11);
      logic<1> env_tick    = b1(spu_tick, 13);

      //----------------------------------------
      // Reg read

      if (read) {
        switch (addr) {
          case 0xFF10: data_out_ = cat(b1(1), s1_sweep_timer_init_, s1_sweep_dir_, s1_sweep_shift_); break;
          case 0xFF11: data_out_ = cat(s1_duty_, s1_len_timer_init_); break;
          case 0xFF12: data_out_ = cat(s1_env_vol_init_, s1_env_add_, s1_env_timer_init_); break;
          case 0xFF13: data_out_ = b8(s1_freq_timer_init_, 0); break;
          case 0xFF14: data_out_ = cat(s1_trig_, s1_len_en_, b3(0b111), b3(s1_freq_timer_init_, 8)); break;

            //----------

          case 0xFF16: data_out_ = cat(s2_duty_, s2_len_timer_init_); break;
          case 0xFF17: data_out_ = cat(s2_env_vol_init_, s2_env_add_, s2_env_timer_init_); break;
          case 0xFF18: data_out_ = b8(s2_freq_timer_init_, 0); break;
          case 0xFF19: data_out_ = cat(s2_trig_, s2_len_en_, b3(0b111), b3(s2_freq_timer_init_, 8)); break;

            //----------

          case 0xFF1A: data_out_ = cat(s3_power_, b7(0b1111111)); break;
          case 0xFF1B: data_out_ = s3_len_timer_init_; break;

          // metron didn't like the block without {}
          case 0xFF1C: {
            switch (s3_volume_shift_) {
              case 0: data_out_ = 0b01000000; break;
              case 1: data_out_ = 0b10000000; break;
              case 2: data_out_ = 0b11000000; break;
              case 4: data_out_ = 0b00000000; break;
            }
            break;
          }
          case 0xFF1D: data_out_ = b8(s3_freq_timer_init_, 0); break;
          case 0xFF1E: data_out_ = cat(s3_trig_, s3_len_en_, b3(0b111), b3(s3_freq_timer_init_, 8)); break;

            //----------

          case 0xFF20: data_out_ = cat(b2(0b11), s4_len_timer_init_); break;
          case 0xFF21: data_out_ = cat(s4_env_vol_init_, s4_env_add_, s4_env_timer_init_); break;
          case 0xFF22: data_out_ = cat(s4_shift_, s4_mode_, s4_freq_timer_init_); break;
          case 0xFF23: data_out_ = cat(s4_trig_, s4_len_en_, b6(0b111111)); break;

            //----------

          case 0xFF24: data_out_ = cat(b1(0), volume_l_, b1(0), volume_r_); break;
          case 0xFF25: data_out_ = cat(mix_l4_, mix_l3_, mix_l2_, mix_l1_, mix_r4_, mix_r3_, mix_r2_, mix_r1_); break;
          case 0xFF26: data_out_ = cat(spu_power_, b7(0)); break;

          // "default: break didn't work?"
          default: { break; }
        }
      }

      //----------
      // s1 clock

      if (s1_freq_timer_ == 0b11111111111) {
        s1_phase_ = s1_phase_ + 1;
        s1_freq_timer_ = s1_sweep_timer_init_ ? s1_sweep_freq_ : s1_freq_timer_init_;
      }
      else {
        s1_freq_timer_ = s1_freq_timer_ + 1;
      }

      //----------
      // s1 length

      if (length_tick && s1_running_ && s1_len_en_) {
        if (s1_len_timer_ == 0b111111) {
          s1_len_timer_ = 0;
          s1_running_ = 0;
        }
        else {
          s1_len_timer_ = s1_len_timer_ + 1;
        }
      }

      //----------
      // s1 sweep

      if (sweep_tick && s1_sweep_timer_init_ && s1_sweep_shift_) {
        if (s1_sweep_timer_) {
          s1_sweep_timer_ = s1_sweep_timer_ - 1;
        }
        else {
          logic<11> delta = s1_sweep_freq_ >> s1_sweep_shift_;
          logic<12> next_freq = s1_sweep_freq_ + (s1_sweep_dir_ ? -delta : +delta);
          if (next_freq > 2047) s1_running_ = 0;
          s1_sweep_timer_ = s1_sweep_timer_init_;
          s1_sweep_freq_ = next_freq;
        }
      }

      //----------
      // s1 env

      if (env_tick && s1_env_timer_init_) {
        if (s1_env_timer_) {
          s1_env_timer_ = s1_env_timer_ - 1;
        }
        else {
          s1_env_timer_ = s1_env_timer_init_;
          if (s1_env_add_) { if (s1_env_vol_ < 15) s1_env_vol_ = s1_env_vol_ + 1; }
          else            { if (s1_env_vol_ >  0) s1_env_vol_ = s1_env_vol_ - 1; }
        }
      }

      //----------
      // s2 clock

      if (s2_freq_timer_ == 0x7FF) {
        s2_phase_ = s2_phase_ + 1;
        s2_freq_timer_ = s2_freq_timer_init_;
      }
      else {
        s2_freq_timer_ = s2_freq_timer_ + 1;
      }

      //----------
      // s2 length

      if (length_tick && s2_running_ && s2_len_en_) {
        if (s2_len_timer_ == 0x3F) {
          s2_len_timer_ = 0;
          s2_running_ = 0;
        }
        else {
          s2_len_timer_ = s2_len_timer_ + 1;
        }
      }

      //----------
      // s2 env

      if (env_tick && s2_env_timer_init_) {
        if (s2_env_timer_) {
          s2_env_timer_ = s2_env_timer_ - 1;
        }
        else {
          s2_env_timer_ = s2_env_timer_init_;
          if (s2_env_add_) { if (s2_env_vol_ < 15) s2_env_vol_ = s2_env_vol_ + 1; }
          else            { if (s2_env_vol_ >  0) s2_env_vol_ = s2_env_vol_ - 1; }
        }
      }

      //----------
      // s3 clock - we run this twice because s3's timer ticks at 2 mhz

      {
        logic<5> next_phase = s3_phase_;
        logic<11> next_timer = s3_freq_timer_;

        if (next_timer == 0x7FF) {
          next_phase = next_phase + 1;
          next_timer = s3_freq_timer_init_;
        }
        else {
          next_timer = next_timer + 1;
        }

        if (next_timer == 0x7FF) {
          next_phase = next_phase + 1;
          next_timer = s3_freq_timer_init_;
        }
        else {
          next_timer = next_timer + 1;
        }

        s3_phase_ = next_phase;
        s3_freq_timer_ = next_timer;
      }

      //----------
      // s3 length

      if (length_tick && s3_running_ && s3_len_en_) {
        if (s3_len_timer_ == 0xFF) {
          s3_len_timer_ = 0;
          s3_running_ = 0;
        }
        else {
          s3_len_timer_ = s3_len_timer_ + 1;
        }
      }

      //----------
      // s4 lfsr

      logic<1> lfsr_clock_old = spu_clock_old_[s4_shift_ + 1];
      logic<1> lfsr_clock_new = spu_clock_new[s4_shift_ + 1];

      if ((lfsr_clock_old == 0) && (lfsr_clock_new == 1)) {
        if (s4_freq_timer_) {
          s4_freq_timer_ = s4_freq_timer_ - 1;
        }
        else {
          logic<1> new_bit = s4_lfsr_[15] ^ s4_lfsr_[14] ^ 1;
          s4_lfsr_ = cat(
            b6(s4_lfsr_, 9),
            s4_mode_ ? new_bit : s4_lfsr_[8],
            b8(s4_lfsr_, 0),
            new_bit);
          s4_freq_timer_ = s4_freq_timer_init_;
        }
      }

      //----------
      // s4 length

      if (length_tick && s4_running_ && s4_len_en_) {
        if (s4_len_timer_ == 0x3F) {
          s4_len_timer_ = 0;
          s4_running_ = 0;
        }
        else {
          s4_len_timer_ = s4_len_timer_ + 1;
        }
      }

      //----------
      // s4 env

      if (env_tick && s4_env_timer_init_) {
        if (s4_env_timer_) {
          s4_env_timer_ = s4_env_timer_ - 1;
        }
        else {
          s4_env_timer_ = s4_env_timer_init_;
          if (s4_env_add_) { if (s4_env_vol_ < 15) s4_env_vol_ = s4_env_vol_ + 1; }
          else            { if (s4_env_vol_ >  0) s4_env_vol_ = s4_env_vol_ - 1; }
        }
      }

      //----------
      // Triggers

      if (s1_trig_ && (s1_env_vol_init_ || s1_env_add_)) {
        s1_running_     = 1;
        s1_len_timer_   = s1_len_timer_init_;
        s1_sweep_timer_ = s1_sweep_timer_init_;
        s1_sweep_freq_  = s1_freq_timer_init_;
        s1_env_vol_     = s1_env_vol_init_;
        s1_env_timer_   = s1_env_timer_init_;
        s1_freq_timer_  = s1_freq_timer_init_;
        s1_phase_       = 0;
        s1_trig_        = 0;
      }

      if (s2_trig_ && (s2_env_vol_init_ || s2_env_add_)) {
        s2_running_    = 1;
        s2_len_timer_  = s2_len_timer_init_;
        s2_env_vol_    = s2_env_vol_init_;
        s2_env_timer_  = s2_env_timer_init_;
        s2_freq_timer_ = s2_freq_timer_init_;
        s2_phase_      = 0;
        s2_trig_       = 0;
      }

      if (s3_trig_) {
        s3_running_    = 1;
        s3_len_timer_  = s3_len_timer_init_;
        s3_freq_timer_ = s3_freq_timer_init_;
        s3_phase_      = 0;
        s3_trig_       = 0;
      }

      if (s4_trig_ && (s4_env_vol_init_ || s4_env_add_)) {
        s4_running_    = 1;
        s4_len_timer_  = s4_len_timer_init_;
        s4_env_vol_    = s4_env_vol_init_;
        s4_env_timer_  = s4_env_timer_init_;
        s4_freq_timer_ = s4_freq_timer_init_;
        s4_lfsr_       = 0;
        s4_trig_       = 0;
      }

      //----------
      // Register writes

      if (write && addr >= 0xFF10 && addr <= 0xFF26) {
        switch (addr) {
          case 0xFF10: {
            s1_sweep_shift_      = b3(data_in, 0);
            s1_sweep_dir_        = b1(data_in, 3);
            s1_sweep_timer_init_ = b3(data_in, 4);
            break;
          }
          case 0xFF11: {
            s1_len_timer_init_ = b6(data_in, 0);
            s1_duty_           = b2(data_in, 6);
            break;
          }
          case 0xFF12: {
            s1_env_timer_init_ = b3(data_in, 0);
            s1_env_add_        = b1(data_in, 3);
            s1_env_vol_init_   = b4(data_in, 4);
            break;
          }
          case 0xFF13: {
            s1_freq_timer_init_ = cat(b3(s1_freq_timer_init_, 8), data_in);
            break;
          }
          case 0xFF14: {
            s1_freq_timer_init_ = cat(b3(data_in, 0), b8(s1_freq_timer_init_, 0));
            s1_len_en_          = b1(data_in, 6);
            s1_trig_            = b1(data_in, 7);
            break;
          }

          //----------

          case 0xFF16: {
            s2_len_timer_init_ = b6(data_in, 0);
            s2_duty_           = b2(data_in, 6);
            break;
          }
          case 0xFF17: {
            s2_env_timer_init_ = b3(data_in, 0);
            s2_env_add_        = b1(data_in, 3);
            s2_env_vol_init_   = b4(data_in, 4);
            break;
          }
          case 0xFF18: {
            s2_freq_timer_init_ = cat(b3(s2_freq_timer_init_, 8), data_in);
            break;
          }
          case 0xFF19: {
            s2_freq_timer_init_ = cat(b3(data_in, 0), b8(s2_freq_timer_init_, 0));
            s2_len_en_          = b1(data_in, 6);
            s2_trig_            = b1(data_in, 7);
            break;
          }

          //----------

          case 0xFF1A: {
            s3_power_ = b1(data_in, 7);
            break;
          }
          case 0xFF1B: {
            s3_len_timer_init_ = b8(data_in, 0);
            break;
          }
          case 0xFF1C: {
            switch (b2(data_in, 5)) {
              case 0: s3_volume_shift_ = 4; break;
              case 1: s3_volume_shift_ = 0; break;
              case 2: s3_volume_shift_ = 1; break;
              case 3: s3_volume_shift_ = 2; break;
            }
            break;
          }
          case 0xFF1D: {
            s3_freq_timer_init_ = cat(b3(s3_freq_timer_init_, 8), data_in);
            break;
          }
          case 0xFF1E: {
            s3_freq_timer_init_ = cat(b3(data_in, 0), b8(s3_freq_timer_init_, 0));
            s3_len_en_          = b1(data_in, 6);
            s3_trig_            = b1(data_in, 7);
            break;
          }

          //----------

          case 0xFF20: {
            s4_len_timer_init_ = b6(data_in, 0);
            break;
          }
          case 0xFF21: {
            s4_env_timer_init_ = b3(data_in, 0);
            s4_env_add_        = b1(data_in, 3);
            s4_env_vol_init_   = b4(data_in, 4);
            break;
          }
          case 0xFF22: {
            s4_freq_timer_init_ = b3(data_in, 0);
            s4_mode_            = b1(data_in, 3);
            s4_shift_           = b4(data_in, 4);
            break;
          }
          case 0xFF23: {
            s4_len_en_ = b1(data_in, 6);
            s4_trig_   = b1(data_in, 7);
            break;
          }

          //----------

          case 0xFF24: {
            volume_r_ = b3(data_in, 0) + 1;
            volume_l_ = b3(data_in, 4) + 1;
            break;
          }
          case 0xFF25: {
            mix_r1_ = b1(data_in, 0);
            mix_r2_ = b1(data_in, 1);
            mix_r3_ = b1(data_in, 2);
            mix_r4_ = b1(data_in, 3);
            mix_l1_ = b1(data_in, 4);
            mix_l2_ = b1(data_in, 5);
            mix_l3_ = b1(data_in, 6);
            mix_l4_ = b1(data_in, 7);
            break;
          }
          case 0xFF26: {
            spu_power_ = b1(data_in, 7);
            break;
          }
          default: {
            break;
          }
        }
      }

      //----------
      // Wavetable writes

      if (write && addr >= 0xFF30 && addr <= 0xFF3F) {
        s3_wave_[addr & 0xF] = data_in;
      }

      spu_clock_old_ = spu_clock_new;
    }
  }

  //----------------------------------------

  logic<9>  out_r; // signals
  logic<9>  out_l; // signals

  //----------------------------------------

private:

  logic<16> spu_clock_old_;
  logic<8>  data_out_;

  // Channel 1

  /*NR10*/ logic<3>  s1_sweep_shift_;
  /*NR10*/ logic<1>  s1_sweep_dir_;
  /*NR10*/ logic<3>  s1_sweep_timer_init_;
  /*NR11*/ logic<6>  s1_len_timer_init_;
  /*NR11*/ logic<2>  s1_duty_;
  /*NR12*/ logic<3>  s1_env_timer_init_;
  /*NR12*/ logic<1>  s1_env_add_;
  /*NR12*/ logic<4>  s1_env_vol_init_;
  /*NR13*/ logic<11> s1_freq_timer_init_;
  /*NR14*/ logic<1>  s1_len_en_;
  /*NR14*/ logic<1>  s1_trig_;

  logic<1>  s1_running_;
  logic<3>  s1_sweep_timer_;
  logic<11> s1_sweep_freq_;
  logic<6>  s1_len_timer_;
  logic<4>  s1_env_vol_;
  logic<3>  s1_env_timer_;
  logic<11> s1_freq_timer_;
  logic<3>  s1_phase_;

  // Channel 2

  /*NR21*/ logic<6>  s2_len_timer_init_;
  /*NR21*/ logic<2>  s2_duty_;
  /*NR22*/ logic<3>  s2_env_timer_init_;
  /*NR22*/ logic<1>  s2_env_add_;
  /*NR22*/ logic<4>  s2_env_vol_init_;
  /*NR23*/ logic<11> s2_freq_timer_init_;
  /*NR24*/ logic<1>  s2_len_en_;
  /*NR24*/ logic<1>  s2_trig_;

  logic<6>  s2_len_timer_;
  logic<1>  s2_running_;
  logic<3>  s2_env_timer_;
  logic<4>  s2_env_vol_;
  logic<11> s2_freq_timer_;
  logic<3>  s2_phase_;

  // Channel 3

  /*NR30*/ logic<1>  s3_power_;
  /*NR31*/ logic<8>  s3_len_timer_init_;
  /*NR32*/ logic<3>  s3_volume_shift_;
  /*NR33*/ logic<11> s3_freq_timer_init_;
  /*NR34*/ logic<1>  s3_len_en_;
  /*NR34*/ logic<1>  s3_trig_;

  logic<1>  s3_running_;
  logic<8>  s3_len_timer_;
  logic<11> s3_freq_timer_;
  logic<5>  s3_phase_;
  logic<8>  s3_wave_[16];

  // Channel 4

  /*NR41*/ logic<6>  s4_len_timer_init_;
  /*NR42*/ logic<3>  s4_env_timer_init_;
  /*NR42*/ logic<1>  s4_env_add_;
  /*NR42*/ logic<4>  s4_env_vol_init_;
  /*NR43*/ logic<3>  s4_freq_timer_init_;
  /*NR43*/ logic<1>  s4_mode_;
  /*NR43*/ logic<4>  s4_shift_;
  /*NR44*/ logic<1>  s4_len_en_;
  /*NR44*/ logic<1>  s4_trig_;

  logic<1>  s4_running_;
  logic<6>  s4_len_timer_;
  logic<3>  s4_env_timer_;
  logic<4>  s4_env_vol_;
  logic<3>  s4_freq_timer_;
  logic<16> s4_lfsr_;

  // SPU Control Registers

  /*NR50*/ logic<4>  volume_l_;
  /*NR50*/ logic<4>  volume_r_;

  /*NR51*/ logic<1>  mix_r1_;
  /*NR51*/ logic<1>  mix_r2_;
  /*NR51*/ logic<1>  mix_r3_;
  /*NR51*/ logic<1>  mix_r4_;
  /*NR51*/ logic<1>  mix_l1_;
  /*NR51*/ logic<1>  mix_l2_;
  /*NR51*/ logic<1>  mix_l3_;
  /*NR51*/ logic<1>  mix_l4_;

  /*NR52*/ logic<1>  spu_power_;

};

#endif // METRO_BOY_SPU2_H
