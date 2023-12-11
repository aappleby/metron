`include "metron/metron_tools.sv"

module Module (
  // tick() ports
  input logic[15:0] tick_addr
);
 /*public:*/

  always_comb begin : tick
    case (tick_addr)

      // metron didn't like the block without {}
      16'hFF1C: begin
        case (1)
          0:  begin /*break;*/ end
        endcase
        /*break;*/
      end
    endcase
  end

  logic[7:0] data_out_;
endmodule
