echo
echo "Metron:"
#rm -f examples/scratch.sv
#bin/metron -d -e -c examples/scratch.h -o examples/scratch.sv
echo
echo "GCC:"
g++ -Isrc --std=gnu++2a -fsyntax-only -c examples/scratch.h
echo
echo "Verilator:"
verilator -Isrc --lint-only examples/scratch.sv
echo
echo "Yosys:"
yosys -q -p "read_verilog -Isrc -sv examples/scratch.sv;"
echo
echo "Icarus:"
iverilog -g2012 -Wall -Isrc -o obj/scratch.o examples/scratch.sv
echo
