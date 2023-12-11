#include "metron/metron_tools.h"

class alu_control {
 public:
  logic<7> inst_funct7;
  logic<5> op_funct;

 public:
  void tock_alu_function() {

    if (inst_funct7[5])
      op_funct = 1;
    else
      op_funct = 2;
  }
};
