echo Will boot via tftp
timeout -c 2
addpart /dev/mem 8M@0x3008000(uImage)
tftp uImage /dev/mem.uImage
#bootargs="console=ttyS0,115200n8, init=/bin/sh "
bootargs="console=ttyS0,115200n8, "
bootargs="$bootargs mac_addr=$eth0.ethaddr panic=3"
bootm /dev/mem.uImage 
