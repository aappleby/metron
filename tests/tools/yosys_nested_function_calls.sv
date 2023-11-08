module Module();
int a;
int b;

always_comb begin
a = f1();
end

function int f1();
f1 = f2();
endfunction

function int f2();
f2 = b;
endfunction

endmodule
