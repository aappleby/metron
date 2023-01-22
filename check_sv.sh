echo $1
echo
echo "Verilator:"
verilator -Isrc --lint-only $1
echo
echo "Yosys:"
yosys -q -p "read_verilog -Isrc -sv tests/metron_sv/structs_as_ports.sv;"
echo
echo "Icarus:"
iverilog -g2012 -Wall -Isrc -o bin/temp.o $1
