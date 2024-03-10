class ModuleWithFunction {
public:

  // Tock methods can call pure functions.
  int get_signal1(int x) {
    return reg1_ + my_func(x);
  }

  // Tick methods can call pure functions.
  void update_reg() {
    reg1_ = my_func(reg1_);
  }

private:
  int reg1_;

  // This function is called elsewhere in the module, so it would not be
  // added to the port list even if it were public.
  int my_func(int x) const {
    return x + 17;
  }

  // This function would appear in the port list if it were public.
  // Uncomment the line below to see the difference.
//public:
  int my_func2(int x) const {
    return x + 17;
  }
};

//----------------------------------------

class ModuleWithTask {
public:
  int reg1_;
  int reg2_;

  // The top-level tick method will become an always_ff.
  void tick1() {
    reg1_ = reg1_ + 1;
    tick2();
  }

  // Tick methods called by other tick methods will become tasks.
  void tick2() {
    reg2_ = reg2_ + 1;
  }
};
