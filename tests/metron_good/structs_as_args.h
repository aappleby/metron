#include "metron/metron_tools.h"

// Passing structs to functions that turn into always_comb or always_ff
// should prepend the function name to the struct name.

//------------------------------------------------------------------------------

struct tilelink_a {
  logic<32> a_data;
};

class block_ram {
public:

  logic<32> unshell(tilelink_a tla) {
    return tla.a_data;
  }

  void tick(tilelink_a tla) {
    data_ = tla.a_data;
  }

  logic<32> data_;
};

//------------------------------------------------------------------------------
