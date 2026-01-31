# CycloMod XIP

This builds on Pico-JBC, adding XIP support for communication with the Cyclone 10LP on the CycloMod board.

Be sure to subscribe to the [Cyclomod Crowd Supply Campaign](https://www.crowdsupply.com/steiert-solutions/cyclomod) for updates on when and how to get CycloMod.

## CycloMod RTL

The provided CycloMod RTL project implements a dual port memory with one port connected to the XIP interface.  The second port is connected to a PWM so you can see the effect of writes on the USR LED.

32KB of memory are implemented.

The customized firmware includes a memory test function using PRBS sequence.

## pico-jbc

Altera JBC implementation for Raspberry Pi Pico boards, adapted (forked) for use with CycloMod board

## CycloMod

* Build the project to create pico_jbc.uf2
* Generate uncompressed JBC file
* Add jbc file to uf2 file (0x10F00000 base address)
* drag-n-drop combined .uf2 file onto RP drive

```
python3 util/jbcuf2.py -u build/pico_jbc.uf2 -b 0x10F00000 -j path-to-image.jbc -d "description" -a "CONFIGURE" -o output.uf2
```

## Loading JBC file separately

Use jbc2u.py to generate a stand alone .uf2 file to load the JBC file into flash

* Use family -f 0xe48bff57 for RP2350 

```
python3 util/jbc2u.py -f 0xe48bff57 -b 0x10F00000 -j path-to-image.jbc -d "description" -a "CONFIGURE" -o output.uf2
```

## JBC to UF2

### -f Family

jbcuf2.py will automatically use the family from the provided .uf2 file.
Jbc2u.py requires manual setting:
 * RP2350:  0xe48bff57

### -b Base Address

This must match the `JUF2_ADDRESS` defined in jbipico.h
For CycloMod, the FPGA image should fit in the top 1MB, so use 0x10F00000 to leave the rest for RP2350 and other data.

### -a Default Action

The default action allows you to specify an action to run immediately at startup.  Use "CONFIGURE" for SRAM based devices like Cyclone.

### -d Description

This is any text you want to include to identify the image

## Generating an uncompressed JBC File

JBC is the STAPL JAM Byte Code format.  Quartus can generate these files.  In order to fit within the memory constraints of a microcontroller, compression is not supported.  Instructions for generating an uncompressed JBC file can be [found here](https://www.intel.com/content/www/us/en/support/programmable/articles/000079036.html).  

To build get an uncompressed .jbc file:

 1.  Go to Device \> Device and Pin Options \> Programming Files and select the "JEDEC STAPL Format File (.jam)" checkbox, or add ```set_global_assignment -name GENERATE_JAM_FILE ON``` to your Quartus project settings
 2.  Run ```quartus_jbcc.exe -n <filename>.jam <filename>.jbc```
