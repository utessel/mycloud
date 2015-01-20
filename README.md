For WD MyCloud Device:

get access when device is not booting:

if already open, try serial cable, see serial.jpg

Bootloader can be triggered to use TFTP when receiving "magic packet".

Use rawping.c to send this:
gcc -o rawping rawping.c
sudo rawping eth0 <mac of device>

This will trigger a download via tftp of 'startup.sh'

That file will trigger a download of "uImage":

To get this, get the GPL Sources from WD.
Install a cross compiler:
sudo apt-get install g++-4.7-arm-linux-gnueabihf

export ARCH=armhf
export 

compile the kernel:
install 

If you need a working busybox:
use build-sys.sh

