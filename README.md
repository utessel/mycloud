For WD MyCloud Device
=====================

Get access when device is not booting:

If it is already open: 
try a serial cable: ![Image of serial port](serial.jpg)

You need a 3,3V cable, and This costs just about 5 Euro.

You don't need to solder anything to the board, but then
you need an edge Connector with the right distances.

Without opening
---------------

The Bootloader (which is stored in FLASH, so it is not related
to anything on the disk) can be triggered to use TFTP when 
receiving a "magic packet".

Trigger TFTP
------------

Use the code of rawping.c to send this:

Compile:
    gcc -o rawping rawping.c

Start:
    sudo rawping eth0 <mac of device>

This will trigger a download via tftp of a file called 'startup.sh'.

The different Startup Files
---------------------------



Complete new setup
==================

use `build-sys.sh` for this:

Requirements:
- it needs the tar file with GPL Sources from WD
- extract this to the same directoy as the build-sys script
- you have to install a cross compiler:
    sudo apt-get install g++-4.7-arm-linux-gnueabihf
- the script itself will download and compile busybox and finally generate a uImage that can be loaded via tftp to the box


