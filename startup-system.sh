addpart /dev/mem 3M@0x3008000(uImage)
btn_status=0
get_button_status
sata 
satapart 0x3008000 5 0x5000
sata stop
bootargs="console=ttyS0,115200n8, init=/sbin/init"
bootargs="$bootargs root=/dev/md0 raid=autodetect"
bootargs="$bootargs rootfstype=ext3 rw noinitrd debug initcall_debug swapaccount=1 panic=3"
bootargs="$bootargs mac_addr=$eth0.ethaddr"
bootargs="$bootargs model=$model serial=$serial board_test=$board_test btn_status=$btn_status"
bootm /dev/mem.uImage
