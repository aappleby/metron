class Metron {
public:

  int my_sig;

  // Divide and mod work, but make Yosys extremely slow to synth

  void tock() {
    int x = 7;
    x += 13;
    x  -= 13;
    x   *= 13;
    x +=   13;
    x -=  13;
    x *= 13;
    my_sig = x;
  }

  int my_reg1_;
  int my_reg2_;
  int my_reg3_;
  int my_reg4_;
  int my_reg5_;
  int my_reg6_;

  void tick() {
    my_reg1_ += 22;
    my_reg2_  -= 22;
    my_reg3_   *= 22;
    my_reg4_ +=   22;
    my_reg5_ -=  22;
    my_reg6_ *= 22;
  }
};
