class ModuleWithPureFunction {
public:

  // Tock methods can call pure functions.
  int get_signal1(int x) {
    return reg1 + my_pure_func(x);
  }

  // Tick methods can call pure functions.
  void update_reg() {
    reg1 = my_pure_func(reg1);
  }

private:
  int reg1;

  // This pure function is called elsewhere in the module, so it would not be
  // added to the port list even if it were public.
  int my_pure_func(int x) const {
    return x + 17;
  }

  // This pure function would appear in the port list if it were public.
  // Uncomment the line below to see the difference.
//public:
  int my_pure_func2(int x) const {
    return x + 17;
  }
};

//----------------------------------------

class ModuleWithImpureFunction {
public:
  int sig1;
  int sig2;

  // The top-level tock method will become an always_comb
  void tock1() {
    sig1 = tock2();
  }

  // This could be a function, but right now some external tools handle this
  // case weird so we emit it as an always_comb with its own set of binding
  // variables.
  int tock2() {
    sig2 = 17;
    return sig2;
  }
};

//----------------------------------------

class ModuleWithTask {
public:
  int reg1;
  int reg2;

  // The top-level tick method will become an always_ff.
  void tick1() {
    reg1 = reg1 + 1;
    tick2();
  }

  // Tick methods called by other tick methods will become tasks.
  void tick2() {
    reg2 = reg2 + 1;
  }
};
