ninja && bin/metron -s -e examples/MetroBoySPU2.h && echo "verilatorin..." && verilator -Isrc --lint-only examples/MetroBoySPU2.sv
