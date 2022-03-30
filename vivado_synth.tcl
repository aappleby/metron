# cmd.exe /k C:/Xilinx/Vivado/2021.2/bin/vivado.bat -mode tcl

# does not work yet!

# disable 'use before decl' warnings
set_msg_config -id {HDL 9-3527} -suppress

cd d:/Metron
pwd

# smallest artix part
set part xc7a12t
set module toplevel

create_project $module -in_memory -part $part
add_files file.vhd
read_xdc file.xdc
synth_design -name $module -top $module -part $partname
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
