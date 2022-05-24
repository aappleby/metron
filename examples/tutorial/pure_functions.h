class Module {
public:
  int get_signal1(int x) {
    return reg1 + my_pure_func(x);
  }

  void update_reg() {
    reg1 = my_pure_func(reg1);
  }

private:
  int reg1;

  // This pure function is called elsewhere in the module, so it would not be added to the port list even if it were public.
  int my_pure_func(int x) const {
    return x + 17;
  }

  // This pure function would appear in the port list if it were public. Uncomment the line below to see the difference.
//public:
  int my_pure_func2(int x) const {
    return x + 17;
  }
};
