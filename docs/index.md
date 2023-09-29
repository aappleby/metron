# Metron, a tool for writing synthesizable Verilog in C++

## [TL;DR - Demo!](demo/index.html)

[Temporal Logic](TemporalLogic.md)

[Tutorial](tutorial/index.html)

[User Guide](UserGuide.md)


```
#include "metron/metron_tools.h"

class Counter {
public:

  logic<8> counter;

  void tick(logic<1> reset) {
    if (reset) {
      counter = 0;
    } else {
      counter = counter + 1;
    }
  }
};
```

```
`include "metron_tools.sv"

module Counter
(
  input logic clock,
  output logic[7:0] counter,
  input logic tick_reset
);
/*public:*/

  task automatic tick(logic reset);
    if (reset) begin
      counter <= 0;
    end else begin
      counter <= counter + 1;
    end
  endtask
  always_ff @(posedge clock) tick(tick_reset);

endmodule
```
