## Generated SDC file "cyclomod_xip.sdc"

## Copyright (C) 2025  Altera Corporation. All rights reserved.
## Your use of Altera Corporation's design tools, logic functions 
## and other software and tools, and any partner logic 
## functions, and any output files from any of the foregoing 
## (including device programming or simulation files), and any 
## associated documentation or information are expressly subject 
## to the terms and conditions of the Altera Program License 
## Subscription Agreement, the Altera Quartus Prime License Agreement,
## the Altera IP License Agreement, or other applicable license
## agreement, including, without limitation, that your use is for
## the sole purpose of programming logic devices manufactured by
## Altera and sold by Altera or its authorized distributors.  Please
## refer to the Altera Software License Subscription Agreements 
## on the Quartus Prime software download page.


## VENDOR  "Altera"
## PROGRAM "Quartus Prime"
## VERSION "Version 25.1std.0 Build 1129 10/21/2025 SC Lite Edition"

## DATE    "Sat Dec 13 15:41:48 2025"

##
## DEVICE  "10CL010YM164A7G"
##


#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************

create_clock -name {sck} -period 10.00 -waveform { 0.000 5.00 } [get_ports {sck}]
create_clock -name {sys} -period 83.000 -waveform { 0.000 41.500 } [get_ports {clk}]
create_clock -name {qmi} -period 10.00 -waveform { 0.000 5.00 } 

#**************************************************************
# Create Generated Clock
#**************************************************************



#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************



#**************************************************************
# Set Input Delay
#**************************************************************

set_input_delay -add_delay -rise -max -clock [get_clocks {qmi}]  4.000 [get_ports {d[0]}]
set_input_delay -add_delay -rise -min -clock [get_clocks {qmi}]  3.000 [get_ports {d[0]}]
set_input_delay -add_delay -rise -max -clock [get_clocks {qmi}]  4.000 [get_ports {d[1]}]
set_input_delay -add_delay -rise -min -clock [get_clocks {qmi}]  3.000 [get_ports {d[1]}]
set_input_delay -add_delay -rise -max -clock [get_clocks {qmi}]  4.000 [get_ports {d[2]}]
set_input_delay -add_delay -rise -min -clock [get_clocks {qmi}]  3.000 [get_ports {d[2]}]
set_input_delay -add_delay -rise -max -clock [get_clocks {qmi}]  4.000 [get_ports {d[3]}]
set_input_delay -add_delay -rise -min -clock [get_clocks {qmi}]  3.000 [get_ports {d[3]}]
set_input_delay -add_delay -rise -max -clock [get_clocks {qmi}]  4.000 [get_ports {csn}]
set_input_delay -add_delay -rise -min -clock [get_clocks {qmi}]  3.000 [get_ports {csn}]


#**************************************************************
# Set Output Delay
#**************************************************************

set_output_delay -add_delay -rise -max -clock [get_clocks {qmi}]  6.000 [get_ports {d[0]}]
set_output_delay -add_delay -rise -min -clock [get_clocks {qmi}]  0.000 [get_ports {d[0]}]
set_output_delay -add_delay -rise -max -clock [get_clocks {qmi}]  6.000 [get_ports {d[1]}]
set_output_delay -add_delay -rise -min -clock [get_clocks {qmi}]  0.000 [get_ports {d[1]}]
set_output_delay -add_delay -rise -max -clock [get_clocks {qmi}]  6.000 [get_ports {d[2]}]
set_output_delay -add_delay -rise -min -clock [get_clocks {qmi}]  0.000 [get_ports {d[2]}]
set_output_delay -add_delay -rise -max -clock [get_clocks {qmi}]  6.000 [get_ports {d[3]}]
set_output_delay -add_delay -rise -min -clock [get_clocks {qmi}]  0.000 [get_ports {d[3]}]


#**************************************************************
# Set Clock Groups
#**************************************************************



#**************************************************************
# Set False Path
#**************************************************************



#**************************************************************
# Set Multicycle Path
#**************************************************************



#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

