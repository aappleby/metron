# cmd.exe /c C:/Xilinx/Vivado/2021.2/bin/vivado.bat -mode tcl

# does not work yet!

# disable 'use before decl' warnings
set_msg_config -id {HDL 9-3527} -suppress
set_msg_config -id {HDL 9-2613} -suppress

# Unused sequential element was removed
set_msg_config -id {Synth 8-6014} -suppress

cd d:/Metron
pwd

# smallest artix part
set part xc7a12t
set module toplevel

create_project $module -in_memory -part $part
add_files example_rvtiny/generated/toplevel.sv
#read_xdc file.xdc
synth_design -name $module -top $module -part $part
#write_checkpoint -force $module-synth.dcp
link_design
opt_design
place_design
#write_checkpoint -force $module-place.dcp
phys_opt_design
route_design
#write_checkpoint -force $module-routed.dcp
report_timing_summary
write_bitstream $module.bit
